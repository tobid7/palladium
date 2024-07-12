#include <pd/LI7.hpp>
#include <pd/Color.hpp>
#include <pd/palladium.hpp>
#include <pd/external/stb_truetype.h>
#include <algorithm>

#include <li7_shbin.h>

#define DISPLAY_TRANSFER_FLAGS                                              \
  (GX_TRANSFER_FLIP_VERT(0) | GX_TRANSFER_OUT_TILED(0) |                    \
   GX_TRANSFER_RAW_COPY(0) | GX_TRANSFER_IN_FORMAT(GX_TRANSFER_FMT_RGBA8) | \
   GX_TRANSFER_OUT_FORMAT(GX_TRANSFER_FMT_RGB8) |                           \
   GX_TRANSFER_SCALING(GX_TRANSFER_SCALE_NO))

#ifndef __builtin_swap_32
#define __builtin_swap_32(x)                                                \
  ((((x) & 0xff) << 24) | (((x) & 0xff00) << 8) | (((x) & 0xff0000) >> 8) | \
   (((x) >> 24) & 0xff))
#endif
#define bs32(x) __builtin_swap_32(x)
#ifndef MAX
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#endif

namespace Palladium {
const float LI7::m_dffs = 8.f;
int LI7::m_uLoc_proj;
float LI7::m_scale = 1.0f;
int LI7::m_width, LI7::m_height;
int LI7::m_d_vertices = 0;
int LI7::m_d_drawcalls = 0;
int LI7::m_d_commands = 0;
const int LI7::m_char_height = 8;  // Constant
float LI7::m_rot = 0.f;
// UI Stuff
std::vector<LI7::Cmd> LI7::m_top_draw_cmds;
std::vector<LI7::Cmd> LI7::m_bot_draw_cmds;
Texture::Ref LI7::m_current_texture;
std::vector<LI7::Vtx, LinearAllocator<LI7::Vtx>> LI7::m_vtx_list[2];
// LI7::Font *LI7::m_font;
std::vector<char> LI7::m_text_buffer;
// Matrix
C3D_Mtx LI7::m_icon_model_matrix;
// Ctx Stuff
bool LI7::m_bottom_active = false;
// Shader
DVLB_s *LI7::li7_dvlb;
shaderProgram_s LI7::li7_shader;
C3D_AttrInfo LI7::li7_attr;

void LIFont::LoadTFF(const std::string path, int px_size) {
    // Supported Sizings (Tested [12, 16, 21, 24, 32, 48, 56, 63])
    this->pixel_height = px_size;
    if(!Palladium::FS::FileExist(path)) return;
    int type = px_size*16;
    // Load TTF
    stbtt_fontinfo inf;
    std::ifstream loader(path, std::ios::binary);
    if (!loader.is_open()) return;
    loader.seekg(0, std::ios::end);
    size_t len = loader.tellg();
    loader.seekg(0, std::ios::beg);
    unsigned char* buffer = new unsigned char[len];
    loader.read(reinterpret_cast<char*>(buffer), len);
    loader.close();
    stbtt_InitFont(&inf, buffer, 0);
    std::vector<unsigned char> fmap(type*type*4);
    float scale = stbtt_ScaleForPixelHeight(&inf, pixel_height);

    int ascent, descent, lineGap;
    stbtt_GetFontVMetrics(&inf, &ascent, &descent, &lineGap);
    int baseline = static_cast<int>(ascent * scale);

    for (int c = 0; c < 255; c++) {
      if (stbtt_IsGlyphEmpty(&inf, c)) continue;

      int width, height, xOffset, yOffset;
      unsigned char* bitmap = stbtt_GetCodepointBitmap(
          &inf, scale, scale, c, &width, &height, &xOffset, &yOffset);
      int x0, y0, x1, y1;
      stbtt_GetCodepointBitmapBox(&inf, c, scale, scale, &x0, &y0, &x1, &y1);
      this->fontw[c] = x0 + x1;
      int i = c % 16;
      int j = c / 16;
      int xoff = i * pixel_height;
      int yoff = j * pixel_height + baseline + yOffset;

      for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
          int map_pos = ((yoff + y) * type + (xoff + x)) * 4;

          fmap[map_pos + 0] = 255;
          fmap[map_pos + 1] = 255;
          fmap[map_pos + 2] = 255;
          fmap[map_pos + 3] = bitmap[x + y * width];
        }
      }

      free(bitmap);
    }
    auto ftex = Texture::New();
    ftex->LoadPixels(fmap, type, type);
    this->tex.push_back(ftex);
}

void LIFont::LoadBitmapFont(const std::string& path) {

}

void LIFont::LoadSystemFont() {
    
}

int LIFont::GetPixelHeight() {

}

LIFont::CPI LIFont::GetCodepoint() {

}

void LI7::Init() {
  m_text_buffer.resize(1024);
  m_vtx_list[0].reserve(2 * 4096);
  m_vtx_list[1].reserve(2 * 4096);

  li7_dvlb = DVLB_ParseFile((u32*)li7_shbin, li7_shbin_size);
  shaderProgramInit(&li7_shader);
  shaderProgramSetVsh(&li7_shader, &li7_dvlb->DVLE[0]);
  m_uLoc_proj =
      shaderInstanceGetUniformLocation(li7_shader.vertexShader, "projection");

  AttrInfo_Init(&li7_attr);
  AttrInfo_AddLoader(&li7_attr, 0, GPU_FLOAT, 3);
  AttrInfo_AddLoader(&li7_attr, 1, GPU_FLOAT, 2);
  AttrInfo_AddLoader(&li7_attr, 2, GPU_UNSIGNED_BYTE, 4);
}

void LI7::Exit() {
  shaderProgramFree(&li7_shader);
  DVLB_Free(li7_dvlb);
}

void LI7::OnScreen(bool bottom) {
    m_width = bottom ? 320 : 400;
    m_height = 240;
    m_bottom_active = bottom;
}

bool LI7::CompareCommands(const LI7::Cmd &a,
                             const LI7::Cmd &b) {
  if (a.layer == b.layer)
    return a.tex < b.tex;
  return b.layer < a.layer;
}

void LI7::RenderFrame(bool bottom) {
  m_rot += M_PI / 60.f;
  C3D_Mtx proj;
  Mtx_OrthoTilt(&proj, 0.f, (bottom ? 320 : 400), m_height, 0.f, 1.f, -1.f,
                false);
  C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, m_uLoc_proj, &proj);

  C3D_DepthTest(false, GPU_GREATER, GPU_WRITE_ALL);

  C3D_TexEnv *env = C3D_GetTexEnv(0);
  C3D_TexEnvInit(env);
  C3D_TexEnvSrc(env, C3D_Both, GPU_TEXTURE0, GPU_PRIMARY_COLOR, (GPU_TEVSRC)0);
  C3D_TexEnvFunc(env, C3D_Both, GPU_MODULATE);

  auto& active_list = m_vtx_list[bottom];

  int total_vertices = 0;
  m_d_drawcalls = 0;
  auto &draw_cmds = bottom ? m_bot_draw_cmds : m_top_draw_cmds;
  std::sort(draw_cmds.begin(), draw_cmds.end(), CompareCommands);
  m_d_commands = draw_cmds.size();
  size_t vtx = 0;
  while (draw_cmds.size() > 0) {
    C3D_Tex *texture = draw_cmds[draw_cmds.size() - 1].tex->Get();
    size_t start_vtx = vtx;

    while (draw_cmds.size() > 0 &&
           draw_cmds[draw_cmds.size() - 1].tex->Get() == texture) {
      auto c = draw_cmds[draw_cmds.size() - 1];
      if(c.cmd_type == 1) {
        active_list[vtx++] = Vtx{{c.ppos.x, c.ppos.y, 0}, {c.uv.x, c.uv.y}, c.clr};
        active_list[vtx++] = Vtx{{c.pszs.x, c.pszs.y, 0}, {c.uv.x, c.uv.w}, c.clr};
        active_list[vtx++] = Vtx{{c.apos.x, c.apos.y, 0}, {c.uv.z, c.uv.w}, c.clr};
      } else if (c.cmd_type == 2) {
        // TRI1
        active_list[vtx++] = Vtx{{c.ppos.x+c.pszs.x, c.ppos.y+c.pszs.y, 0}, {c.uv.z, c.uv.y}, c.clr};
        active_list[vtx++] = Vtx{{c.ppos.x+c.pszs.x, c.ppos.y, 0}, {c.uv.z, c.uv.w}, c.clr};
        active_list[vtx++] = Vtx{{c.ppos.x, c.ppos.y, 0}, {c.uv.x, c.uv.w}, c.clr};
        // TRI2
        active_list[vtx++] = Vtx{{c.ppos.x, c.ppos.y, 0}, {c.uv.x, c.uv.w}, c.clr};
        active_list[vtx++] = Vtx{{c.ppos.x, c.ppos.y+c.pszs.y, 0}, {c.uv.x, c.uv.y}, c.clr};
        active_list[vtx++] = Vtx{{c.ppos.x+c.pszs.x, c.ppos.y+c.pszs.y, 0}, {c.uv.z, c.uv.y}, c.clr};
      }
      draw_cmds.pop_back();
    }

    C3D_TexBind(0, texture);

    C3D_BufInfo *bufinfo = C3D_GetBufInfo();
    BufInfo_Init(bufinfo);
    BufInfo_Add(bufinfo, active_list.data() + start_vtx, sizeof(Vtx), 3,
                0x210);

    C3D_DrawArrays(GPU_TRIANGLES, 0, vtx - start_vtx);
    m_d_drawcalls++;
    total_vertices += vtx - start_vtx;
  }
  C3D_DepthTest(true, GPU_GREATER, GPU_WRITE_ALL);

  m_d_vertices = total_vertices;
  m_current_texture = nullptr;
  m_scale = 1.f;
}

void LI7::Render(C3D_RenderTarget* top, C3D_RenderTarget* bot) {
 C3D_BindProgram(&li7_shader);
 C3D_SetAttrInfo(&li7_attr);
 C3D_FrameDrawOn(top);
  RenderFrame(false);
  int d_tmp_cmds1 = m_d_commands;
  int d_tmp_dcls1 = m_d_drawcalls;
  int d_tmp_vtxs1 = m_d_vertices;
  C3D_FrameDrawOn(bot);
  RenderFrame(true);
  int d_tmp_cmds2 = m_d_commands;
  int d_tmp_dcls2 = m_d_drawcalls;
  int d_tmp_vtxs2 = m_d_vertices;
  m_d_commands = d_tmp_cmds1 + d_tmp_cmds2;
  m_d_drawcalls = d_tmp_dcls1 + d_tmp_dcls2;
  m_d_vertices = d_tmp_vtxs1 + d_tmp_vtxs2;
}

void LI7::ColorRect(NVec2 pos, NVec2 szs, NVec4 uvs, unsigned int clr) {
    Cmd c;
    c.ppos = pos;
    c.pszs = szs;
    c.uv = uvs;
    c.clr = bs32(clr);
    c.tex = m_current_texture;
    c.cmd_type = 2;
    if(m_bottom_active) m_bot_draw_cmds.push_back(c);
    else m_top_draw_cmds.push_back(c);
}

void LI7::ColorRect(NVec2 pos, NVec2 szs, unsigned int clr) {
    ColorRect(pos, szs, NVec4(0, 0, 1, 1), clr);
}

void LI7::Rect(NVec2 pos, NVec2 szs, NVec4 uvs) {
    ColorRect(pos, szs, uvs, 0xffffffff);
}

void LI7::Rect(NVec2 pos, NVec2 szs) {
    ColorRect(pos, szs, NVec4(0, 0, 1, 1), 0xffffffff);
}

void LI7::TexCutRect(NVec2 pos, NVec2 szs, NVec2 cb, NVec2 ce) {
  float u0 = (float)(cb.x / (float)m_current_texture->Get()->width);
  float v1 = (float)(((float)m_current_texture->Get()->height - cb.y) /
                     (float)m_current_texture->Get()->height);
  float u1 = (float)(ce.x / (float)m_current_texture->Get()->width);
  float v0 = (float)(((float)m_current_texture->Get()->height - ce.y) /
                     (float)m_current_texture->Get()->height);
  Rect(pos, szs, NVec4(u0, v0, u1, v1));
}

int LI7::DrawTextVargs(int x, int y, int z, unsigned int color, bool shadow,
                           int wrap, int* ySize, const char* fmt, va_list arg) {
  // FONT
  int len = vsnprintf(m_text_buffer.data(), m_text_buffer.size(), fmt, arg);
  m_text_buffer[len] = '\0';
  int offsetX = 0;
  int offsetY = 0;
  int maxWidth = 0;
  float cpm = m_dffs /1;// m_font->char_size;

  for (const auto &it : m_text_buffer) {
    if (it == '\0') break;
    bool implicitBreak = offsetX + 0/*m_font->fontWidth[(int)it]*/ >= wrap;
    if (it == '\n' || implicitBreak) {
      offsetY += /*m_font->char_size*/0 * cpm * m_scale;
      maxWidth = std::max(maxWidth, offsetX);
      offsetX = 0;
      if (implicitBreak) continue;
    } else if (it == '\t') {
      offsetX = ((offsetX / m_dffs) / 4 + 1) * 4 * m_dffs;
    } else {
      if (it != ' ') {
        int texX = (it % 16) * /*m_font->char_size*/0;
        int texY = (/*m_font->texture.tex.height*/0 - /*m_font->char_size*/0) -
                   (it / 16) * /*m_font->char_size*/0;
        ColorRect(NVec2(x + offsetX, y + offsetY), NVec2(m_dffs, m_dffs),
                      NVec4(static_cast<float>(texX) / /*m_font->texture.tex.width*/0,
                      static_cast<float>(texY) / /*m_font->texture.tex.height*/0,
                      (static_cast<float>(texX) + /*m_font->char_size*/0) /
                          /*m_font->texture.tex.width*/0,
                      (static_cast<float>(texY) + /*m_font->char_size*/0) /
                          /*m_font->texture.tex.height*/0),
                      color);

        if (shadow)
          ColorRect(NVec2(x + offsetX + 1, y + offsetY + 1), NVec2(m_dffs, m_dffs),
                        NVec4(static_cast<float>(texX) / /*m_font->texture.tex.width*/0,
                        static_cast<float>(texY) / /*m_font->texture.tex.height*/0,
                        (static_cast<float>(texX) + /*m_font->char_size*/0) /
                            /*m_font->texture.tex.width*/0,
                        (static_cast<float>(texY) + /*m_font->char_size*/0) /
                            /*m_font->texture.tex.height*/0),
                        RGBA8(10, 10, 10, 255));
      }
      offsetX += /*m_font->fontWidth[(int)it]*/0 * cpm * m_scale +
                 ((/*m_font->char_size*/0 * 0.2) * cpm * m_scale);
    }
  }
  maxWidth = std::max(maxWidth, offsetX);

  if (ySize != nullptr) *ySize = offsetY + /*m_font->char_size*/0;
  return maxWidth;
                           }
}