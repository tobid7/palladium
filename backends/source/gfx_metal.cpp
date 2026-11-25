#include <pd/lithium/formatters.hpp>
#include <pd_system/gfx_metal.hpp>

#if defined(PD_ENABLE_METAL) && defined(__APPLE__)
#if !defined(__OBJC__) || !__has_feature(objc_arc)
#error "gfx_metal.cpp must be compiled as Objective-C++ with -fobjc-arc"
#endif

#import <Metal/Metal.h>
#include <TargetConditionals.h>

#include <algorithm>
#include <cstring>
#include <pd/drivers/drivers.hpp>
#include <pd_system/shaders.hpp>
#include <spirv-helper.hpp>
#include <string>
#include <unordered_map>
#include <vector>

namespace PD {
static constexpr NSUInteger kVtxBufIdx = 30;
static constexpr NSUInteger kUboIdx = 1;
static constexpr NSUInteger kPushIdx = 2;
static constexpr NSUInteger kTexIdx = 0;
static constexpr NSUInteger kSmpIdx = 0;
static constexpr size_t kNoOffset = static_cast<size_t>(-1);

struct MslBlockInfo {
  size_t Size = 0;
  std::unordered_map<std::string, size_t> Offsets;

  void Merge(const std::vector<SpirvHelper::MSLBlockInfo>& blocks) {
    for (auto& b : blocks) {
      Size = std::max(Size, b.Size);
      Offsets.insert(b.Offsets.begin(), b.Offsets.end());
    }
  }
};

static std::string SPV2MSL(const std::vector<unsigned int>& spv,
                           std::string& entry, MslBlockInfo& ubo,
                           MslBlockInfo& push) {
  SpirvHelper::MSLOptions opts;
#if TARGET_OS_IPHONE
  opts.IOS = true;
#endif
  opts.VersionMajor = 2;
  opts.VersionMinor = 1;
  opts.FixupClipspace = true;  // GL z [-1, 1] -> Metal z [0, 1]
  opts.UniformBufferSlot = kUboIdx;
  opts.PushConstantSlot = kPushIdx;
  opts.TextureSlot = kTexIdx;
  opts.SamplerSlot = kSmpIdx;
  auto res = SpirvHelper::SPV2MSL(spv, opts);
  ubo.Merge(res.UniformBuffers);
  push.Merge(res.PushConstants);
  entry = res.EntryPoint;
  return res.Code;
}

static id<MTLFunction> CompileMslFunc(id<MTLDevice> dev, const std::string& src,
                                      const std::string& entry) {
  NSError* err = nil;
  id<MTLLibrary> lib =
      [dev newLibraryWithSource:[NSString stringWithUTF8String:src.c_str()]
                        options:nil
                          error:&err];
  if (!lib) {
    PDERR("GfxMetal: MSL compile failed: {}",
          err ? err.localizedDescription.UTF8String : "unknown");
    return nil;
  }
  id<MTLFunction> fn =
      [lib newFunctionWithName:[NSString stringWithUTF8String:entry.c_str()]];
  if (!fn) {
    PDERR("GfxMetal: Entry point '{}' not found", entry);
  }
  return fn;
}

struct GfxMetal::Impl {
  static constexpr int kFramesInFlight = 3;

  struct Tex {
    id<MTLTexture> Handle = nil;
    bool Linear = true;
    bool IsA8 = false;
  };

  id<MTLDevice> Device = nil;

  // Pipeline
  id<MTLFunction> VFunc = nil;
  id<MTLFunction> FFunc = nil;
  id<MTLRenderPipelineState> Pipeline = nil;
  id<MTLDepthStencilState> DepthState = nil;
  u32 PipeColor = 0, PipeDepth = 0, PipeStencil = 0, PipeSamples = 0;

  // Textures
  id<MTLSamplerState> SamplerLinear = nil;
  id<MTLSamplerState> SamplerNearest = nil;
  std::unordered_map<TextureID, Tex> Textures;
  TextureID NextTexID = 1;  // 0 = no texture

  id<MTLBuffer> VBuf[kFramesInFlight];
  id<MTLBuffer> IBuf[kFramesInFlight];
  int Slot = 0;
  bool HasUpload = false;
  id<MTLCommandBuffer> SlotCmdBuf = nil;
  dispatch_semaphore_t InFlight = nil;

  std::vector<u8> UboData;
  std::vector<u8> PushData;
  size_t UboOffProj = kNoOffset;
  size_t PushOffAlfa = kNoOffset;
  size_t PushOffSDF = kNoOffset;

  bool EnsurePipeline(const GfxMetalCtx& ctx) {
    if (Pipeline && PipeColor == ctx.ColorFormat &&
        PipeDepth == ctx.DepthFormat && PipeStencil == ctx.StencilFormat &&
        PipeSamples == ctx.SampleCount) {
      return true;
    }
    if (!Device || !VFunc || !FFunc) return false;

    MTLVertexDescriptor* vd = [MTLVertexDescriptor vertexDescriptor];
    vd.attributes[0].format = MTLVertexFormatFloat2;
    vd.attributes[0].offset = offsetof(PD::Li::Vertex, pos);
    vd.attributes[0].bufferIndex = kVtxBufIdx;
    vd.attributes[1].format = MTLVertexFormatFloat2;
    vd.attributes[1].offset = offsetof(PD::Li::Vertex, uv);
    vd.attributes[1].bufferIndex = kVtxBufIdx;
    vd.attributes[2].format = MTLVertexFormatUChar4Normalized;
    vd.attributes[2].offset = offsetof(PD::Li::Vertex, color);
    vd.attributes[2].bufferIndex = kVtxBufIdx;
    vd.layouts[kVtxBufIdx].stride = sizeof(PD::Li::Vertex);
    vd.layouts[kVtxBufIdx].stepFunction = MTLVertexStepFunctionPerVertex;

    MTLRenderPipelineDescriptor* pd = [MTLRenderPipelineDescriptor new];
    pd.label = @"PD::GfxMetal";
    pd.vertexFunction = VFunc;
    pd.fragmentFunction = FFunc;
    pd.vertexDescriptor = vd;
    pd.rasterSampleCount = ctx.SampleCount ? ctx.SampleCount : 1;
    pd.depthAttachmentPixelFormat =
        static_cast<MTLPixelFormat>(ctx.DepthFormat);
    pd.stencilAttachmentPixelFormat =
        static_cast<MTLPixelFormat>(ctx.StencilFormat);

    MTLRenderPipelineColorAttachmentDescriptor* ca = pd.colorAttachments[0];
    ca.pixelFormat = static_cast<MTLPixelFormat>(ctx.ColorFormat);
    ca.blendingEnabled = YES;
    ca.rgbBlendOperation = MTLBlendOperationAdd;
    ca.alphaBlendOperation = MTLBlendOperationAdd;
    ca.sourceRGBBlendFactor = MTLBlendFactorSourceAlpha;
    ca.destinationRGBBlendFactor = MTLBlendFactorOneMinusSourceAlpha;
    ca.sourceAlphaBlendFactor = MTLBlendFactorSourceAlpha;
    ca.destinationAlphaBlendFactor = MTLBlendFactorOneMinusSourceAlpha;

    NSError* err = nil;
    Pipeline = [Device newRenderPipelineStateWithDescriptor:pd error:&err];
    if (!Pipeline) {
      PDERR("GfxMetal: Failed to create pipeline: {}",
            err ? err.localizedDescription.UTF8String : "unknown");
      return false;
    }
    PipeColor = ctx.ColorFormat;
    PipeDepth = ctx.DepthFormat;
    PipeStencil = ctx.StencilFormat;
    PipeSamples = ctx.SampleCount;
    return true;
  }

  void EnsureBuffer(id<MTLBuffer> __strong& buf, size_t size) {
    if (buf && buf.length >= size) return;
    buf = [Device newBufferWithLength:size + size / 2
                              options:MTLResourceStorageModeShared |
                                      MTLResourceCPUCacheModeWriteCombined];
  }

  void WaitIdle() {
    if (!InFlight) return;
    for (int i = 0; i < kFramesInFlight; i++) {
      dispatch_semaphore_wait(InFlight, DISPATCH_TIME_FOREVER);
    }
    for (int i = 0; i < kFramesInFlight; i++) {
      dispatch_semaphore_signal(InFlight);
    }
  }
};

void GfxMetal::SysInit() {
  if (impl) return;
  PDLOG("GfxMetal::SysInit();");
  Flags |= PDGfxBackendFlags_HasClipRect;
  impl = new Impl();

  if (pCtx && pCtx->Device) {
    impl->Device = (__bridge id<MTLDevice>)pCtx->Device;
  } else {
    PDERR("GfxMetal::SysInit: No GfxMetalCtx/Device given, using default");
    impl->Device = MTLCreateSystemDefaultDevice();
  }
  if (!impl->Device) {
    PDERR("GfxMetal::SysInit: No Metal device available");
    return;
  }

  std::string vcode, fcode, ventry, fentry;
  MslBlockInfo ubo, push;
  try {
    vcode = SPV2MSL(std::vector<unsigned int>(
                        Shaders::VertexShader,
                        Shaders::VertexShader + (sizeof(Shaders::VertexShader) /
                                                 sizeof(unsigned int))),
                    ventry, ubo, push);
    fcode = SPV2MSL(
        std::vector<unsigned int>(
            Shaders::FragmentShader,
            Shaders::FragmentShader +
                (sizeof(Shaders::FragmentShader) / sizeof(unsigned int))),
        fentry, ubo, push);
  } catch (const std::exception& e) {
    PDERR("GfxMetal::SysInit: SPIR-V -> MSL failed: {}", e.what());
    return;
  }
  PDLOG("Vertex: \n{}", vcode);
  PDLOG("Fragment: \n{}", fcode);

  impl->VFunc = CompileMslFunc(impl->Device, vcode, ventry);
  impl->FFunc = CompileMslFunc(impl->Device, fcode, fentry);

  auto off = [](const MslBlockInfo& b, const char* name) {
    auto it = b.Offsets.find(name);
    return it == b.Offsets.end() ? kNoOffset : it->second;
  };
  impl->UboData.assign(ubo.Size, 0);
  impl->PushData.assign(push.Size, 0);
  impl->UboOffProj = off(ubo, "projection");
  impl->PushOffAlfa = off(push, "alfa");
  impl->PushOffSDF = off(push, "is_sdf");
  if (impl->UboOffProj == kNoOffset || impl->PushOffAlfa == kNoOffset ||
      impl->PushOffSDF == kNoOffset) {
    PDERR("GfxMetal: Uniform not found (projection={}, alfa={}, is_sdf={})",
          impl->UboOffProj != kNoOffset, impl->PushOffAlfa != kNoOffset,
          impl->PushOffSDF != kNoOffset);
  }

  MTLSamplerDescriptor* sd = [MTLSamplerDescriptor new];
  sd.sAddressMode = MTLSamplerAddressModeClampToEdge;
  sd.tAddressMode = MTLSamplerAddressModeClampToEdge;
  sd.minFilter = sd.magFilter = MTLSamplerMinMagFilterLinear;
  impl->SamplerLinear = [impl->Device newSamplerStateWithDescriptor:sd];
  sd.minFilter = sd.magFilter = MTLSamplerMinMagFilterNearest;
  impl->SamplerNearest = [impl->Device newSamplerStateWithDescriptor:sd];

  MTLDepthStencilDescriptor* dd = [MTLDepthStencilDescriptor new];
  dd.depthCompareFunction = MTLCompareFunctionAlways;
  dd.depthWriteEnabled = NO;
  impl->DepthState = [impl->Device newDepthStencilStateWithDescriptor:dd];

  impl->InFlight = dispatch_semaphore_create(Impl::kFramesInFlight);
  if (pCtx) impl->EnsurePipeline(*pCtx);
}

void GfxMetal::SysDeinit() {
  if (!impl) return;
  impl->WaitIdle();
  delete impl;
  impl = nullptr;
  PDLOG("GfxMetal::SysDeinit()");
}

void GfxMetal::SysReset() {
  if (!impl || !pCtx || !pCtx->Encoder) return;
  if (!impl->EnsurePipeline(*pCtx)) return;
  auto enc = (__bridge id<MTLRenderCommandEncoder>)pCtx->Encoder;
  [enc setRenderPipelineState:impl->Pipeline];
  [enc setDepthStencilState:impl->DepthState];
  [enc setCullMode:MTLCullModeNone];
}

void GfxMetal::Submit(size_t count, size_t start) {
  if (!impl || !pCtx || !pCtx->Encoder || !impl->HasUpload) return;
  if (!impl->EnsurePipeline(*pCtx)) return;
  auto enc = (__bridge id<MTLRenderCommandEncoder>)pCtx->Encoder;

  [enc setRenderPipelineState:impl->Pipeline];
  [enc setDepthStencilState:impl->DepthState];
  [enc setCullMode:MTLCullModeNone];
  [enc setVertexBuffer:impl->VBuf[impl->Slot] offset:0 atIndex:kVtxBufIdx];

  if (!impl->UboData.empty()) {
    if (impl->UboOffProj != kNoOffset) {
      const size_t n =
          std::min(sizeof(float) * 16, impl->UboData.size() - impl->UboOffProj);
      std::memcpy(impl->UboData.data() + impl->UboOffProj, Projection.m.data(),
                  n);
    }
    [enc setVertexBytes:impl->UboData.data()
                 length:impl->UboData.size()
                atIndex:kUboIdx];
  }

  BindTexture(CurrentTex);

  [enc drawIndexedPrimitives:MTLPrimitiveTypeTriangle
                  indexCount:count
                   indexType:MTLIndexTypeUInt16
                 indexBuffer:impl->IBuf[impl->Slot]
           indexBufferOffset:start * sizeof(u16)];
}

void GfxMetal::BindTexture(TextureID tex_id) {
  if (!impl || !pCtx || !pCtx->Encoder) return;
  auto enc = (__bridge id<MTLRenderCommandEncoder>)pCtx->Encoder;

  auto it = impl->Textures.find(tex_id);
  if (it == impl->Textures.end()) {
    it = impl->Textures.find(pWhite.GetID());
    if (it == impl->Textures.end()) return;
  }
  [enc setFragmentTexture:it->second.Handle atIndex:kTexIdx];
  [enc setFragmentSamplerState:(it->second.Linear ? impl->SamplerLinear
                                                  : impl->SamplerNearest)
                       atIndex:kSmpIdx];

  if (!impl->PushData.empty()) {
    auto write = [&](size_t off, u32 v) {
      if (off != kNoOffset && off + sizeof(u32) <= impl->PushData.size()) {
        std::memcpy(impl->PushData.data() + off, &v, sizeof(u32));
      }
    };
    write(impl->PushOffAlfa, it->second.IsA8 ? 1 : 0);
    write(impl->PushOffSDF, CurrentTexIsSDF ? 1 : 0);
    [enc setFragmentBytes:impl->PushData.data()
                   length:impl->PushData.size()
                  atIndex:kPushIdx];
  }
}

Li::Texture GfxMetal::LoadTexture(const std::vector<PD::u8>& pixels, int w,
                                  int h, TextureFormat type,
                                  TextureFilter filter) {
  const size_t expected = static_cast<size_t>(w) * static_cast<size_t>(h) *
                          static_cast<size_t>(Li::TextureFormat2Bpp(type));
  if (w <= 0 || h <= 0 || pixels.size() != expected) {
    PDERR("Failed to load Texture due to Size mismatch: {} != {}",
          pixels.size(), expected);
    return Li::Texture();
  }
  if (!impl || !impl->Device) {
    PDERR("GfxMetal::LoadTexture: Driver not initialized");
    return Li::Texture();
  }

  MTLPixelFormat pf = MTLPixelFormatRGBA8Unorm;
  const u8* src = pixels.data();
  size_t bpr = static_cast<size_t>(w) * 4;
  std::vector<u8> conv;

  if (type == TextureFormat::RGB24) {
    // Metal has no 24 bit format -> RGBA
    const size_t n = static_cast<size_t>(w) * static_cast<size_t>(h);
    conv.resize(n * 4);
    for (size_t i = 0; i < n; i++) {
      conv[i * 4 + 0] = pixels[i * 3 + 0];
      conv[i * 4 + 1] = pixels[i * 3 + 1];
      conv[i * 4 + 2] = pixels[i * 3 + 2];
      conv[i * 4 + 3] = 0xFF;
    }
    src = conv.data();
  } else if (type == TextureFormat::A8) {
    pf = MTLPixelFormatR8Unorm;
    bpr = static_cast<size_t>(w);
  }

  MTLTextureDescriptor* d =
      [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:pf
                                                         width:w
                                                        height:h
                                                     mipmapped:NO];
  d.usage = MTLTextureUsageShaderRead;
  if (type == TextureFormat::A8) {
    // Same swizzle as the OpenGL3 backend: (1, 1, 1, R)
    if (@available(macOS 10.15, iOS 13.0, *)) {
      d.swizzle = MTLTextureSwizzleChannelsMake(
          MTLTextureSwizzleOne, MTLTextureSwizzleOne, MTLTextureSwizzleOne,
          MTLTextureSwizzleRed);
    } else {
      PDERR("GfxMetal: A8 swizzle requires macOS 10.15 / iOS 13");
    }
  }

  id<MTLTexture> t = [impl->Device newTextureWithDescriptor:d];
  if (!t) {
    PDERR("GfxMetal::LoadTexture: newTextureWithDescriptor failed");
    return Li::Texture();
  }
  [t replaceRegion:MTLRegionMake2D(0, 0, w, h)
       mipmapLevel:0
         withBytes:src
       bytesPerRow:bpr];

  const TextureID tid = impl->NextTexID++;
  impl->Textures[tid] = {t, filter == TextureFilter::Linear,
                         type == TextureFormat::A8};

  Li::Texture res;
  res.SetID(tid);
  res.SetSize(w, h);
  res.SetUV(0.f, 0.f, 1.f, 1.f);
  RegisterTexture(res);
  PDLOG("GfxMetal::LoadTexture -> {{ {} }}, [{}, {}]", res, type, filter);
  return res;
}

void GfxMetal::DeleteTexture(const Li::Texture& tex) {
  UnregisterTexture(tex);
  if (impl) impl->Textures.erase(tex.GetID());
}

void GfxMetal::UploadPools() {
  if (!impl) return;
  impl->HasUpload = false;
  if (!pCtx || !pCtx->CmdBuf || !impl->Device) return;

  const size_t vsz = GetVertexPoolSize() * sizeof(PD::Li::Vertex);
  const size_t isz = GetIndexPoolSize() * sizeof(u16);
  if (!vsz || !isz) return;

  auto cmd = (__bridge id<MTLCommandBuffer>)pCtx->CmdBuf;
  if (cmd != impl->SlotCmdBuf) {
    dispatch_semaphore_wait(impl->InFlight, DISPATCH_TIME_FOREVER);
    impl->Slot = (impl->Slot + 1) % Impl::kFramesInFlight;
    impl->SlotCmdBuf = cmd;
    dispatch_semaphore_t sem = impl->InFlight;
    [cmd addCompletedHandler:^(id<MTLCommandBuffer>) {
      dispatch_semaphore_signal(sem);
    }];
  }

  impl->EnsureBuffer(impl->VBuf[impl->Slot], vsz);
  impl->EnsureBuffer(impl->IBuf[impl->Slot], isz);
  std::memcpy([impl->VBuf[impl->Slot] contents], GetVertexBufPtr(0), vsz);
  std::memcpy([impl->IBuf[impl->Slot] contents], GetIndexBufPtr(0), isz);
  impl->HasUpload = true;
}

void GfxMetal::ClipRect() {
  if (!impl || !pCtx || !pCtx->Encoder) return;
  auto enc = (__bridge id<MTLRenderCommandEncoder>)pCtx->Encoder;

  // Clip is in viewport units, the scissor wants framebuffer pixels
  const fvec2& s = FramebufferScale;
  const long vw = std::max(0L, static_cast<long>(ViewPort.x * s.x));
  const long vh = std::max(0L, static_cast<long>(ViewPort.y * s.y));
  MTLScissorRect r = {0, 0, static_cast<NSUInteger>(vw),
                      static_cast<NSUInteger>(vh)};
  if (CurrentHasClip) {
    const long x0 = std::clamp(static_cast<long>(CurrentClip.x * s.x), 0L, vw);
    const long y0 = std::clamp(static_cast<long>(CurrentClip.y * s.y), 0L, vh);
    const long x1 = std::clamp(
        static_cast<long>((CurrentClip.x + CurrentClip.z) * s.x), 0L, vw);
    const long y1 = std::clamp(
        static_cast<long>((CurrentClip.y + CurrentClip.w) * s.y), 0L, vh);
    r.x = static_cast<NSUInteger>(x0);
    r.y = static_cast<NSUInteger>(y0);
    r.width = static_cast<NSUInteger>(std::max(0L, x1 - x0));
    r.height = static_cast<NSUInteger>(std::max(0L, y1 - y0));
  }
  [enc setScissorRect:r];
}
}  // namespace PD
#else
namespace PD {
void GfxMetal::SysInit() {
  PDERR(
      "GfxMetal::SysInit: Metal Driver is not included in "
      "pd-system");
}
void GfxMetal::SysDeinit() {}
void GfxMetal::Submit(size_t count, size_t start) {}
void GfxMetal::BindTexture(TextureID id) {}
void GfxMetal::SysReset() {}
Li::Texture GfxMetal::LoadTexture(const std::vector<PD::u8>& pixels, int w,
                                  int h, TextureFormat type,
                                  TextureFilter filter) {
  return Li::Texture();
}
void GfxMetal::DeleteTexture(const Li::Texture& tex) {}
void GfxMetal::UploadPools() {}
void GfxMetal::ClipRect() {}
}  // namespace PD
#endif
