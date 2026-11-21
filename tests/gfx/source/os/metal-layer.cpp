#include <os/metal-layer.hpp>

#if defined(PD_ENABLE_METAL) && defined(__APPLE__)
#import <AppKit/AppKit.h>
#import <Metal/Metal.h>
#import <QuartzCore/CAMetalLayer.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_COCOA
#include <GLFW/glfw3native.h>

namespace PD {
struct MetalLayer::Impl {
  id<MTLDevice> Device = nil;
  id<MTLCommandQueue> Queue = nil;
  CAMetalLayer* Layer = nil;
  // Current frame
  id<CAMetalDrawable> Drawable = nil;
  id<MTLCommandBuffer> CmdBuf = nil;
  id<MTLRenderCommandEncoder> Encoder = nil;
};

bool MetalLayer::Create(GLFWwindow* win, GfxMetalCtx& ctx) {
  if (impl) return true;
  impl = new Impl();
  impl->Device = MTLCreateSystemDefaultDevice();
  if (!impl->Device) {
    PDERR("MetalLayer: No Metal device");
    return false;
  }
  impl->Queue = [impl->Device newCommandQueue];

  impl->Layer = [CAMetalLayer layer];
  impl->Layer.device = impl->Device;
  impl->Layer.pixelFormat = MTLPixelFormatBGRA8Unorm;
  impl->Layer.framebufferOnly = YES;

  NSWindow* nswin = glfwGetCocoaWindow(win);
  nswin.contentView.layer = impl->Layer;
  nswin.contentView.wantsLayer = YES;

  ctx.Device = (__bridge void*)impl->Device;
  ctx.ColorFormat = static_cast<u32>(MTLPixelFormatBGRA8Unorm);
  ctx.DepthFormat = static_cast<u32>(MTLPixelFormatInvalid);
  ctx.StencilFormat = static_cast<u32>(MTLPixelFormatInvalid);
  ctx.SampleCount = 1;
  return true;
}

void MetalLayer::Destroy(GfxMetalCtx& ctx) {
  ctx = GfxMetalCtx();
  if (impl) {
    delete impl;
    impl = nullptr;
  }
}

void MetalLayer::BeginFrame(GfxMetalCtx& ctx, const ivec2& size) {
  if (impl && impl->Encoder) {
    @autoreleasepool {
      [impl->Encoder endEncoding];
      MTLRenderPassDescriptor* rpd =
          [MTLRenderPassDescriptor renderPassDescriptor];
      rpd.colorAttachments[0].texture = impl->Drawable.texture;
      rpd.colorAttachments[0].loadAction = MTLLoadActionClear;
      rpd.colorAttachments[0].storeAction = MTLStoreActionStore;
      rpd.colorAttachments[0].clearColor =
          MTLClearColorMake(0.1, 0.1, 0.1, 1.0);
      impl->Encoder = [impl->CmdBuf renderCommandEncoderWithDescriptor:rpd];
    }
    ctx.CmdBuf = (__bridge void*)impl->CmdBuf;
    ctx.Encoder = (__bridge void*)impl->Encoder;
    return;
  }
  ctx.CmdBuf = nullptr;
  ctx.Encoder = nullptr;
  if (!impl || size.x <= 0 || size.y <= 0) return;
  @autoreleasepool {
    impl->Layer.drawableSize = CGSizeMake(size.x, size.y);
    impl->Drawable = [impl->Layer nextDrawable];
    if (!impl->Drawable) return;  // skip frame

    MTLRenderPassDescriptor* rpd =
        [MTLRenderPassDescriptor renderPassDescriptor];
    rpd.colorAttachments[0].texture = impl->Drawable.texture;
    rpd.colorAttachments[0].loadAction = MTLLoadActionClear;
    rpd.colorAttachments[0].storeAction = MTLStoreActionStore;
    rpd.colorAttachments[0].clearColor = MTLClearColorMake(0.1, 0.1, 0.1, 1.0);

    impl->CmdBuf = [impl->Queue commandBuffer];
    impl->Encoder = [impl->CmdBuf renderCommandEncoderWithDescriptor:rpd];
  }
  ctx.CmdBuf = (__bridge void*)impl->CmdBuf;
  ctx.Encoder = (__bridge void*)impl->Encoder;
}

void MetalLayer::EndFrame(GfxMetalCtx& ctx) {
  if (impl && impl->Encoder) {
    @autoreleasepool {
      [impl->Encoder endEncoding];
      [impl->CmdBuf presentDrawable:impl->Drawable];
      [impl->CmdBuf commit];
    }
  }
  if (impl) {
    impl->Encoder = nil;
    impl->CmdBuf = nil;
    impl->Drawable = nil;
  }
  ctx.CmdBuf = nullptr;
  ctx.Encoder = nullptr;
}
}  // namespace PD
#else
namespace PD {
struct MetalLayer::Impl {};
bool MetalLayer::Create(GLFWwindow* win, GfxMetalCtx& ctx) { return false; }
void MetalLayer::Destroy(GfxMetalCtx& ctx) {}
void MetalLayer::BeginFrame(GfxMetalCtx& ctx, const ivec2& size) {}
void MetalLayer::EndFrame(GfxMetalCtx& ctx) {}
}  // namespace PD
#endif
