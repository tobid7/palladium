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

#include <pd/core/strings.hpp>
#include <pd/ui7/drawlist.hpp>

namespace PD {
namespace UI7 {
void DrawList::PathArcToN(const vec2& c, float radius, float a_min, float a_max,
                          int segments) {
  // Path.push_back(c);
  PathReserve(segments + 1);
  for (int i = 0; i < segments; i++) {
    float a = a_min + ((float)i / (float)segments) * (a_max - a_min);
    PathNext(vec2(c.x() + std::cos(a) * radius, c.y() + std::sin(a) * radius));
  }
}

void DrawList::PathRect(vec2 a, vec2 b, float rounding, UI7DrawFlags flags) {
  if (rounding == 0.f) {
    PathNext(a);
    PathNext(vec2(b.x(), a.y()));
    PathNext(b);
    PathNext(vec2(a.x(), b.y()));
  } else {
    PathArcToN(vec2(a.x() + rounding, a.y() + rounding), rounding, 4*6, 4*9, 21);
    PathArcToN(vec2(b.x() - rounding, a.y() + rounding), rounding, 4*9, 4*12, 21);
    PathArcToN(vec2(b.x() - rounding, b.y() - rounding), rounding, 4*0, 4*3, 21);
    PathArcToN(vec2(a.x() + rounding, b.y() - rounding), rounding, 4*3, 4*6, 21);
  }
}

void DrawList::AddRect(const vec2& pos, const vec2& size, const UI7Color& clr,
                       int thickness) {
  if (!ren->InBox(pos, size, ren->GetViewport())) {
    return;
  }
  ren->UseTex();
  PathRect(pos, pos + size);
  PathStroke(clr, thickness, UI7DrawFlags_Close);
}
void DrawList::AddRectangle(vec2 pos, vec2 szs, const UI7Color& clr) {
  if (!ren->InBox(pos, szs, ren->GetViewport())) {
    return;
  }
  ren->UseTex();
  PathRect(pos, pos + szs);
  PathFill(clr);
}

void DrawList::AddTriangle(const vec2& a, const vec2& b, const vec2& c,
                           const UI7Color& clr, int thickness) {
  ren->UseTex();
  PathNext(a);
  PathNext(b);
  PathNext(c);
  PathStroke(clr, thickness, UI7DrawFlags_Close);
}

void DrawList::AddTriangleFilled(const vec2& a, const vec2& b, const vec2& c,
                                 const UI7Color& clr) {
  ren->UseTex();
  PathNext(a);
  PathNext(b);
  PathNext(c);
  PathFill(clr);
}

void DrawList::AddCircle(const vec2& pos, float rad, UI7Color col,
                         int num_segments, int thickness) {
  if (num_segments <= 0) {
    // Auto Segment
  } else {
    float am = (M_PI * 2.0f) * ((float)num_segments) / (float)num_segments;
    PathArcToN(pos, rad, 0.f, am, num_segments);
  }
  ren->UseTex();
  PathStroke(col, thickness, UI7DrawFlags_Close);
}

void DrawList::AddCircleFilled(const vec2& pos, float rad, UI7Color col,
                               int num_segments) {
  if (num_segments <= 0) {
    // Auto Segment
  } else {
    float am = (M_PI * 2.0f) * ((float)num_segments) / (float)num_segments;
    PathArcToN(pos, rad, 0.f, am, num_segments);
  }
  ren->UseTex();
  PathFill(col);
}

void DrawList::AddText(vec2 pos, const std::string& text, const UI7Color& clr,
                       LITextFlags flags, vec2 box) {
  if (!ren->Font()) {
    return;
  }
  u32 id = Strings::FastHash(text);
  LI::StaticText::Ref e;
  auto f = static_text.find(id);
  if (static_text.find(id) == static_text.end()) {
    e = LI::StaticText::New();
    static_text[id] = e;
  } else {
    e = f->second;
  }
  if (!e->IsSetup() || e->Font() != ren->Font()) {
    int l = ren->Layer();
    ren->Layer(layer);
    e->Setup(ren.get(), pos, clr, text, flags, box);
    e->Font(ren->Font());
    ren->Layer(l);
  }
  e->SetPos(pos);
  e->SetColor(clr);
  e->SetLayer(layer);
  if (!clip_rects.empty()) {
    e->SetScissorMode(LI::ScissorMode_Normal);
    e->ScissorRect(clip_rects.top());
  }
  for (auto it : e->GetRawObject()->List()) {
    this->commands.push_back(std::make_pair(
        ren->CurrentScreen()->ScreenType() == Screen::Bottom, it));
  }
  e->GetRawObject()->ReCopy();
}

void DrawList::AddImage(vec2 pos, Texture::Ref img, vec2 size, LI::Rect uv) {
  size = size == 0.f ? img->GetSize() : size;
  uv = (uv.Top() == 0.0f && uv.Bot() == 0.0f) ? img->GetUV() : uv;
  if (!ren->InBox(pos, size, ren->GetViewport())) {
    return;
  }
  auto rect = ren->CreateRect(pos, size, 0.f);
  auto cmd = LI::Command::New();
  ren->UseTex(img);
  ren->SetupCommand(cmd);
  cmd->Layer(layer);
  if (!clip_rects.empty()) {
    cmd->SetScissorMode(LI::ScissorMode_Normal);
    cmd->ScissorRect(clip_rects.top());
  }
  ren->QuadCommand(cmd, rect, uv, 0xffffffff);
  commands.push_back(std::make_pair(
      ren->CurrentScreen()->ScreenType() == Screen::Bottom, cmd));
}

void DrawList::AddLine(const vec2& a, const vec2& b, const UI7Color& clr,
                       int t) {
  if (!ren->InBox(a, ren->GetViewport()) &&
      !ren->InBox(b, ren->GetViewport())) {
    return;
  }
  ren->UseTex();
  PathNext(a);
  PathNext(b);
  PathStroke(clr, t);
}

// TODO: Don't render OOS
void DrawList::AddPolyLine(const std::vector<vec2>& points, const UI7Color& clr,
                           UI7DrawFlags flags, int thickness) {
  if (points.size() < 2) {
    return;
  }
  auto cmd = LI::Command::New();
  ren->SetupCommand(cmd);
  cmd->Layer(layer);
  if (!clip_rects.empty()) {
    cmd->SetScissorMode(LI::ScissorMode_Normal);
    cmd->ScissorRect(clip_rects.top());
  }
  bool close = (flags & UI7DrawFlags_Close);
  int num_points = close ? (int)points.size() : (int)points.size() - 1;
  if (flags & UI7DrawFlags_AALines) {
    // TODO: Find a way to draw less garbage looking lines
  } else {
    // Non antialiased lines look awful when rendering with thickness != 1
    for (int i = 0; i < num_points; i++) {
      int j = (i + 1) == (int)points.size() ? 0 : (i + 1);
      auto line = ren->CreateLine(points[i], points[j], thickness);
      ren->QuadCommand(cmd, line, vec4(0.f, 1.f, 1.f, 0.f), clr);
    }
  }
  commands.push_back(std::make_pair(
      ren->CurrentScreen()->ScreenType() == Screen::Bottom, cmd));
}

// TODO: Don't render OOS (Probably make it with a define as it
// would probably be faster to render out of screen than checking if
// it could be skipped)
void DrawList::AddConvexPolyFilled(const std::vector<vec2>& points,
                                   const UI7Color& clr) {
  if (points.size() < 3) {
    return;  // Need at least three points
  }
  auto cmd = LI::Command::New();
  ren->SetupCommand(cmd);
  cmd->Layer(layer);
  if (!clip_rects.empty()) {
    cmd->SetScissorMode(LI::ScissorMode_Normal);
    cmd->ScissorRect(clip_rects.top());
  }
  for (int i = 2; i < (int)points.size(); i++) {
    cmd->PushIndex(0).PushIndex(i).PushIndex(i - 1);
  }
  for (int i = 0; i < (int)points.size(); i++) {
    cmd->PushVertex(LI::Vertex(points[i], vec2(0, 0), clr));
  }
  commands.push_back(std::make_pair(
      ren->CurrentScreen()->ScreenType() == Screen::Bottom, cmd));
}

void DrawList::Clear() { commands.clear(); }

void DrawList::Process() {
  num_vertices = 0;
  num_indices = 0;
  for (auto command : commands) {
    ren->OnScreen(ren->GetScreen(command.first));
    command.second->Layer(command.second->Layer() + base);
    ren->PushCommand(command.second);
    num_vertices += command.second->VertexList().size();
    num_indices += command.second->IndexList().size();
  }
  commands.clear();
  layer = 0;
  std::vector<u32> rem;
  for (auto it : static_text) {
    if (!it.second->Used()) {
      rem.push_back(it.first);
    }
    it.second->SetUnused();
  }
  for (auto& it : rem) {
    static_text.erase(it);
  }
  while (!clip_rects.empty()) {
    clip_rects.pop();
  }
}
}  // namespace UI7
}  // namespace PD