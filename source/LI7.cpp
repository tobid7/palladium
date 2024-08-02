#include <pd/external/stb_truetype.h>

#include <algorithm>
#include <filesystem>
#include <pd/Color.hpp>
#include <pd/LI7.hpp>
#include <pd/li7_shader.hpp>
#include <pd/palladium.hpp>

#ifndef MAX
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#endif

namespace Palladium {
const float LI7::m_dffs = 24.f;
const float LI7::m_dts = 0.7f;
float LI7::m_txt_scale = 0.7f;
int LI7::m_uLoc_proj;
float LI7::m_scale = 1.0f;
int LI7::m_width, LI7::m_height;
int LI7::m_d_vertices = 0;
int LI7::m_d_drawcalls = 0;
int LI7::m_d_commands = 0;
const int LI7::m_char_height = 8;  // Constant
// UI Stuff
std::vector<LI7::Cmd> LI7::m_top_draw_cmds;
std::vector<LI7::Cmd> LI7::m_bot_draw_cmds;
Texture::Ref LI7::m_current_texture;
Texture::Ref LI7::m_white;
std::vector<LI7::Vtx, LinearAllocator<LI7::Vtx>> LI7::m_vtx_list[2];
// LI7::Font *LI7::m_font;
LIFont::Ref LI7::m_font;
std::vector<char> LI7::m_text_buffer;
// Ctx Stuff
bool LI7::m_bottom_active = false;
// Shader
DVLB_s* LI7::li7_dvlb;
shaderProgram_s LI7::li7_prog;
C3D_AttrInfo LI7::li7_attr;

void LIFont::LoadTFF(const std::string path, int px_size) {
  Palladium::Ftrace::ScopedTrace st(
      "LIFont", std::filesystem::path(path).filename().string());
  this->pixel_height = px_size;
  if (!Palladium::FS::FileExist(path)) return;
  int type = px_size * 16;
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
  std::vector<unsigned char> fmap(type * type * 4);
  float scale = stbtt_ScaleForPixelHeight(&inf, pixel_height);

  int ascent, descent, lineGap;
  stbtt_GetFontVMetrics(&inf, &ascent, &descent, &lineGap);
  int baseline = static_cast<int>(ascent * scale);

  name = path;
  auto ftex = Texture::New();
  NVec2 offset;
  for (int c = 0; c < 255; c++) {
    CPI codepoint;
    if (stbtt_IsGlyphEmpty(&inf, c)) {
      codepoint.codepoint = c;
      codepoint.tex = ftex;
      cpmap.push_back(codepoint);
      continue;
    }

    int width, height, xOffset, yOffset;
    unsigned char* bitmap = stbtt_GetCodepointBitmap(
        &inf, scale, scale, c, &width, &height, &xOffset, &yOffset);
    int x0, y0, x1, y1;
    stbtt_GetCodepointBitmapBox(&inf, c, scale, scale, &x0, &y0, &x1, &y1);

    if (offset.x + width > type) {
      offset.y += pixel_height;
      offset.x = 0;
    }

    codepoint.uv.x = static_cast<float>((float)offset.x / (float)type);
    codepoint.uv.y = 1.0f - static_cast<float>((float)offset.y / (float)type);
    codepoint.uv.z =
        static_cast<float>(float(offset.x + width) / (float)type);
    codepoint.uv.w =
        1.0f - static_cast<float>(float(offset.y + height) / (float)type);

    codepoint.tex = ftex;
    codepoint.szs.x = width;
    codepoint.szs.y = height;
    codepoint.off = baseline + yOffset;

    for (int y = 0; y < height; ++y) {
      for (int x = 0; x < width; ++x) {
        int map_pos = ((offset.y + y) * type + (offset.x + x)) * 4;

        fmap[map_pos + 0] = 255;
        fmap[map_pos + 1] = 255;
        fmap[map_pos + 2] = 255;
        fmap[map_pos + 3] = bitmap[x + y * width];
      }
    }
    offset.x += width;
    if (offset.x + width > type) {
      offset.y += pixel_height;
      offset.x = 0;
    }
    free(bitmap);
    cpmap.push_back(codepoint);
  }
  ftex->LoadPixels(fmap, type, type);
  this->tex.push_back(ftex);
}

void LIFont::LoadBitmapFont(const std::string& path) {}

void LIFont::LoadSystemFont() {
  name = "System Font";
  float text_scale = 0.f;
  // Code to Load the System Font
  const auto fnt = fontGetSystemFont();
  const auto fnt_info = fontGetInfo(fnt);
  const auto glyph_info = fontGetGlyphInfo(fnt);
  this->tex.resize(glyph_info->nSheets + 1);
  text_scale = 30.f / glyph_info->cellHeight;
  for (size_t i = 0; i < glyph_info->nSheets; i++) {
    auto tex = this->tex[i];
    tex = Texture::New();
    auto tx = new C3D_Tex;
    tx->data = fontGetGlyphSheetTex(fnt, i);
    tx->fmt = (GPU_TEXCOLOR)glyph_info->sheetFmt;
    tx->size = glyph_info->sheetSize;
    tx->width = glyph_info->sheetWidth;
    tx->height = glyph_info->sheetHeight;
    tx->param = GPU_TEXTURE_MAG_FILTER(GPU_LINEAR) |
                GPU_TEXTURE_MIN_FILTER(GPU_LINEAR) |
                GPU_TEXTURE_WRAP_S(GPU_REPEAT) | GPU_TEXTURE_WRAP_T(GPU_REPEAT);
    tx->border = 0xffffffff;
    tx->lodParam = 0;
    tex->ExternalLoad(tx, NVec2(tx->width, tx->height), NVec4(0, 1, 1, 0));
  }
}

int LIFont::GetPixelHeight() { return this->pixel_height; }

LIFont::CPI LIFont::GetCodepoint(char c) { return cpmap[c]; }

void LIFont::Dump() {
  std::ofstream ofs("sdmc:/font.txt", std::ios::out);
  ofs << "LI7 Font Dump" << std::endl;
  ofs << "Pixel Height: " << (int)pixel_height << std::endl;
  for (auto& it : this->cpmap) {
    ofs << "  Codepoint: " << (int)it.codepoint << std::endl;
    ofs << "  Width: " << (int)it.szs.x << std::endl;
    ofs << "  UV: (" << (float)it.uv.x << ", " << (float)it.uv.y << ", "
        << (float)it.uv.z << ", " << (float)it.uv.w << ")" << std::endl;
  }
  ofs.close();
}

void LI7::Init() {
  m_text_buffer.resize(1024);
  m_vtx_list[0].reserve(2 * 4096);
  m_vtx_list[1].reserve(2 * 4096);

  li7_dvlb = DVLB_ParseFile((u32*)li7_shader, li7_shader_size);
  shaderProgramInit(&li7_prog);
  shaderProgramSetVsh(&li7_prog, &li7_dvlb->DVLE[0]);
  m_uLoc_proj =
      shaderInstanceGetUniformLocation(li7_prog.vertexShader, "projection");

  AttrInfo_Init(&li7_attr);
  AttrInfo_AddLoader(&li7_attr, 0, GPU_FLOAT, 3);
  AttrInfo_AddLoader(&li7_attr, 1, GPU_FLOAT, 2);
  AttrInfo_AddLoader(&li7_attr, 2, GPU_UNSIGNED_BYTE, 4);

  m_white = Texture::New();
  std::vector<unsigned char> pixels(16 * 16 * 4, 255);
  m_white->LoadPixels(pixels, 16, 16);
  // C3D_Tex* w = new C3D_Tex;
  // C3D_TexInit(w, 16, 16, GPU_L8);
  // C3D_TexLoadImage(w, pixels.data(), GPU_TEXFACE_2D, 0);
  // m_white->ExternalLoad(w, NVec2(16, 16), NVec4(0, 1, 1, 0));

  m_font = LIFont::New();
  // m_font->LoadSystemFont();
  m_font->LoadTFF("romfs:/fonts/ComicNeue.ttf", 32);
}

void LI7::Exit() {
  shaderProgramFree(&li7_prog);
  DVLB_Free(li7_dvlb);
}

void LI7::OnScreen(bool bottom) {
  m_width = bottom ? 320 : 400;
  m_height = 240;
  m_bottom_active = bottom;
}

bool LI7::CompareCommands(const LI7::Cmd& a, const LI7::Cmd& b) {
  if (a.layer == b.layer) return a.tex < b.tex;
  return b.layer < a.layer;
}

void LI7::RenderFrame(bool bottom) {
  C3D_Mtx proj;
  Mtx_OrthoTilt(&proj, 0.f, (bottom ? 320 : 400), m_height, 0.f, 1.f, -1.f,
                false);
  C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, m_uLoc_proj, &proj);

  C3D_DepthTest(false, GPU_GREATER, GPU_WRITE_ALL);

  C3D_TexEnv* env = C3D_GetTexEnv(0);
  C3D_TexEnvInit(env);
  C3D_TexEnvSrc(env, C3D_Both, GPU_TEXTURE0, GPU_PRIMARY_COLOR, (GPU_TEVSRC)0);
  C3D_TexEnvFunc(env, C3D_Both, GPU_MODULATE);

  auto& active_list = m_vtx_list[bottom];

  int total_vertices = 0;
  m_d_drawcalls = 0;
  auto& draw_cmds = bottom ? m_bot_draw_cmds : m_top_draw_cmds;
  std::reverse(draw_cmds.begin(), draw_cmds.end());
  // std::sort(draw_cmds.begin(), draw_cmds.end(), CompareCommands);
  m_d_commands = draw_cmds.size();
  size_t vtx = 0;
  while (draw_cmds.size() > 0) {
    C3D_Tex* texture = draw_cmds[draw_cmds.size() - 1].tex->Get();
    size_t start_vtx = vtx;

    while (draw_cmds.size() > 0 &&
           draw_cmds[draw_cmds.size() - 1].tex->Get() == texture) {
      auto c = draw_cmds[draw_cmds.size() - 1];
      if (c.cmd_type == 1) {
        active_list[vtx++] =
            Vtx(NVec2(c.bot.z, c.bot.w), c.uv.z, c.uv.w, c.clr);
        active_list[vtx++] =
            Vtx(NVec2(c.top.z, c.top.w), c.uv.z, c.uv.y, c.clr);
        active_list[vtx++] =
            Vtx(NVec2(c.top.x, c.top.y), c.uv.x, c.uv.y, c.clr);
        ///
        active_list[vtx++] =
            Vtx(NVec2(c.top.x, c.top.y), c.uv.x, c.uv.y, c.clr);
        active_list[vtx++] =
            Vtx(NVec2(c.bot.x, c.bot.y), c.uv.x, c.uv.w, c.clr);
        active_list[vtx++] =
            Vtx(NVec2(c.bot.z, c.bot.w), c.uv.z, c.uv.w, c.clr);
      } else if (c.cmd_type == 2) {
        active_list[vtx++] =
            Vtx(NVec2(c.top.x, c.top.y), c.uv.x, c.uv.y, c.clr);
        active_list[vtx++] =
            Vtx(NVec2(c.top.z, c.top.w), c.uv.z, c.uv.w, c.clr);
        active_list[vtx++] =
            Vtx(NVec2(c.bot.x, c.bot.y), c.uv.x, c.uv.w, c.clr);
      }
      draw_cmds.pop_back();
    }

    C3D_TexBind(0, texture);

    C3D_BufInfo* bufinfo = C3D_GetBufInfo();
    BufInfo_Init(bufinfo);
    BufInfo_Add(bufinfo, active_list.data() + start_vtx, sizeof(Vtx), 3, 0x210);

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
  Palladium::Ftrace::ScopedTrace st("Li7", "Render");
  C3D_BindProgram(&li7_prog);
  C3D_SetAttrInfo(&li7_attr);
  C3D_FrameDrawOn(top);
  RenderFrame(false);
  int d_tmp_cmds1 = m_d_commands;
  int d_tmp_dcls1 = m_d_drawcalls;
  int d_tmp_vtxs1 = m_d_vertices;
  C3D_FrameDrawOn(bot);
  RenderFrame(true);
  m_d_commands += d_tmp_cmds1;
  m_d_drawcalls += d_tmp_dcls1;
  m_d_vertices += d_tmp_vtxs1;
}

void LI7::ColorRect(NVec2 pos, NVec2 szs, NVec4 uvs, unsigned int clr) {
  Cmd c;
  if (pos.x > m_width || pos.x + szs.x < 0 || pos.y > m_height ||
      pos.y + szs.y < 0)
    return;
  c.top = NVec4(pos, NVec2(pos.x + szs.x, pos.y));
  c.bot = NVec4(NVec2(pos.x, pos.y + szs.y), pos + szs);
  c.uv = uvs;
  c.clr = clr;
  c.tex = m_current_texture;
  c.cmd_type = 1;
  if (m_bottom_active)
    m_bot_draw_cmds.push_back(c);
  else
    m_top_draw_cmds.push_back(c);
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

void LI7::Triangle(NVec2 a, NVec2 b, NVec2 c, unsigned int clr) {
  Cmd cmd;
  if ((a.x > m_width && b.x > m_width && c.x > m_width) ||
      (a.y > m_height && b.y > m_height && c.y > m_height) ||
      (a.x < 0 && b.x < 0 && c.x < 0) || (a.y < 0 && b.y < 0 && c.y < 0))
    return;
  cmd.ppos = a;
  cmd.pszs = b;
  cmd.apos = c;
  cmd.top = NVec4(a, b);
  cmd.bot = NVec4(c, NVec2());
  cmd.uv = NVec4(0, 1, 1, 0);
  cmd.clr = clr;
  cmd.tex = m_current_texture;
  cmd.cmd_type = 1;
  if (m_bottom_active)
    m_bot_draw_cmds.push_back(cmd);
  else
    m_top_draw_cmds.push_back(cmd);
}

void LI7::Line(NVec2 a, NVec2 b, unsigned int clr, int t) {
  // Calculate direction vector
  NVec2 direction = {b.x - a.x, b.y - a.y};
  float length =
      std::sqrt(direction.x * direction.x + direction.y * direction.y);

  // Normalize direction vector
  NVec2 unit_direction = {direction.x / length, direction.y / length};

  // Calculate perpendicular vector
  NVec2 perpendicular = {-unit_direction.y, unit_direction.x};

  // Scale perpendicular vector by half the thickness
  float half_t = t / 2.0f;
  NVec2 offset = {perpendicular.x * half_t, perpendicular.y * half_t};

  // Calculate corner positions
  float px0 = a.x + offset.x;
  float py0 = a.y + offset.y;
  float px1 = b.x + offset.x;
  float py1 = b.y + offset.y;
  float px2 = a.x - offset.x;
  float py2 = a.y - offset.y;
  float px3 = b.x - offset.x;
  float py3 = b.y - offset.y;

  Cmd c;
  c.top = NVec4(px0, py0, px1, py1);
  c.bot = NVec4(px2, py2, px3, py3);
  c.uv = NVec4(0, 1, 1, 0);
  c.clr = clr;
  c.tex = m_current_texture;
  c.cmd_type = 1;
  if (m_bottom_active)
    m_bot_draw_cmds.push_back(c);
  else
    m_top_draw_cmds.push_back(c);
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

NVec2 LI7::GetTextDimensions(const std::string& text) {
  // FONT
  std::string txt = text + "\0";
  NVec2 pos(0, 0);  // Temp Pos
  NVec2 offset;
  float maxWidth = 0.f;
  float ntxtszs = m_dffs * m_txt_scale;
  float cpm = ntxtszs / m_font->GetPixelHeight();
  float line_height = m_font->GetPixelHeight() * cpm * m_scale;

  for (size_t i = 0; i < txt.length(); i++) {
    if (txt[i] == '\0') break;
    auto cp = m_font->GetCodepoint(txt[i]);
    bool implicitBreak = false;
    if (txt[i] == '\n' || implicitBreak) {
      offset.y += line_height;
      maxWidth = std::max(maxWidth, offset.x);
      offset.x = 0;
      if (implicitBreak) continue;
    } else if (txt[i] == '\t') {
      offset.x = ((offset.x / ntxtszs) / 4 + 1) * 4 * ntxtszs;
    } else {
      if (txt[i] == ' ') {
        // this will make the space twice
        offset.x += 2 * m_txt_scale;
      }
      if (i == txt.length() - 1)
        offset.x += cp.szs.x * cpm + m_txt_scale;
      else
        offset.x += cp.szs.x * cpm + (2 * m_txt_scale);
    }
  }
  maxWidth = std::max(maxWidth, offset.x);
  return NVec2(maxWidth, offset.y + (m_dffs * m_txt_scale));
}

void LI7::DrawText(NVec2 pos, unsigned int color, const std::string& text,
                   PDTextFlags flags, NVec2 ap) {
  std::string txt = text;
  NVec2 offset;
  float ntxtszs = m_dffs * m_txt_scale;
  float cpm = ntxtszs / m_font->GetPixelHeight();
  float line_height = m_font->GetPixelHeight() * cpm * m_scale;
  NVec2 td = GetTextDimensions(text);
  if (flags & PDTextFlags_AlignRight) pos.x -= td.x;
  if (flags & PDTextFlags_AlignMid) {
    pos.x = (ap.x * 0.5) - (td.x * 0.5) + pos.x;
  }
  std::vector<std::string> lines;
  std::istringstream iss(txt);
  std::string temp;
  while (std::getline(iss, temp)) {
    lines.push_back(temp);
  }

  for (auto& it : lines) {
    if (pos.y + offset.y + line_height < 0) {
      offset.y += line_height;
      continue;
    } else if (pos.y + offset.y > m_height) {
      // Break func as we dont want loop over lines that get skipped too
      break;
    }
    // Loop over line
    for (auto& jt : it) {
      auto cp = m_font->GetCodepoint(jt);
      m_current_texture = cp.tex;
      if (jt == '\t') {
        offset.x = ((offset.x / ntxtszs) / 4 + 1) * 4 * ntxtszs;
      } else {
        if (jt != ' ') {
          if (flags & PDTextFlags_Shaddow) {
            ColorRect(pos + NVec2(offset.x + 1, (offset.y+(cp.off*cpm)) + 1),
                      NVec2(cp.szs.x*cpm, cp.szs.y*cpm), cp.uv,
                      Palladium::Color::RGBA(color).is_light() ? 0xff111111
                                                               : 0xffeeeeee);
          }
          ColorRect(pos + offset + NVec2(0, (cp.off*cpm)), NVec2(cp.szs.x*cpm, cp.szs.y*cpm), cp.uv, color);

        } else {
          // this will make the space twice
          offset.x += 2 * m_txt_scale;
        }
        offset.x += cp.szs.x * cpm + (2 * m_txt_scale);
      }
    }
    offset.y += line_height;
    offset.x = 0;
  }
}
}  // namespace Palladium