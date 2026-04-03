// Well yes, i finally try it

#include <pd/core/core.hpp>
#include <pd/lithium/formatters.hpp>
#include <pd_system/gfx_citro3d.hpp>

#if defined(PD_ENABLE_CITRO3D) && defined(__3DS__)

#include <3ds.h>
#include <citro3d.h>

#include <pd/drivers/drivers.hpp>
#include <pica.hpp>

namespace PD {
const char* LIShaderCTR = R"(
; LI7 Shader
; Constants
.constf myconst(0.0, 1.0, 0.00392156862745, 0.0)
.alias  ones  	myconst.yyyy ; Vector full of ones

; Uniforms
.fvec projection[4]

; Outputs
.out out_position		position
.out out_color			color
.out out_uv				texcoord0

; Inputs
.alias in_xy		v0
.alias in_uvc 		v1
.alias in_col	    v2

.entry vmain
.proc vmain
	mov r0.xy, in_xy.xy
	mov r0.w, ones

	dp4 out_position.x, projection[0], r0
	dp4 out_position.y, projection[1], r0
	dp4 out_position.z, projection[2], r0
	dp4 out_position.w, projection[3], r0

	mov out_uv, in_uvc.xy

	mul r1, myconst.zzzz, in_col
	mov out_color, r1
	end
.end
)";

struct GfxCitro3D::Impl {
  C3D_AttrInfo pAttr;
  shaderProgram_s pShader;
  DVLB_s* pCode;
  int uLocProjection = 0;
  C3D_Tex* CurrentTex = nullptr;
  std::vector<u8> pShaderRaw;

  GPU_TEXCOLOR TextureTranslateFormat(TextureFormat fmt) {
    switch (fmt) {
      case PD::TextureFormat::A8:
        return GPU_A8;
      case PD::TextureFormat::RGB24:
        return GPU_RGB8;
      case PD::TextureFormat::RGBA32:
        return GPU_RGBA8;
      default:
        return GPU_RGBA8;
    }
  }

  int TextureFormat2Bpp(TextureFormat fmt) {
    switch (fmt) {
      case PD::TextureFormat::A8:
        return 1;
      case PD::TextureFormat::RGB24:
        return 3;
      case PD::TextureFormat::RGBA32:
        return 4;
      default:
        return 0;
    }
  }

  void SetupPixelStage() {
    shaderProgramUse(&pShader);
    C3D_BindProgram(&pShader);
    C3D_SetAttrInfo(&pAttr);
    C3D_DepthTest(false, GPU_GREATER, GPU_WRITE_ALL);
    if (!CurrentTex) return;
    C3D_TexEnv* env = C3D_GetTexEnv(0);
    C3D_TexEnvInit(env);
    switch (CurrentTex->fmt) {
      case GPU_A4:
      case GPU_A8:
      case GPU_L4:
      case GPU_L8:
        C3D_TexEnvSrc(env, C3D_Alpha, GPU_TEXTURE0);
        C3D_TexEnvFunc(env, C3D_RGB, GPU_REPLACE);
        C3D_TexEnvFunc(env, C3D_Alpha, GPU_MODULATE);
        break;
      case GPU_RGB565:
        C3D_TexEnvSrc(env, C3D_Alpha, GPU_TEXTURE0);
        C3D_TexEnvFunc(env, C3D_RGB, GPU_MODULATE);
        C3D_TexEnvFunc(env, C3D_Alpha, GPU_REPLACE);
        break;

      default:
        C3D_TexEnvSrc(env, C3D_Both, GPU_TEXTURE0);
        C3D_TexEnvFunc(env, C3D_Both, GPU_MODULATE);
        break;
    }
  }
};

void GfxCitro3D::SysInit() {
  if (impl) return;
  PDLOG("GfxCitro3D::SysInit();");
  impl = new Impl();
  Flags |= PDGfxBackendFlags_FlipUV_Y;
  impl->pShaderRaw = Pica::AssembleCode(LIShaderCTR);
  impl->pCode = DVLB_ParseFile((uint32_t*)impl->pShaderRaw.data(),
                               impl->pShaderRaw.size());
  shaderProgramInit(&impl->pShader);
  shaderProgramSetVsh(&impl->pShader, &impl->pCode->DVLE[0]);
  impl->uLocProjection = shaderInstanceGetUniformLocation(
      impl->pShader.vertexShader, "projection");

  AttrInfo_Init(&impl->pAttr);
  AttrInfo_AddLoader(&impl->pAttr, 0, GPU_FLOAT, 2);
  AttrInfo_AddLoader(&impl->pAttr, 1, GPU_FLOAT, 2);
  AttrInfo_AddLoader(&impl->pAttr, 2, GPU_UNSIGNED_BYTE, 4);
}

void GfxCitro3D::SysDeinit() {
  if (!impl) return;
  shaderProgramFree(&impl->pShader);
  DVLB_Free(impl->pCode);
  delete impl;
  impl = nullptr;
  PDLOG("GfxCitro3D::SysDeinit()");
}

void GfxCitro3D::Submit(size_t count, size_t start) {
  if (!impl) return;
  BindTexture(CurrentTex);
  impl->SetupPixelStage();  // needs to be called after
  C3D_Mtx proj;
  Mtx_OrthoTilt(&proj, 0.f, ViewPort.x, ViewPort.y, 0.f, 1.f, -1.f, false);
  C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, impl->uLocProjection, &proj);
  // C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, impl->uLocProjection,
  //                  (C3D_Mtx*)&Projection);
  auto buf = C3D_GetBufInfo();
  BufInfo_Init(buf);
  BufInfo_Add(buf, GetVertexBufPtr(0), sizeof(Li::Vertex), 3, 0x210);
  C3D_DrawElements(GPU_TRIANGLES, count, C3D_UNSIGNED_SHORT,
                   GetIndexBufPtr(start));
  impl->CurrentTex = nullptr;
}

void GfxCitro3D::BindTexture(TextureID id) {
  if (!impl || !id) return;
  impl->CurrentTex = (C3D_Tex*)id;
  C3D_TexBind(0, (C3D_Tex*)id);
}

void GfxCitro3D::SysReset() {
  if (!impl) return;
  C3D_CullFace(GPU_CULL_NONE);
}

Li::Texture GfxCitro3D::LoadTexture(const std::vector<PD::u8>& pixels, int w,
                                    int h, TextureFormat type,
                                    TextureFilter filter) {
  if (!impl || w > 1024 || h > 1024) return Li::Texture();
  if (pixels.size() !=
      static_cast<size_t>(w * h * Li::TextureFormat2Bpp(type))) {
    PDERR("Failed to load Texture due to Size mismatch: {} != {}",
          pixels.size(),
          static_cast<size_t>(w * h * Li::TextureFormat2Bpp(type)));
    return Li::Texture();
  }
  // Don't check here as check done before
  PD::Li::Texture res;
  int bpp = impl->TextureFormat2Bpp(type);
  ivec2 tex_size(w, h);
  // Pow2
  if (!PD::Bits::IsSingleBit(w)) {
    tex_size.x = PD::Bits::GetPow2((unsigned int)w);
  }
  if (!PD::Bits::IsSingleBit(h)) {
    tex_size.y = PD::Bits::GetPow2((unsigned int)h);
  }
  res.SetSize(w, h);
  res.SetUV(0.f, 1.f, ((float)w / (float)tex_size.x),
            1.0 - ((float)h / (float)tex_size.y));

  // Texture Setup
  auto fltr = (filter == TextureFilter::Linear ? GPU_NEAREST : GPU_LINEAR);
  auto tex_fmt = impl->TextureTranslateFormat(type);
  auto tex = new C3D_Tex;
  C3D_TexInit(tex, (u16)tex_size.x, (u16)tex_size.y, tex_fmt);
  C3D_TexSetFilter(tex, fltr, fltr);

  std::memset((PD::u8*)tex->data, 0x0, tex->size);

  /// Probably Remove this if statement in future
  /// This are the things confirmed as working
  if (bpp == 3 || bpp == 4 || bpp == 1) {
    for (int x = 0; x < w; x++) {
      for (int y = 0; y < h; y++) {
        int dst_pos = ((((y >> 3) * ((int)tex_size.x >> 3) + (x >> 3)) << 6) +
                       ((x & 1) | ((y & 1) << 1) | ((x & 2) << 1) |
                        ((y & 2) << 2) | ((x & 4) << 2) | ((y & 4) << 3))) *
                      bpp;
        int src_pos = (y * w + x) * bpp;
        /// Best idea i had
        for (int i = 0; i < bpp; i++) {
          ((u8*)tex->data)[dst_pos + bpp - 1 - i] = pixels[src_pos + i];
        }
      }
    }
    C3D_TexFlush(tex);
  }

  tex->border = 0x00000000;
  C3D_TexSetWrap(tex, GPU_REPEAT, GPU_REPEAT);
  res.SetID((TextureID)tex);
  RegisterTexture(res);
  PDLOG("GfxCitro3D::LoadTexture -> {{ {} }}, [{}, {}]", res, type, filter);
  return res;
}

void GfxCitro3D::DeleteTexture(const Li::Texture& tex) {
  if (!tex.GetID()) return;
  UnregisterTexture(tex);
  C3D_Tex* t = reinterpret_cast<C3D_Tex*>(tex.GetID());
  C3D_TexDelete(t);
  delete t;
}
}  // namespace PD
#else
namespace PD {
void GfxCitro3D::SysInit() {
  PDERR(
      "GfxCitro3D::SysInit: Citro3D Driver is not included in "
      "palladium-system");
}
void GfxCitro3D::SysDeinit() {}
void GfxCitro3D::Submit(size_t count, size_t start) {}
void GfxCitro3D::BindTexture(TextureID id) {}
void GfxCitro3D::SysReset() {}
Li::Texture GfxCitro3D::LoadTexture(const std::vector<PD::u8>& pixels, int w,
                                    int h, TextureFormat type,
                                    TextureFilter filter) {
  return Li::Texture();
}
void GfxCitro3D::DeleteTexture(const Li::Texture& tex) {}
}  // namespace PD
#endif