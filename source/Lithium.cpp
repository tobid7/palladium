#include <pd/external/stb_truetype.h>

#include <algorithm>
#include <cmath>
#include <codecvt>
#include <filesystem>
#include <fstream>
#include <pd/Lithium.hpp>
#include <pd/base/Color.hpp>
#include <pd/li7_shader.hpp>
#include <pd/palladium.hpp>

#ifndef MAX
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#endif

std::wstring make_wstring(const std::string& str) {
  std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
  return converter.from_bytes(str);
}

namespace Palladium {
PDLithiumFlags LI::flags = PDLithiumFlags_Default;
const float LI::default_font_size = 24.f;
const float LI::default_text_size = 0.7f;
float LI::text_scale = 0.7f;
// Renderer Stuff
bool LI::bottom_screen = false;
NVec2 LI::screen_size;
int LI::layer = 0;
std::vector<LI::Cmd> LI::draw_lists[2];
Texture::Ref LI::active_texture;
Texture::Ref LI::single_color;
std::vector<LI::Vtx, LinearAllocator<LI::Vtx>> LI::vertex_buffer;
std::vector<unsigned short, LinearAllocator<unsigned short>> LI::idx_buffer;
size_t LI::vertex_index = 0;
size_t LI::idx_index = 0;
bool LI::font_update = false;
LIFont::Ref LI::font;
bool LI::sysfont_render = false;
std::map<std::string, LI::TextBox> LI::text_sizes;
int LI::cmd_index = 0;
// Debug
int LI::num_vertices = 0;
int LI::num_drawcalls = 0;
int LI::num_commands = 0;
int LI::num_indices = 0;
// Shader
DVLB_s* LI::li7_dvlb;
shaderProgram_s LI::li7_prog;
C3D_AttrInfo LI::li7_attr;
int LI::uLoc_proj;

void LIFont::LoadTFF(const std::string path, int px_size) {
  sysfnt = false;
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
      codepoint.invalid = true;
      cpmap[c] = codepoint;
      continue;
    }

    int width, height, xOffset, yOffset;
    unsigned char* bitmap = stbtt_GetCodepointBitmap(
        &inf, scale, scale, c, &width, &height, &xOffset, &yOffset);
    int x0, y0, x1, y1;
    stbtt_GetCodepointBitmapBox(&inf, c, scale, scale, &x0, &y0, &x1, &y1);

    if (offset[0] + width > type) {
      offset[1] += pixel_height;
      offset[0] = 0;
    }

    codepoint.uv[0] = static_cast<float>((float)offset[0] / (float)type);
    codepoint.uv[1] = 1.0f - static_cast<float>((float)offset[1] / (float)type);
    codepoint.uv[2] =
        static_cast<float>(float(offset[0] + width) / (float)type);
    codepoint.uv[3] =
        1.0f - static_cast<float>(float(offset[1] + height) / (float)type);

    codepoint.tex = ftex;
    codepoint.szs[0] = width;
    codepoint.szs[1] = height;
    codepoint.off = baseline + yOffset;

    for (int y = 0; y < height; ++y) {
      for (int x = 0; x < width; ++x) {
        int map_pos = ((offset[1] + y) * type + (offset[0] + x)) * 4;

        fmap[map_pos + 0] = 255;
        fmap[map_pos + 1] = 255;
        fmap[map_pos + 2] = 255;
        fmap[map_pos + 3] = bitmap[x + y * width];
      }
    }
    offset[0] += width;
    if (offset[0] + width > type) {
      offset[1] += pixel_height;
      offset[0] = 0;
    }
    free(bitmap);
    cpmap[c] = codepoint;
  }
  ftex->LoadPixels(fmap, type, type);
  this->tex.push_back(ftex);
}

void LIFont::LoadBitmapFont(const std::string& path) {}

void LIFont::LoadSystemFont() {
  Palladium::Ftrace::ScopedTrace st("li", "sysfnt");
  sysfnt = true;
  name = "System Font";
  // Code to Load the System Font
  const auto fnt = fontGetSystemFont();
  const auto fnt_info = fontGetInfo(fnt);
  const auto glyph_info = fontGetGlyphInfo(fnt);
  this->tex.resize(glyph_info->nSheets + 1);
  pixel_height = glyph_info->cellHeight;
  for (size_t i = 0; i < glyph_info->nSheets; i++) {
    auto stex = Texture::New();
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
    stex->ExternalLoad(tx, NVec2(tx->width, tx->height), NVec4(0, 1, 1, 0));
    tex[i] = stex;
  }
  std::vector<unsigned int> charSet;
  for (auto cmap = fnt_info->cmap; cmap; cmap = cmap->next) {
    if (cmap->mappingMethod == CMAP_TYPE_DIRECT) {
      if (cmap->codeEnd >= cmap->codeBegin) {
        charSet.reserve(charSet.size() + cmap->codeEnd - cmap->codeBegin + 1);
        for (auto i = cmap->codeBegin; i <= cmap->codeEnd; ++i) {
          if (cmap->indexOffset + (i - cmap->codeBegin) == 0xFFFF) break;
          charSet.emplace_back(i);
        }
      }
    } else if (cmap->mappingMethod == CMAP_TYPE_TABLE) {
      if (cmap->codeEnd >= cmap->codeBegin) {
        charSet.reserve(charSet.size() + cmap->codeEnd - cmap->codeBegin + 1);
        for (auto i = cmap->codeBegin; i <= cmap->codeEnd; ++i) {
          if (cmap->indexTable[i - cmap->codeBegin] == 0xFFFF) continue;
          charSet.emplace_back(i);
        }
      }
    } else if (cmap->mappingMethod == CMAP_TYPE_SCAN) {
      charSet.reserve(charSet.size() + cmap->nScanEntries);
      for (unsigned i = 0; i < cmap->nScanEntries; ++i) {
        if (cmap->scanEntries[i].code >= cmap->codeBegin &&
            cmap->scanEntries[i].code <= cmap->codeEnd) {
          if (cmap->scanEntries[i].glyphIndex != 0xFFFF) {
            charSet.emplace_back(cmap->scanEntries[i].code);
          }
        }
      }
    } else {
      continue;
    }
  }

  std::sort(charSet.begin(), charSet.end());
  charSet.erase(std::unique(charSet.begin(), charSet.end()));

  for (auto cp : charSet) {
    int gidx = fontGlyphIndexFromCodePoint(fnt, cp);
    if (gidx >= 0xFFFF) continue;
    CPI codepoint;
    fontGlyphPos_s dat;
    fontCalcGlyphPos(&dat, fnt, gidx, GLYPH_POS_CALC_VTXCOORD, 1.f, 1.f);

    codepoint.codepoint = cp;
    codepoint.uv[0] = dat.texcoord.left;
    codepoint.uv[1] = dat.texcoord.top;
    codepoint.uv[2] = dat.texcoord.right;
    codepoint.uv[3] = dat.texcoord.bottom;
    if (tex.at(dat.sheetIndex) != nullptr) codepoint.tex = tex[dat.sheetIndex];
    codepoint.szs[0] = dat.vtxcoord.right;
    codepoint.szs[1] = dat.vtxcoord.bottom;
    codepoint.off = 0;
    cpmap[cp] = codepoint;
  }
}

int LIFont::GetPixelHeight() { return this->pixel_height; }

LIFont::CPI LIFont::GetCodepoint(unsigned int c) {
  auto res = cpmap.find(c);
  if (res == cpmap.end()) return CPI(true);
  return res->second;
}

void LIFont::Dump() {
  //   std::ofstream ofs("sdmc:/font.txt", std::ios::out);
  //   ofs << "LI7 Font Dump" << std::endl;
  //   ofs << "Pixel Height: " << (int)pixel_height << std::endl;
  //   for (auto& it : this->cpmap) {
  //     ofs << "  Codepoint: " << (int)it.codepoint << std::endl;
  //     ofs << "  Tex: " << (unsigned int)it.tex->Get() << std::endl;
  //     ofs << "  Szs: (" << (int)it.szs[0] << ", " << (int)it.szs[1] << ")"
  //         << std::endl;
  //     ofs << "  UV: (" << (float)it.uv[0] << ", " << (float)it.uv[1] << ", "
  //         << (float)it.uv[2] << ", " << (float)it.uv[3] << ")" << std::endl;
  //   }
  //   ofs.close();
}

void LI::Init() {
  vertex_buffer.resize(4 * 4096);
  idx_buffer.resize(6 * 4096);

  li7_dvlb = DVLB_ParseFile((u32*)li7_shader, li7_shader_size);
  shaderProgramInit(&li7_prog);
  shaderProgramSetVsh(&li7_prog, &li7_dvlb->DVLE[0]);
  uLoc_proj =
      shaderInstanceGetUniformLocation(li7_prog.vertexShader, "projection");

  AttrInfo_Init(&li7_attr);
  AttrInfo_AddLoader(&li7_attr, 0, GPU_FLOAT, 3);
  AttrInfo_AddLoader(&li7_attr, 1, GPU_FLOAT, 2);
  AttrInfo_AddLoader(&li7_attr, 2, GPU_UNSIGNED_BYTE, 4);

  single_color = Texture::New();
  std::vector<unsigned char> pixels(16 * 16 * 4, 255);
  single_color->LoadPixels(pixels, 16, 16);
  // C3D_Tex* w = new C3D_Tex;
  // C3D_TexInit(w, 16, 16, GPU_L8);
  // C3D_TexLoadImage(w, pixels.data(), GPU_TEXFACE_2D, 0);
  // single_color->ExternalLoad(w, NVec2(16, 16), NVec4(0, 1, 1, 0));

  font = LIFont::New();
  font->LoadSystemFont();
}

void LI::Exit() {
  shaderProgramFree(&li7_prog);
  DVLB_Free(li7_dvlb);
}

void LI::OnScreen(bool bottom) {
  screen_size[0] = bottom ? 320 : 400;
  screen_size[1] = 240;
  bottom_screen = bottom;
}

bool LI::CompareCommands(const LI::Cmd& a, const LI::Cmd& b) {
  if (a.layer == b.layer) {
    if (a.tex == b.tex) return a.index < b.index;
    return a.tex > b.tex;
  }
  return a.layer < b.layer;
}

void LI::RotateCorner(NVec2& v, float s, float c) {
  float x = v[0] * c - v[1] * s;
  float y = v[1] * c + v[0] * s;
  v = NVec2(x, y);
}

void LI::MakeRect(NVec4& top, NVec4& bot, NVec2 pos, NVec2 szs, float angle) {
  NVec2 c;                                   // Center
  NVec2 tl(-c[0], -c[1]);                    // Top Left Corner
  NVec2 tr(-c[0] + szs[0], -c[1]);           // Top Right Corner
  NVec2 bl(-c[0], -c[1] + szs[1]);           // Bottom Left Corner
  NVec2 br(-c[0] + szs[0], -c[1] + szs[1]);  // Bottom Right Corner

  // Calculate Rotation if required
  if (angle != 0.f) {
    float s = std::sin(angle);
    float c = std::cos(angle);
    RotateCorner(tl, s, c);
    RotateCorner(tr, s, c);
    RotateCorner(bl, s, c);
    RotateCorner(br, s, c);
  }

  // Generate top and bottom positions
  top = NVec4(tl + pos + c, tr + pos + c);
  bot = NVec4(bl + pos + c, br + pos + c);
}

void LI::RenderFrame(bool bottom) {
  Palladium::Ftrace::ScopedTrace st(
      "LI", "Render" + std::string(bottom ? "Bot" : "Top"));
  // Create and Setup Projection Matrix
  C3D_Mtx proj;
  Mtx_OrthoTilt(&proj, 0.f, (bottom ? 320 : 400), screen_size[1], 0.f, 1.f,
                -1.f, false);
  C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, uLoc_proj, &proj);

  // Disable Depth Test
  C3D_DepthTest(false, GPU_GREATER, GPU_WRITE_ALL);

  bool sfr = false;

  // Set Tex Env to use Texture 0
  C3D_TexEnv* env = C3D_GetTexEnv(0);
  C3D_TexEnvInit(env);
  C3D_TexEnvSrc(env, C3D_Both, GPU_TEXTURE0, GPU_PRIMARY_COLOR, (GPU_TEVSRC)0);
  C3D_TexEnvFunc(env, C3D_Both, GPU_MODULATE);

  // Setup vars for performance data
  int total_vertices = 0;
  int total_indices = 0;
  num_drawcalls = 0;

  // Get DrawCmd List and reverse it
  auto& draw_cmds = draw_lists[bottom];
  if (flags & PDLithiumFlags_LRS) {
    std::sort(draw_cmds.begin(), draw_cmds.end(), CompareCommands);
  }
  num_commands = draw_cmds.size();
  size_t index = 0;
  // Process Command List
  while (index < draw_cmds.size()) {
    // Get Active Texture and Setup Vertex List for every command with same tex
    C3D_Tex* texture = draw_cmds[index].tex->Get();
    size_t start_vtx = vertex_index;
    size_t start_idx = idx_index;

    while (index < draw_cmds.size() && draw_cmds[index].tex->Get() == texture) {
      auto c = draw_cmds[index];
      if (c.sfr != sfr) {
        if (c.sfr == 0) {
          C3D_TexEnv* env = C3D_GetTexEnv(0);
          C3D_TexEnvInit(env);
          C3D_TexEnvSrc(env, C3D_Both, GPU_TEXTURE0, GPU_PRIMARY_COLOR,
                        (GPU_TEVSRC)0);
          C3D_TexEnvFunc(env, C3D_Both, GPU_MODULATE);
        } else if (c.sfr == 1) {
          C3D_TexEnv* env = C3D_GetTexEnv(0);
          C3D_TexEnvInit(env);
          C3D_TexEnvSrc(env, C3D_RGB, GPU_PRIMARY_COLOR, (GPU_TEVSRC)0,
                        (GPU_TEVSRC)0);
          C3D_TexEnvFunc(env, C3D_RGB, GPU_REPLACE);
          C3D_TexEnvSrc(env, C3D_Alpha, GPU_PRIMARY_COLOR, GPU_TEXTURE0,
                        (GPU_TEVSRC)0);
          C3D_TexEnvFunc(env, C3D_Alpha, GPU_MODULATE);
        }
        sfr = c.sfr;
      }
      // FCS
      if (c.fcs) {
        c.top[0] = std::floor(c.top[0]);
        c.top[1] = std::floor(c.top[1]);
        c.top[2] = std::ceil(c.top[2]);
        c.top[3] = std::floor(c.top[3]);
        c.bot[0] = std::floor(c.bot[0]);
        c.bot[1] = std::ceil(c.bot[1]);
        c.bot[2] = std::ceil(c.bot[2]);
        c.bot[3] = std::ceil(c.bot[3]);
      }
      if (c.cmd_type == 1) {
        // Tiangle 1
        idx_buffer[idx_index++] = vertex_index + 0;
        idx_buffer[idx_index++] = vertex_index + 1;
        idx_buffer[idx_index++] = vertex_index + 2;
        // Tirangle 2
        idx_buffer[idx_index++] = vertex_index + 0;
        idx_buffer[idx_index++] = vertex_index + 2;
        idx_buffer[idx_index++] = vertex_index + 3;

        // Vertices
        vertex_buffer[vertex_index++] =
            Vtx(NVec2(c.bot[2], c.bot[3]), c.uv[2], c.uv[3], c.clr);
        vertex_buffer[vertex_index++] =
            Vtx(NVec2(c.top[2], c.top[3]), c.uv[2], c.uv[1], c.clr);
        vertex_buffer[vertex_index++] =
            Vtx(NVec2(c.top[0], c.top[1]), c.uv[0], c.uv[1], c.clr);
        vertex_buffer[vertex_index++] =
            Vtx(NVec2(c.bot[0], c.bot[1]), c.uv[0], c.uv[3], c.clr);
      } else if (c.cmd_type == 2) {
        // Triangle
        idx_buffer[idx_index++] = vertex_index + 0;
        idx_buffer[idx_index++] = vertex_index + 1;
        idx_buffer[idx_index++] = vertex_index + 2;
        // Vertices
        vertex_buffer[vertex_index++] =
            Vtx(NVec2(c.bot[0], c.bot[1]), c.uv[0], c.uv[3], c.clr);
        vertex_buffer[vertex_index++] =
            Vtx(NVec2(c.top[2], c.top[3]), c.uv[2], c.uv[3], c.clr);
        vertex_buffer[vertex_index++] =
            Vtx(NVec2(c.top[0], c.top[1]), c.uv[0], c.uv[1], c.clr);
      } else if (c.cmd_type == 3) {
        for (size_t i = 1; i < (size_t)c.top[3] - 1; i++) {
          idx_buffer[idx_index++] = vertex_index;
          idx_buffer[idx_index++] = vertex_index + i + 1;
          idx_buffer[idx_index++] = vertex_index + i;
        }
        float as = 2.0f * M_PI / c.top[3];
        for (int i = 0; i < (int)c.top[3]; i++) {
          float a = i * as;
          float x = c.top[0] + c.top[2] * cos(a);
          float y = c.top[1] + c.top[2] * sin(a);
          vertex_buffer[vertex_index++] =
              Vtx(NVec2(x, y), (cos(a) + 1) / 2, (sin(a) + 1) / 2, c.clr);
        }
      }
      index++;
    }

    // Bind Texture
    C3D_TexBind(0, texture);

    // Setup Buffer
    C3D_BufInfo* bufinfo = C3D_GetBufInfo();
    BufInfo_Init(bufinfo);
    BufInfo_Add(bufinfo, vertex_buffer.data(), sizeof(Vtx), 3, 0x210);

    // Draw
    C3D_DrawElements(GPU_TRIANGLES, idx_index - start_idx, C3D_UNSIGNED_SHORT,
                     idx_buffer.data() + start_idx);

    num_drawcalls++;
    total_vertices += vertex_index - start_vtx;
    total_indices += idx_index - start_idx;
  }
  draw_cmds.clear();
  // Enable DepthTest
  C3D_DepthTest(true, GPU_GREATER, GPU_WRITE_ALL);
  num_vertices = total_vertices;
  num_indices = total_indices;
  active_texture = nullptr;
}

void LI::Render(C3D_RenderTarget* top, C3D_RenderTarget* bot) {
  Palladium::Ftrace::ScopedTrace st("Li7", "Render");
  if (font_update) {
    text_sizes.clear();
    font_update = false;
  }
  vertex_index = 0;
  idx_index = 0;
  C3D_BindProgram(&li7_prog);
  C3D_SetAttrInfo(&li7_attr);
  C3D_FrameDrawOn(top);
  RenderFrame(false);
  int d_tmp_cmds1 = num_commands;
  int d_tmp_dcls1 = num_drawcalls;
  int d_tmp_vtxs1 = num_vertices;
  int d_tmp_idx1 = num_indices;
  C3D_FrameDrawOn(bot);
  RenderFrame(true);
  num_commands += d_tmp_cmds1;
  num_drawcalls += d_tmp_dcls1;
  num_vertices += d_tmp_vtxs1;
  num_indices += d_tmp_idx1;
  layer = 0;
  cmd_index = 0;
  if (flags & PDLithiumFlags_TMS) {
    std::vector<std::string> rem;
    for (auto& it : text_sizes) {
      if (Palladium::GetTime() - it.second.time_created > 5)
        rem.push_back(it.first);
    }
    for (auto it : rem) text_sizes.erase(it);
  } else {
    if (text_sizes.size()) text_sizes.clear();
  }
}

void LI::DrawRect(NVec2 pos, NVec2 size, unsigned int clr, NVec4 uvs) {
  Cmd c;
  if (pos[0] > screen_size[0] || pos[0] + size[0] < 0 ||
      pos[1] > screen_size[1] || pos[1] + size[1] < 0)
    return;

  MakeRect(c.top, c.bot, pos, size);
  c.fcs = flags & PDLithiumFlags_FCS;
  c.uv = uvs;
  c.clr = clr;
  c.tex = active_texture;
  c.cmd_type = 1;
  c.layer = layer;
  c.sfr = sysfont_render;
  c.index = cmd_index++;
  sysfont_render = false;
  draw_lists[bottom_screen].push_back(c);
}

void LI::DrawTriangle(NVec2 a, NVec2 b, NVec2 c, unsigned int clr) {
  UseTexture();
  Cmd cmd;
  if ((a[0] > screen_size[0] && b[0] > screen_size[0] &&
       c[0] > screen_size[0]) ||
      (a[1] > screen_size[1] && b[1] > screen_size[1] &&
       c[1] > screen_size[1]) ||
      (a[0] < 0 && b[0] < 0 && c[0] < 0) || (a[1] < 0 && b[1] < 0 && c[1] < 0))
    return;
  cmd.fcs = flags & PDLithiumFlags_FCS;
  cmd.top = NVec4(a, b);
  cmd.bot = NVec4(c, NVec2());
  cmd.uv = NVec4(0, 1, 1, 0);
  cmd.layer = layer;
  cmd.clr = clr;
  cmd.tex = active_texture;
  cmd.cmd_type = 2;
  cmd.index = cmd_index++;
  draw_lists[bottom_screen].push_back(cmd);
}

void LI::DrawCircle(NVec2 pos, float r, unsigned int color, int segments) {
  if (segments < 3) return;
  Cmd c;
  // Not Tested yet ...
  c.fcs = flags & PDLithiumFlags_FCS;
  c.top = NVec4(pos, NVec2(r, segments));
  c.bot = NVec4();
  c.uv = NVec4(0, 1, 1, 0);
  c.clr = color;
  c.tex = active_texture;
  c.layer = layer;
  c.cmd_type = 3;
  c.index = cmd_index++;
  draw_lists[bottom_screen].push_back(c);
}

void LI::DrawLine(NVec2 a, NVec2 b, unsigned int clr, int t) {
  UseTexture();
  NVec2 direction = {b[0] - a[0], b[1] - a[1]};
  float length =
      std::sqrt(direction[0] * direction[0] + direction[1] * direction[1]);
  NVec2 unit_direction = {direction[0] / length, direction[1] / length};
  NVec2 perpendicular = {-unit_direction[1], unit_direction[0]};
  float half_t = t / 2.0f;
  NVec2 offset = {perpendicular[0] * half_t, perpendicular[1] * half_t};

  // Calculate corner positions
  float px0 = a[0] + offset[0];
  float py0 = a[1] + offset[1];
  float px1 = b[0] + offset[0];
  float py1 = b[1] + offset[1];
  float px2 = a[0] - offset[0];
  float py2 = a[1] - offset[1];
  float px3 = b[0] - offset[0];
  float py3 = b[1] - offset[1];

  Cmd c;
  // Schould be always true as lines otherwise could disappear for some reason
  c.fcs = flags & PDLithiumFlags_FCS;
  c.top = NVec4(px2, py2, px3, py3);
  c.bot = NVec4(px0, py0, px1, py1);
  c.uv = NVec4(0, 1, 1, 0);
  c.clr = clr;
  c.tex = active_texture;
  c.layer = layer;
  c.cmd_type = 1;
  c.index = cmd_index++;
  draw_lists[bottom_screen].push_back(c);
}

std::string LI::WrapText(const std::string& in, int maxlen, NVec2& dim) {
  if (flags & PDLithiumFlags_TMS) {
    if (text_sizes.find(in) != text_sizes.end()) {
      text_sizes[in].time_created = Palladium::GetTime();
      dim = text_sizes[in].size;
      if (text_sizes[in].optinal) return text_sizes[in].text;
    }
  }
  std::string out;
  std::string line;
  int line_x = 0;
  std::istringstream istream(in);
  std::string temp;

  while (istream >> temp) {
    NVec2 dim = GetTextDimensions(line + temp);
    if (line_x + dim[0] <= maxlen) {
      line += temp + ' ';
      line_x += dim[0];
    } else {
      out += line + '\n';
      line = temp + ' ';
      line_x = GetTextDimensions(line)[0];
    }
  }
  out += line;
  dim = GetTextDimensions(out);
  if (flags & PDLithiumFlags_TMS) {
    text_sizes[in] = TextBox{dim, Palladium::GetTime(), true, out};
  }
  return out;
}

std::string LI::ShortText(const std::string& in, int maxlen, NVec2& dim) {
  auto textdim = GetTextDimensions(in);
  if (textdim[0] < (float)maxlen) return in;
  if (flags & PDLithiumFlags_TMS) {
    if (text_sizes.find(in) != text_sizes.end()) {
      text_sizes[in].time_created = Palladium::GetTime();
      dim = text_sizes[in].size;
      if (text_sizes[in].optinal) return text_sizes[in].text;
    }
  }
  std::string ext = "";
  std::string ph = "(...)";  // placeholder
  std::string worker = in;
  std::string out;
  size_t ext_pos = in.find_last_of('.');
  if (ext_pos != in.npos) {
    ext = in.substr(ext_pos);
    worker = in.substr(0, ext_pos);
  }

  maxlen -= GetTextDimensions(ext)[0];
  maxlen -= GetTextDimensions(ph)[0];

  for (auto& it : worker) {
    if (GetTextDimensions(out)[0] > (float)maxlen) {
      out += ph;
      out += ext;
      dim = GetTextDimensions(out);
      if (flags & PDLithiumFlags_TMS) {
        text_sizes[in] = TextBox{dim, Palladium::GetTime(), true, out};
      }
      return out;
    }
    out += it;
  }
  return out;  // Impossible to reach
}

NVec2 LI::GetTextDimensions(const std::string& text) {
  if (!font) return NVec2();
  if (flags & PDLithiumFlags_TMS) {
    if (text_sizes.find(text) != text_sizes.end()) {
      text_sizes[text].time_created = Palladium::GetTime();
      return text_sizes[text].size;
    }
  }
  // FONT
  auto txt = make_wstring(text + "\0");
  NVec2 pos(0, 0);  // Temp Pos
  NVec2 offset;
  float txt_scale = text_scale;
  if (font->IsSystemFont()) txt_scale *= 0.9;
  float gapm = 1;
  float maxWidth = 0.f;
  float ntxtszs = default_font_size * txt_scale;
  float cpm = ntxtszs / font->GetPixelHeight();
  float line_height = font->GetPixelHeight() * cpm;

  for (size_t i = 0; i < txt.length(); i++) {
    if (txt[i] == '\0') break;
    auto cp = font->GetCodepoint(txt[i]);
    if (cp.invalid && txt[i] != '\n' && txt[i] != ' ' && txt[i] != '\t')
      continue;
    bool implicitBreak = false;
    if (txt[i] == '\n' || implicitBreak) {
      offset[1] += line_height;
      maxWidth = std::max(maxWidth, offset[0]);
      offset[0] = 0;
      if (implicitBreak) continue;
    } else if (txt[i] == '\t') {
      offset[0] = ((offset[0] / ntxtszs) / 4 + 1) * 4 * ntxtszs;
    } else {
      if (txt[i] == ' ') {
        // this will make the space twice
        if (!font->IsSystemFont()) offset[0] += 4 * gapm * txt_scale;
      }
      if (i == txt.length() - 1)
        offset[0] += cp.szs[0] * cpm + txt_scale;
      else
        offset[0] += cp.szs[0] * cpm + (gapm * txt_scale);
    }
  }
  maxWidth = std::max(maxWidth, offset[0]);
  NVec2 res = NVec2(maxWidth, offset[1] + (default_font_size * txt_scale));
  if (flags & PDLithiumFlags_TMS)
    text_sizes[text] = TextBox{res, Palladium::GetTime(), false, ""};
  return res;
}

void LI::DrawText(NVec2 pos, unsigned int color, const std::string& text,
                  PDTextFlags flags, NVec2 ap) {
  if (!font) return;
  PDLithiumFlags tmp_flags = flags;
  // Do not use FCS in Text Rendering...
  flags &= ~PDLithiumFlags_FCS;
  std::string txt = text;
  NVec2 offset;
  float txt_scale = text_scale;
  if (font->IsSystemFont()) txt_scale *= 0.9;
  float gapm = 1;
  float ntxtszs = default_font_size * txt_scale;
  float cpm = ntxtszs / font->GetPixelHeight();
  float line_height = font->GetPixelHeight() * cpm;
  NVec2 td;
  if (flags & PDTextFlags_Wrap)
    txt = WrapText(text, ap[0] - pos[0], td);
  else if (flags & PDTextFlags_Short)
    txt = ShortText(text, ap[0] - pos[0], td);
  if (td[0] == 0 && td[1] == 0) td = GetTextDimensions(text);
  if (flags & PDTextFlags_AlignRight)
    pos[0] -= td[0];
  else if (flags & PDTextFlags_AlignMid) {
    pos[0] = (ap[0] * 0.5) - (td[0] * 0.5) + pos[0];
  }
  std::vector<std::string> lines;
  std::istringstream iss(txt);
  std::string temp;
  while (std::getline(iss, temp)) {
    lines.push_back(temp);
  }

  for (auto& it : lines) {
    if (pos[1] + offset[1] + line_height < 0) {
      offset[1] += line_height;
      continue;
    } else if (pos[1] + offset[1] > screen_size[1]) {
      // Break func as we dont want loop over lines that get skipped too
      break;
    }
    auto wstr = make_wstring(it);
    // Loop over line
    for (auto& jt : wstr) {
      auto cp = font->GetCodepoint(jt);
      if (cp.invalid && jt != '\n' && jt != ' ' && jt != '\t') continue;
      active_texture = cp.tex;
      if (jt == '\t') {
        offset[0] = ((offset[0] / ntxtszs) / 4 + 1) * 4 * ntxtszs;
      } else {
        if (jt != ' ') {
          int lr = layer;
          if (flags & PDTextFlags_Shaddow) {
            sysfont_render = true;
            DrawRect(
                pos + NVec2(offset[0] + 1, (offset[1] + (cp.off * cpm)) + 1),
                NVec2(cp.szs[0] * cpm, cp.szs[1] * cpm),
                Palladium::Color::RGBA(color).is_light() ? 0xff111111
                                                         : 0xffeeeeee,
                cp.uv);
            layer++;
          }
          sysfont_render = true;
          DrawRect(pos + offset + NVec2(0, (cp.off * cpm)),
                   NVec2(cp.szs[0] * cpm, cp.szs[1] * cpm), color, cp.uv);
          layer = lr;

        } else {
          // this will make the space twice
          if (!font->IsSystemFont()) offset[0] += 4 * gapm * txt_scale;
        }
        offset[0] += cp.szs[0] * cpm + (gapm * txt_scale);
      }
    }
    offset[1] += line_height;
    offset[0] = 0;
  }
  flags = tmp_flags;
}
}  // namespace Palladium