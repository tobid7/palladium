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

#include <pd/core/core.hpp>
#include <pd/ui7/drawlist.hpp>
#include <pd/ui7/io.hpp>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace PD {
namespace UI7 {
PD_UI7_API void DrawList::PathArcToN(const fvec2& c, float radius, float a_min,
                                     float a_max, int segments) {
  // Path.push_back(c);
  PathReserve(segments + 1);
  for (int i = 0; i < segments; i++) {
    float a = a_min + ((float)i / (float)segments) * (a_max - a_min);
    PathNext(vec2(c.x + std::cos(a) * radius, c.y + std::sin(a) * radius));
  }
}

PD_UI7_API void DrawList::PathRect(fvec2 a, fvec2 b, float rounding,
                                   UI7DrawFlags flags) {
  if (rounding == 0.f) {
    PathNext(a);
    PathNext(vec2(b.x, a.y));
    PathNext(b);
    PathNext(vec2(a.x, b.y));
  } else {
    PathArcToN(vec2(a.x + rounding, a.y + rounding), rounding, 4 * 6, 4 * 9,
               21);
    PathArcToN(vec2(b.x - rounding, a.y + rounding), rounding, 4 * 9, 4 * 12,
               21);
    PathArcToN(vec2(b.x - rounding, b.y - rounding), rounding, 4 * 0, 4 * 3,
               21);
    PathArcToN(vec2(a.x + rounding, b.y - rounding), rounding, 4 * 3, 4 * 6,
               21);
  }
}

PD_UI7_API void DrawList::AddRect(const fvec2& pos, const fvec2& size,
                                  const UI7Color& clr, int thickness) {
  PathRect(pos, pos + size);
  PathStroke(clr, thickness, UI7DrawFlags_Close);
}
PD_UI7_API void DrawList::AddRectangle(fvec2 pos, fvec2 szs,
                                       const UI7Color& clr) {
  PathRect(pos, pos + szs);
  PathFill(clr);
}

PD_UI7_API void DrawList::AddTriangle(const fvec2& a, const fvec2& b,
                                      const fvec2& c, const UI7Color& clr,
                                      int thickness) {
  PathNext(a);
  PathNext(b);
  PathNext(c);
  PathStroke(clr, thickness, UI7DrawFlags_Close);
}

PD_UI7_API void DrawList::AddTriangleFilled(const fvec2& a, const fvec2& b,
                                            const fvec2& c,
                                            const UI7Color& clr) {
  PathNext(a);
  PathNext(b);
  PathNext(c);
  PathFill(clr);
}

PD_UI7_API void DrawList::AddCircle(const fvec2& pos, float rad, UI7Color col,
                                    int num_segments, int thickness) {
  if (num_segments <= 0) {
    // Auto Segment
  } else {
    float am = (M_PI * 2.0f) * ((float)num_segments) / (float)num_segments;
    PathArcToN(pos, rad, 0.f, am, num_segments);
  }
  PathStroke(col, thickness, UI7DrawFlags_Close);
}

PD_UI7_API void DrawList::AddCircleFilled(const fvec2& pos, float rad,
                                          UI7Color col, int num_segments) {
  if (num_segments <= 0) {
    // Auto Segment
  } else {
    float am = (M_PI * 2.0f) * ((float)num_segments) / (float)num_segments;
    PathArcToN(pos, rad, 0.f, am, num_segments);
  }
  PathFill(col);
}

PD_UI7_API void DrawList::AddText(fvec2 pos, const std::string& text,
                                  const UI7Color& clr, u32 flags, fvec2 box) {
  Vec<LI::Command::Ref> cmds;
  pIO->Font->CmdTextEx(cmds, pos, clr, pIO->FontScale, text, flags, box);
  for (size_t i = 0; i < cmds.Size(); i++) {
    ClipCmd(cmds[i]);
    cmds[i]->Layer = Layer;
    cmds[i]->Index = Commands.Size();
    Commands.Add(cmds[i]);
  }
  // if (!IO->Ren->Font()) {
  //   return;
  // }
  // u32 id = Strings::FastHash(text);
  // LI::StaticText::Ref e;
  // auto f = static_text.find(id);
  // if (static_text.find(id) == static_text.end()) {
  //   e = LI::StaticText::New();
  //   static_text[id] = e;
  // } else {
  //   e = f->second;
  // }
  // if (!e->IsSetup() || e->Font() != IO->Ren->Font()) {
  //   int l = IO->Ren->Layer();
  //   IO->Ren->Layer(layer);
  //   e->Setup(ren.get(), pos, clr, text, flags, box);
  //   e->Font(IO->Ren->Font());
  //   IO->Ren->Layer(l);
  // }
  // e->SetPos(pos);
  // e->SetColor(clr);
  // e->SetLayer(layer);
  // if (!clip_rects.empty()) {
  //   e->SetScissorMode(LI::ScissorMode_Normal);
  //   e->ScissorRect(clip_rects.top());
  // }
  // for (auto it : e->GetRawObject()->List()) {
  //   this->commands.push_back(std::make_pair(
  //       IO->Ren->CurrentScreen()->ScreenType() == Screen::Bottom, it));
  // }
  // e->GetRawObject()->ReCopy();
}

PD_UI7_API void DrawList::AddImage(fvec2 pos, LI::Texture::Ref img, fvec2 size,
                                   LI::Rect uv) {
  size = size == 0.f ? fvec2(img->GetSize().x, img->GetSize().y) : size;
  uv = (uv.Top() == 0.0f && uv.Bot() == 0.0f) ? img->GetUV() : uv;
  LI::Command::Ref cmd = LI::Command::New();
  cmd->Layer = Layer;
  cmd->Index = Commands.Size();
  cmd->Tex = img;
  auto r = LI::Renderer::PrimRect(pos, size);
  LI::Renderer::CmdQuad(cmd, r, uv, 0xffffffff);
  // auto rect = IO->Ren->CreateRect(pos, size, 0.f);
  // auto cmd = LI::Command::New();
  // IO->Ren->UseTex(img);
  // IO->Ren->SetupCommand(cmd);
  // cmd->Layer(layer);
  // if (!clip_rects.empty()) {
  //   cmd->SetScissorMode(LI::ScissorMode_Normal);
  //   cmd->ScissorRect(clip_rects.top());
  // }
  // IO->Ren->QuadCommand(cmd, rect, uv, 0xffffffff);
  // commands.push_back(std::make_pair(
  //     IO->Ren->CurrentScreen()->ScreenType() == Screen::Bottom, cmd));
}

PD_UI7_API void DrawList::AddLine(const fvec2& a, const fvec2& b,
                                  const UI7Color& clr, int t) {
  PathNext(a);
  PathNext(b);
  PathStroke(clr, t);
}

// TODO: Don't render OOS
PD_UI7_API void DrawList::AddPolyLine(const Vec<fvec2>& points,
                                      const UI7Color& clr, UI7DrawFlags flags,
                                      int thickness) {
  if (points.Size() < 2) {
    return;
  }
  auto cmd = LI::Command::New();
  cmd->Index = Commands.Size();
  cmd->Layer = Layer;
  cmd->Tex = pIO->Ren->WhitePixel;
  ClipCmd(cmd);
  bool close = (flags & UI7DrawFlags_Close);
  int num_points = close ? (int)points.Size() : (int)points.Size() - 1;
  if (flags & UI7DrawFlags_AALines) {
    // TODO: Find a way to draw less garbage looking lines
  } else {
    // Non antialiased lines look awful when rendering with thickness != 1
    for (int i = 0; i < num_points; i++) {
      int j = (i + 1) == (int)points.Size() ? 0 : (i + 1);
      auto line = LI::Renderer::PrimLine(points[i], points[j], thickness);
      LI::Renderer::CmdQuad(cmd, line, vec4(0.f, 1.f, 1.f, 0.f), clr);
    }
  }
  Commands.Add(cmd);
}

PD_UI7_API void DrawList::ClipCmd(LI::Command::Ref cmd) {
  if (!pClipRects.IsEmpty()) {
    cmd->ScissorEnabled = true;
    fvec4 sr = pClipRects.Top();
    cmd->ScissorRect = ivec4(sr.x, sr.y, sr.z, sr.w);
  }
}

// TODO: Don't render OOS (Probably make it with a define as it
// would probably be faster to render out of screen than checking if
// it could be skipped)
PD_UI7_API void DrawList::AddConvexPolyFilled(const Vec<fvec2>& points,
                                              const UI7Color& clr) {
  if (points.Size() < 3) {
    return;  // Need at least three points
  }
  auto cmd = LI::Command::New();
  cmd->Index = Commands.Size();
  cmd->Layer = Layer;
  auto tex = CurrentTex;
  if (!tex) {
    tex = pIO->Ren->WhitePixel;
  }
  cmd->Tex = tex;
  ClipCmd(cmd);
  for (int i = 2; i < (int)points.Size(); i++) {
    cmd->AppendIndex(0).AppendIndex(i).AppendIndex(i - 1);
  }
  for (int i = 0; i < (int)points.Size(); i++) {
    cmd->AppendVertex(LI::Vertex(points[i], fvec2(0, 0), clr));
  }
  Commands.Add(cmd);
}

PD_UI7_API void DrawList::Clear() { Commands.Clear(); }

/** Process [Render] the Drawlist */
PD_UI7_API void DrawList::Process(LI::DrawList::Ref d) {
  std::sort(Commands.Begin(), Commands.End(),
            [](LI::Command::Ref a, LI::Command::Ref b) {
              /** Advanced (for saving Drawcalls)
               * - Probably could handle this by creating diffrent layers
               *   for texts and solid objectives
               * if(a->Tex == b->Tex) { return a->Layer < b->Layer; }
               * return a->Tex < b->Tex;
               */
              /** Simple */
              return a->Layer < b->Layer;
            });
  NumVertices = 0;
  NumIndices = 0;
  for (auto command = Commands.Begin(); command != Commands.End(); command++) {
    // IO->Ren->OnScreen(IO->Ren->GetScreen(command.first));
    (*command)->Layer = (*command)->Layer + Base;
    d->AddCommand(*command);
    NumVertices += (*command)->VertexBuffer.Size();
    NumIndices += (*command)->IndexBuffer.Size();
  }
  Commands.Clear();
  Layer = 0;
  std::vector<u32> rem;
  // for (auto it : static_text) {
  //   if (!it.second->Used()) {
  //     rem.push_back(it.first);
  //   }
  //   it.second->SetUnused();
  // }
  // for (auto& it : rem) {
  //   static_text.erase(it);
  // }
  pClipRects.Clear();
}
}  // namespace UI7
}  // namespace PD