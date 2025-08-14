/*
MIT License

Copyright (c) 2024 - 2025 tobid7

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
 */

#include <pd-3ds/bknd-gfx.hpp>

/// @brief Shader Code (Unused as i dont want to use libpicasso here (yet))
/// Update: Picasso breaks the linearRam or ram for somereason
/// as far as i found out loading anything into linear ram after
/// using libpicasso to compile a shader leads into a system freeze
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

// clang-format off
unsigned char li_shader[] = {
  0x44, 0x56, 0x4c, 0x42, 0x1, 0x0, 0x0, 0x0, 0xa4, 0x0, 0x0, 0x0, 0x44, 0x56, 0x4c, 0x50, 0x0, 0x0, 0x0, 0x0, 0x28, 0x0, 0x0, 0x0, 0xa, 0x0, 0x0, 0x0, 0x50, 0x0, 0x0, 0x0, 0x9, 0x0, 0x0, 0x0, 0x98, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x4e, 0x1, 0xf0, 0x7, 0x4e, 0x2, 0x8, 0x2, 0x8, 0x3, 0x18, 0x2, 0x8, 0x4, 0x28, 0x2, 0x8, 0x5, 0x38, 0x2, 0x8, 0x6, 0x10, 0x40, 0x4c, 0x7, 0xf1, 0x27, 0x22, 0x8, 0x10, 0x21, 0x4c, 0x0, 0x0, 0x0, 0x88, 0xac, 0x2, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xa1, 0xa, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x68, 0xc3, 0x6, 0x0, 0x0, 0x0, 0x0, 0x0, 0x64, 0xc3, 0x6, 0x0, 0x0, 0x0, 0x0, 0x0, 0x62, 0xc3, 0x6, 0x0, 0x0, 0x0, 0x0, 0x0, 0x61, 0xc3, 0x6, 0x0, 0x0, 0x0, 0x0, 0x0, 0xaf, 0x2, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x4f, 0xd5, 0x6, 0x0, 0x0, 0x0, 0x0, 0x0, 0x6f, 0x3, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x44, 0x56, 0x4c, 0x45, 0x2, 0x10, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xa, 0x0, 0x0, 0x0, 0x0, 0x0, 0x7, 0x0, 0x0, 0x0, 0x0, 0x0, 0x40, 0x0, 0x0, 0x0, 0x1, 0x0, 0x0, 0x0, 0x54, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x54, 0x0, 0x0, 0x0, 0x3, 0x0, 0x0, 0x0, 0x6c, 0x0, 0x0, 0x0, 0x1, 0x0, 0x0, 0x0, 0x74, 0x0, 0x0, 0x0, 0xb, 0x0, 0x0, 0x0, 0x2, 0x0, 0x5f, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x3f, 0x0, 0x1, 0x1, 0x37, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xf, 0x0, 0x0, 0x0, 0x2, 0x0, 0x1, 0x0, 0xf, 0x0, 0x0, 0x0, 0x3, 0x0, 0x2, 0x0, 0xf, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x10, 0x0, 0x13, 0x0, 0x70, 0x72, 0x6f, 0x6a, 0x65, 0x63, 0x74, 0x69, 0x6f, 0x6e, 0x0, 0x0, 
  };
// clang-format on
size_t li_shader_size = 0x124;

namespace PD {
GPU_TEXCOLOR GetTexFmt(Texture::Type type) {
  if (type == Texture::RGBA32)
    return GPU_RGBA8;
  else if (type == Texture::RGB24)
    return GPU_RGB8;
  else if (type == Texture::A8)
    return GPU_A8;
  return GPU_RGBA8;  // Default
}
int GetBPP(Texture::Type type) {
  if (type == Texture::RGBA32)
    return 4;
  else if (type == Texture::RGB24)
    return 3;
  else if (type == Texture::A8)
    return 1;
  return 0;  // Error
}

void GfxC3D::Init() {
  VertexBuffer.Resize(4 * 8192);
  IndexBuffer.Resize(6 * 8192);

  Flags |= LiBackendFlags_FlipUV_Y;

  ShaderCode = DVLB_ParseFile((uint32_t*)li_shader, li_shader_size);
  shaderProgramInit(&Shader);
  shaderProgramSetVsh(&Shader, &ShaderCode->DVLE[0]);
  pLocProjection =
      shaderInstanceGetUniformLocation(Shader.vertexShader, "projection");

  AttrInfo_Init(&ShaderInfo);
  AttrInfo_AddLoader(&ShaderInfo, 0, GPU_FLOAT, 2);
  AttrInfo_AddLoader(&ShaderInfo, 1, GPU_FLOAT, 2);
  AttrInfo_AddLoader(&ShaderInfo, 2, GPU_UNSIGNED_BYTE, 4);
}

void GfxC3D::Deinit() {
  shaderProgramFree(&Shader);
  DVLB_Free(ShaderCode);
}

void GfxC3D::NewFrame() {
  C3D_BindProgram(&Shader);
  C3D_SetAttrInfo(&ShaderInfo);
  CurrentIndex = 0;
  CurrentVertex = 0;
  FrameCounter++;
  VertexCounter = NumVtx;
  IndexCounter = NumIdx;
  NumVtx = 0;
  NumIdx = 0;
}

void GfxC3D::BindTex(PD::Li::TexAddress addr) {
  C3D_TexBind(0, reinterpret_cast<C3D_Tex*>(addr));
}

void GfxC3D::RenderDrawData(const std::vector<PD::Li::Command::Ref>& Commands) {
  // C3D_BindProgram(&Shader);
  shaderProgramUse(&Shader);
  C3D_SetAttrInfo(&ShaderInfo);
  C3D_Mtx proj;
  Mtx_OrthoTilt(&proj, 0.f, ViewPort.x, ViewPort.y, 0.f, 1.f, -1.f, false);
  C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, pLocProjection, &proj);
  // Mat4 proj = Mat4::Ortho(0.f, ViewPort.x, ViewPort.y, 0.f, 1.f, -1.f);
  // C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, pLocProjection, (C3D_Mtx*)&proj);
  C3D_DepthTest(false, GPU_GREATER, GPU_WRITE_ALL);
  C3D_TexEnv* env = C3D_GetTexEnv(0);
  C3D_TexEnvInit(env);
  C3D_TexEnvSrc(env, C3D_Both, GPU_TEXTURE0);
  C3D_TexEnvFunc(env, C3D_Both, GPU_MODULATE);
  size_t index = 0;
  while (index < Commands.size()) {
    PD::Li::Texture::Ref Tex = Commands[index]->Tex;
    if (!Tex) {
      index++;
      continue;
    }
    bool ScissorEnabled = Commands[index]->ScissorOn;
    ivec4 ScissorRect = Commands[index]->ScissorRect;
    size_t StartIndex = CurrentIndex;

    while (index < Commands.size() && Commands[index]->Tex == Tex &&
           Commands[index]->ScissorOn == ScissorEnabled &&
           Commands[index]->ScissorRect == ScissorRect) {
      auto c = Commands[index].get();
      for (size_t i = 0; i < c->IndexBuffer.Size(); i++) {
        NumIdx++;
        IndexBuffer[CurrentIndex++] = CurrentVertex + c->IndexBuffer.At(i);
      }
      for (size_t i = 0; i < c->VertexBuffer.Size(); i++) {
        NumVtx++;
        VertexBuffer[CurrentVertex++] = c->VertexBuffer.At(i);
      }
      index++;
    }
    if (ScissorEnabled) {
      // Figure out this setup was pain
      C3D_SetScissor(GPU_SCISSOR_NORMAL,
                     ViewPort.y - (ScissorRect.y + ScissorRect.w),
                     ViewPort.x - (ScissorRect.x + ScissorRect.z),
                     ViewPort.y - ScissorRect.y, ViewPort.x - ScissorRect.x);
    } else {
      C3D_SetScissor(GPU_SCISSOR_DISABLE, 0, 0, 0, 0);
    }
    BindTex(Tex->Address);
    auto bufInfo = C3D_GetBufInfo();
    BufInfo_Init(bufInfo);
    BufInfo_Add(bufInfo, VertexBuffer.Data(), sizeof(Vertex), 3, 0x210);

    C3D_DrawElements(GPU_TRIANGLES, CurrentIndex - StartIndex,
                     C3D_UNSIGNED_SHORT, IndexBuffer.Data() + StartIndex);
  }
  C3D_DepthTest(true, GPU_GREATER, GPU_WRITE_ALL);
}

PD::Li::Texture::Ref GfxC3D::LoadTex(const std::vector<PD::u8>& pixels, int w,
                                     int h, PD::Li::Texture::Type type,
                                     PD::Li::Texture::Filter filter) {
  if (w > 1024 || h > 1024) {
    return nullptr;
  }
  // Don't check here as check done before
  PD::Li::Texture::Ref res = PD::Li::Texture::New();
  int bpp = GetBPP(type);
  ivec2 tex_size(w, h);
  // Pow2
  if (!PD::BitUtil::IsSingleBit(w)) {
    tex_size.x = PD::BitUtil::GetPow2((unsigned int)w);
  }
  if (!PD::BitUtil::IsSingleBit(h)) {
    tex_size.y = PD::BitUtil::GetPow2((unsigned int)h);
  }
  res->Size.x = w;
  res->Size.y = h;
  res->UV = fvec4(0.f, 1.f, ((float)w / (float)tex_size.x),
                  1.0 - ((float)h / (float)tex_size.y));

  // Texture Setup
  auto fltr = (filter == Texture::NEAREST ? GPU_NEAREST : GPU_LINEAR);
  auto tex_fmt = GetTexFmt(type);
  auto tex = new C3D_Tex;
  C3D_TexInit(tex, (u16)tex_size.x, (u16)tex_size.y, tex_fmt);
  C3D_TexSetFilter(tex, fltr, fltr);

  // Using std::fill_n instead cause i hate this error lines
  // under the memset func in my editor
  std::fill_n((PD::u8*)tex->data, tex->size, 0);
  // memset(tex->data, 0, tex->size);s

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
  res->Address = (TexAddress)tex;
  std::cout << std::format("Tex {:#08x} Addr {:#08X}", (TexAddress)res.get(),
                           res->Address)
            << std::endl;
  return res;
}
}  // namespace PD