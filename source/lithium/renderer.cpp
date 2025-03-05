/*
MIT License
Copyright (c) 2024 - 2025 René Amthor (tobid7)

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

#include <3ds.h>
#include <pd/external/stb_truetype.h>

#include <pd/core/io.hpp>
#include <pd/core/strings.hpp>
#include <pd/core/sys.hpp>
#include <pd/lithium/font.hpp>
#include <pd/lithium/li7_shader.hpp>
#include <pd/lithium/renderer.hpp>

namespace PD {
namespace LI {
Renderer::Renderer(LIRenderFlags flags) {
  vertex_buf.resize(4 * 4096, Vertex());
  index_buf.resize(6 * 4096, 0);

  /// Use 3ds u32 here
  dvlb = DVLB_ParseFile((uint32_t*)li7_shader, li7_shader_size);
  shaderProgramInit(&shader);
  shaderProgramSetVsh(&shader, &dvlb->DVLE[0]);
  uLoc_projection =
      shaderInstanceGetUniformLocation(shader.vertexShader, "projection");

  AttrInfo_Init(&attr);
  AttrInfo_AddLoader(&attr, 0, GPU_FLOAT, 2);
  AttrInfo_AddLoader(&attr, 1, GPU_FLOAT, 2);
  AttrInfo_AddLoader(&attr, 2, GPU_UNSIGNED_BYTE, 4);

  // Precalculate Projection (Never changes)
  Mtx_OrthoTilt(&top_proj, 0.f, 400.f, 240.f, 0.f, 1.f, -1.f, false);
  Mtx_OrthoTilt(&bot_proj, 0.f, 320.f, 240.f, 0.f, 1.f, -1.f, false);

  std::vector<u8> pixels(16 * 16 * 4, 255);
  white = Texture::New(pixels, 16, 16);
  UseTex(white);

  // Not Loading as Systemfont is freezing
  // font = Font::New();
  // font->LoadSystemFont();
}
Renderer::~Renderer() {
  shaderProgramFree(&shader);
  DVLB_Free(dvlb);
}

bool Renderer::InBox(const vec2& pos, const vec2& szs, const vec4& rect) {
  return (pos[0] + szs[0] >= rect[0] && pos[1] + szs[1] >= rect[1] &&
          pos[0] <= rect[2] && pos[1] <= rect[3]);
}

bool Renderer::InBox(const vec2& pos, const vec4& rect) {
  return (pos.x() > rect.x() && pos.x() < rect.x() + rect.z() &&
          pos.y() > rect.y() && pos.y() < rect.y() + rect.w());
}

bool Renderer::InBox(const vec2& alpha, const vec2& bravo, const vec2& charlie,
                     const vec4& rect) {
  return ((alpha[0] < rect[2] && bravo[0] < rect[2] && charlie[0] < rect[2]) ||
          (alpha[1] < rect[3] && bravo[1] < rect[3] && charlie[1] < rect[3]) ||
          (alpha[0] > 0 && bravo[0] > 0 && charlie[0] > 0) ||
          (alpha[1] > 0 && bravo[1] > 0 && charlie[1] > 0));
}

void Renderer::RotateCorner(vec2& v, float s, float c) {
  float x = v[0] * c - v[1] * s;
  float y = v[1] * c + v[0] * s;
  v = vec2(x, y);
}

Rect Renderer::CreateRect(const vec2& pos, const vec2& size, float angle) {
  vec2 c = size * 0.5f;  // Center
  vec2 corner[4] = {
      vec2(-c[0], -c[1]),
      vec2(-c[0] + size[0], -c[1]),
      vec2(-c[0], -c[1] + size[1]),
      vec2(-c[0] + size[0], -c[1] + size[1]),
  };

  // Only rotate if required
  if (angle != 0.f) {
    float s = std::sin(angle);
    float co = std::cos(angle);
    for (int i = 0; i < 4; i++) {
      RotateCorner(corner[i], s, co);
    }
  }

  // Return Result
  return Rect(corner[0] + pos + c, corner[1] + pos + c, corner[2] + pos + c,
              corner[3] + pos + c);
}

Rect Renderer::CreateLine(const vec2& a, const vec2& b, int t) {
  // Usin g th evec maths api makes the code as short as it is
  vec2 dir = a - b;
  float len = dir.len();
  vec2 unit_dir = dir / len;
  vec2 perpendicular(-unit_dir.y(), unit_dir.x());
  vec2 off = perpendicular * ((float)t * 0.5f);

  return Rect(a + off, b + off, a - off, b - off);
}

void Renderer::OptiCommandList(std::vector<Command::Ref>& list) {
  std::sort(list.begin(), list.end(), [](Command::Ref a, Command::Ref b) {
    if (a->Layer() == b->Layer()) {
      if (a->Tex() == b->Tex()) {
        return a->Index() < b->Index();
      }
      return a->Tex() < b->Tex();  // else
    }
    return a->Layer() < b->Layer();  // else
  });
}

void Renderer::SetupCommand(Command::Ref cmd) {
  cmd->Index(cmd_idx++).Layer(current_layer).Tex(current_tex);
}

void Renderer::QuadCommand(Command::Ref cmd, const Rect& quad, const Rect& uv,
                           u32 col) {
  cmd->PushIndex(0).PushIndex(1).PushIndex(2);
  cmd->PushIndex(0).PushIndex(2).PushIndex(3);
  cmd->PushVertex(Vertex(quad.BotRight(), uv.BotRight(), col));
  cmd->PushVertex(Vertex(quad.TopRight(), uv.TopRight(), col));
  cmd->PushVertex(Vertex(quad.TopLeft(), uv.TopLeft(), col));
  cmd->PushVertex(Vertex(quad.BotLeft(), uv.BotLeft(), col));
}

void Renderer::TriangleCommand(Command::Ref cmd, const vec2& a, const vec2& b,
                               const vec2& c, u32 col) {
  cmd->Index(cmd_idx++).Layer(current_layer).Tex(current_tex);
  cmd->PushIndex(2).PushIndex(1).PushIndex(0);
  cmd->PushVertex(Vertex(a, vec2(0.f, 1.f), col));
  cmd->PushVertex(Vertex(b, vec2(1.f, 1.f), col));
  cmd->PushVertex(Vertex(c, vec2(1.f, 0.f), col));
}

void Renderer::TextCommand(std::vector<Command::Ref>& cmds, const vec2& pos,
                           u32 color, const std::string& text,
                           LITextFlags flags, const vec2& box) {
  if (!font) {
    return;
  }
  vec2 off;
  float cfs = (default_font_h * text_size) / (float)font->PixelHeight();
  float lh = (float)font->PixelHeight() * cfs;
  vec2 td;
  vec2 rpos = pos;
  vec2 rbox = box;
  if (flags & (LITextFlags_AlignMid | LITextFlags_AlignRight)) {
    td = GetTextDimensions(text);
    if (rbox[0] == 0.f) {
      rbox[0] = area_size.x();
    }
    if (rbox[1] == 0.f) {
      rbox[1] = area_size.y();
    }
  }
  if (flags & LITextFlags_AlignMid) {
    rpos = rbox * 0.5 - td * 0.5 + pos;
  }
  if (flags & LITextFlags_AlignRight) {
    rpos[0] = rpos[0] - td[0];
  }

  std::vector<std::string> lines;
  std::istringstream iss(text);
  std::string tmp;
  while (std::getline(iss, tmp)) {
    lines.push_back(tmp);
  }

  for (auto& it : lines) {
    if (flags & LITextFlags_Short) {
      vec2 tmp_dim;
      it = ShortText(it, box.x() - pos.x(), tmp_dim);
    }
    /// Well support OOS Rendering here as well
    /// Fixes UI7 Scroll back up bug
    if (rpos[1] + off[1] + lh < 0 && !(flags & LITextFlags_RenderOOS)) {
      off[1] += lh;
      continue;
    } else if (rpos[1] + off[1] > GetViewport().w() &&
               !(flags & LITextFlags_RenderOOS)) {
      // Break cause next lines would be out of screen
      break;
    }
    auto wline = Strings::MakeWstring(it);
    auto cmd = Command::New();
    current_tex = font->GetCodepoint(wline[0]).tex();
    SetupCommand(cmd);
    cmd->Rendermode(RenderMode_Font);
    for (auto& jt : wline) {
      auto cp = font->GetCodepoint(jt);
      if (cp.invalid() && jt != '\n' && jt != '\t') {
        continue;
      }
      if (current_tex != cp.tex()) {
        cmds.push_back(cmd);
        cmd = Command::New();
        current_tex = cp.tex();
        SetupCommand(cmd);
        cmd->Rendermode(RenderMode_Font);
      }
      if (jt == '\t') {
        off[0] += 16 * cfs;
      } else {
        if (jt != ' ') {
          int lr = current_layer;
          if (flags & LITextFlags_Shaddow) {
            // Draw
            Rect rec = CreateRect(
                rpos + vec2(off[0] + 1, off[1] + (cp.off() * cfs)) + 1,
                cp.size() * cfs, 0.f);
            QuadCommand(cmd, rec, cp.uv(), 0xff111111);
            current_layer++;
          }
          // Draw
          Rect rec = CreateRect(rpos + off + vec2(0, (cp.off() * cfs)),
                                cp.size() * cfs, 0.f);
          QuadCommand(cmd, rec, cp.uv(), color);
          current_layer = lr;
        } else {
          if (!font->SystemFont()) {
            off[0] += 2 * cfs;
          }
        }
        off[0] += cp.size().x() * cfs + 2 * cfs;
      }
    }
    cmds.push_back(cmd);
    off[1] += lh;
    off[0] = 0;
  }
}

vec4 Renderer::GetViewport() { return vec4(vec2(), screen->GetSize()); }

std::string Renderer::ShortText(const std::string& text, int maxlen,
                                vec2& newsize) {
  vec2 cdim;
  if (flags & LIRenderFlags_TMS) {
    auto e = tms.find(text);
    if (e != tms.end()) {
      e->second.TimeCreated(Sys::GetTime());
      if (e->second.Optional()) {
        return e->second.Text();
      }
      cdim = e->second.Size();
    }
  }
  cdim = this->GetTextDimensions(text);
  if (cdim[0] < (float)maxlen) {
    return text;
  }
  std::string ext;
  /// Forgot why i called this var ending cause
  /// Its more a placeholder for removed content
  std::string ending = "...";
  std::string cpy = text;
  std::string res;
  size_t extension = text.find_last_of('.');
  if (extension != text.npos) {
    ext = text.substr(extension);
    cpy = text.substr(0, extension);
    maxlen -= GetTextDimensions(ext).x();
  }
  maxlen -= GetTextDimensions(ending).x();
  for (auto& it : cpy) {
    if (GetTextDimensions(res).x() > (float)maxlen) {
      res += ending;
      res += ext;
      newsize = GetTextDimensions(res);
      if (flags & LIRenderFlags_TMS) {
        auto& tmp = tms[text];
        tmp.Text(res);
        tmp.Size(newsize);
        tmp.TimeCreated(Sys::GetTime());
        tmp.Optional(true);
      }
      break;
    }
    res += it;
  }
  return res;
}

std::string Renderer::WrapText(const std::string& text, int maxlen,
                               vec2& newsize) {}

vec2 Renderer::GetTextDimensions(const std::string& text) {
  if (!font) {
    // No font no size (oder so)
    return vec2();
  }
  // Handle TextMapSystem
  if (flags & LIRenderFlags_TMS) {
    auto ref = tms.find(text);
    if (ref != tms.end()) {
      ref->second.TimeCreated(Sys::GetTime());
      return ref->second.Size();
    }
  }
  // Use wstring for exemple for german äöü
  auto wtext = Strings::MakeWstring(text);
  // Create a temp position and offset as [0, 0]
  vec2 res;
  float x = 0;
  // Curent Font Scale
  float cfs = (default_font_h * text_size) / (float)font->PixelHeight();
  float lh = (float)font->PixelHeight() * cfs;
  size_t index = 0;
  for (auto& it : wtext) {
    if (it == '\0') {
      break;
    }
    index++;
    auto cp = font->GetCodepoint(it);
    if (cp.invalid() && it != '\n' && it != '\t' && it != ' ') {
      continue;
    }
    switch (it) {
      case '\n':
        res[1] += lh;
        res[0] = std::max(res[0], x);
        x = 0.f;
        break;
      case '\t':
        x += 16 * cfs;
        break;
      case ' ':
        if (!font->SystemFont()) {
          x += 2 * cfs;
        }
      // Fall trough here to get the same result as in
      // TextCommand if/else Section
      default:
        x += cp.size().x() * cfs;
        if (index != wtext.size()) {
          x += 2 * cfs;
        }
        break;
    }
  }
  res[0] = std::max(res[0], x);
  res[1] += lh;
  if (flags & LIRenderFlags_TMS) {
    tms[text] = TextBox(res, Sys::GetTime());
  }
  return res;
}

void Renderer::UpdateRenderMode(const RenderMode& mode) {
  C3D_TexEnv* env = C3D_GetTexEnv(0);
  switch (mode) {
    case RenderMode_Font:
      /// Sets Only Alpha Using the Color and Replase RGB with vertex color
      C3D_TexEnvInit(env);
      C3D_TexEnvSrc(env, C3D_RGB, GPU_PRIMARY_COLOR);
      C3D_TexEnvFunc(env, C3D_RGB, GPU_REPLACE);
      C3D_TexEnvSrc(env, C3D_Alpha, GPU_TEXTURE0);
      C3D_TexEnvFunc(env, C3D_Alpha, GPU_MODULATE);
      break;
    // Fall trough instead of defining twice
    case RenderMode_RGBA:
    default:
      /// Use Texture for RGBA and vertexcolor for visibility
      C3D_TexEnvInit(env);
      C3D_TexEnvSrc(env, C3D_Both, GPU_TEXTURE0);
      C3D_TexEnvFunc(env, C3D_Both, GPU_MODULATE);
      break;
  }
}

void Renderer::PrepareRender() {
  if (font_update) {
    tms.clear();
    font_update = false;
  }
  C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
  TT::Beg("LI_RenderAll");
  vertex_idx = 0;
  index_idx = 0;
  vertices = 0;
  indices = 0;
  commands = 0;
  drawcalls = 0;
  C3D_BindProgram(&shader);
  C3D_SetAttrInfo(&attr);
}

void Renderer::FinalizeRender() {
  C3D_FrameEnd(0);
  TT::End("LI_RenderAll");
  current_layer = 0;
  cmd_idx = 0;
  rot = 0.f;
  UseTex();
  if (flags & LIRenderFlags_TMS) {
    std::vector<std::string> rem;
    for (auto& it : tms) {
      if (Sys::GetTime() - it.second.TimeCreated() > 5) rem.push_back(it.first);
    }
    for (auto it : rem) tms.erase(it);
  } else {
    tms.clear();
  }
  if (flags & LIRenderFlags_AST) {
    std::vector<u32> rem;
    for (auto it : ast) {
      if (!it.second->Used()) {
        rem.push_back(it.first);
      }
      it.second->SetUnused();
    }
    for (auto& it : rem) {
      ast.erase(it);
    }
  } else {
    ast.clear();
  }
}

void Renderer::Render(Screen::Ref s) {
  Assert(s.get(), "Expected Screen Address but got nullptr!");
  s->Clear();
  s->Use();
  bool bot = s->ScreenType() == Screen::Bottom;
  C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, uLoc_projection,
                   (bot ? &bot_proj : &top_proj));
  C3D_DepthTest(false, GPU_GREATER, GPU_WRITE_ALL);
  UpdateRenderMode(RenderMode_RGBA);
  int total_vertices = 0;
  int total_indices = 0;
  auto& cmds = draw_list[Screen32(s)];
  commands += cmds.size();
  size_t index = 0;
  if (flags & LIRenderFlags_LRS) {
    OptiCommandList(cmds);
  }
  while (index < cmds.size()) {
    C3D_Tex* tex = cmds[index]->Tex()->GetTex();
    auto mode = cmds[index]->Rendermode();
    auto smode = cmds[index]->GetScissorMode();
    auto spos = cmds[index]->ScissorRect();
    C3D_SetScissor((GPU_SCISSORMODE)smode, s->GetSize().y() - spos.w(),
                   s->GetSize().x() - spos.z(), s->GetSize().y() - spos.y(),
                   s->GetSize().x() - spos.x());
    UpdateRenderMode(mode);
    u32 start_vtx = vertex_idx;
    u32 start_idx = index_idx;
    while (index < cmds.size() && cmds[index]->Tex()->GetTex() == tex &&
           cmds[index]->Rendermode() == mode &&
           cmds[index]->GetScissorMode() == smode) {
      auto c = cmds[index];
      // Indices
      for (size_t i = 0; i < c->IndexList().size(); i++) {
        index_buf[index_idx++] = vertex_idx + c->IndexList().at(i);
      }
      // Vertices
      for (size_t i = 0; i < c->VertexList().size(); i++) {
        vertex_buf[vertex_idx++] = c->VertexList().at(i);
      }
      index++;
    }

    C3D_TexBind(0, tex);

    auto bufInfo = C3D_GetBufInfo();
    BufInfo_Init(bufInfo);
    BufInfo_Add(bufInfo, vertex_buf.data(), sizeof(Vertex), 3, 0x210);

    C3D_DrawElements(GPU_TRIANGLES, index_idx - start_idx, C3D_UNSIGNED_SHORT,
                     index_buf.data() + start_idx);

    drawcalls++;
    total_vertices += vertex_idx - start_vtx;
    total_indices += index_idx - start_idx;
  }
  cmds.clear();
  C3D_DepthTest(true, GPU_GREATER, GPU_WRITE_ALL);
  vertices += total_vertices;
  indices += total_indices;
}

void Renderer::DrawRect(const vec2& pos, const vec2& size, u32 color,
                        const Rect& uv) {
  if (!InBox(pos, size, GetViewport())) {
    // Instand abort as it is out of screen
    return;
  }
  Rect rec = CreateRect(pos, size, rot);
  auto cmd = Command::New();
  SetupCommand(cmd);
  QuadCommand(cmd, rec, uv, color);
  draw_list[Screen32(screen)].push_back(cmd);
}

void Renderer::DrawRectSolid(const vec2& pos, const vec2& size, u32 color) {
  UseTex();
  DrawRect(pos, size, color, vec4(0.f, 1.f, 1.f, 0.f));
}

void Renderer::DrawTriangle(const vec2& a, const vec2& b, const vec2& c,
                            u32 color) {
  if (!InBox(a, b, c, GetViewport())) {
    return;
  }
  UseTex();
  auto cmd = Command::New();
  SetupCommand(cmd);
  TriangleCommand(cmd, a, b, c, color);
  draw_list[Screen32(screen)].push_back(cmd);
}

void Renderer::DrawCircle(const vec2& center_pos, float r, u32 color,
                          int segments) {
  if (segments < 3) {
    return;
  }
  auto cmd = Command::New();
  cmd->Index(cmd_idx++).Layer(current_layer).Tex(current_tex);
  for (int i = 1; i < segments - 1; i++) {
    cmd->PushIndex(0);
    cmd->PushIndex(i + 1).PushIndex(i);
  }
  float as = 2.f * M_PI / segments;
  for (int i = 0; i < segments; i++) {
    float a = i * as;
    float x = center_pos.x() + r * std::cos(a);
    float y = center_pos.y() + r * std::sin(a);
    cmd->PushVertex(Vertex(
        vec2(x, y), vec2((std::cos(a) + 1.f) / 2.f, (std::sin(a) + 1.f) / 2.f),
        color));
  }
  draw_list[Screen32(screen)].push_back(cmd);
}

void Renderer::DrawLine(const vec2& a, const vec2& b, u32 color, int t) {
  UseTex();
  Rect line = CreateLine(a, b, t);

  auto cmd = Command::New();
  SetupCommand(cmd);
  QuadCommand(cmd, line, vec4(0.f, 1.f, 1.f, 0.f), color);
  draw_list[Screen32(screen)].push_back(cmd);
}

void Renderer::DrawImage(const vec2& pos, Texture::Ref tex, const vec2& scale) {
  UseTex(tex);
  DrawRect(pos, tex->GetSize() * scale, 0xffffffff, tex->GetUV());
}

void Renderer::DrawText(const vec2& pos, u32 color, const std::string& text,
                        u32 flags, const vec2& ap) {
  if (!font) {
    return;
  }
  if (this->flags & LIRenderFlags_AST) {
    u32 id = Strings::FastHash(text);
    auto e = ast.find(id);
    if (e == ast.end()) {
      ast[id] = StaticText::New();
      e = ast.find(id);
    }
    if (!e->second->IsSetup() || e->second->Font() != font) {
      e->second->Setup(this, pos, color, text, flags, ap);
      e->second->Font(font);
    }
    e->second->SetPos(pos);
    e->second->SetColor(color);
    e->second->Draw();
    return;
  }
  TextCommand(draw_list[Screen32(screen)], pos, color, text, flags, ap);
}
}  // namespace LI
}  // namespace PD