/*
MIT License
Copyright (c) 2024 - 2026 René Amthor (tobid7)

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

#include <pd/lithium/drawlist.hpp>
#include <pd/lithium/renderer.hpp>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace PD {
namespace Li {
PD_API DrawList::DrawList(Context& ctx, int initial_size) : pCtx(&ctx) {
  DrawSolid();
  pPool.Init(initial_size);
}

PD_API DrawList::~DrawList() {
  Clear();
  pPool.Deinit();
}

PD_API void DrawList::DrawSolid() { CurrentTex = pCtx->GetSolidTex(); }

PD_API void DrawList::Clear() {
  pNumIndices = 0;
  pNumVertices = 0;
  pPool.Reset();
  pPath.clear();
  if (pCurrentFont) {
    pCurrentFont->CleanupTMS();
  }
  while (!pClipRects.empty()) {
    pClipRects.pop();
  }
}

PD_API void DrawList::Merge(DrawList::Ref list) {
  pPool.Merge(list->pPool);
  /*for (size_t i = 0; i < list->pDrawList.size(); i++) {
    pNumIndices += list->pDrawList[i]->IndexBuffer.size();
    pNumVertices += list->pDrawList[i]->VertexBuffer.size();
    auto cmd = pPool.NewCmd();
    pDrawList.push_back(list->pDrawList[i]);
  }*/
  /** Make sure The list gets cleared */
  list->Clear();
}

PD_API void DrawList::Copy(DrawList::Ref list) { pPool.Copy(list->pPool); }

PD_API void DrawList::Optimize() {
  /*std::sort(pDrawList.begin(), pDrawList.end(),
            [](const PD::Li::Command::Ref &a, const PD::Li::Command::Ref &b) {
              if (a->Layer == b->Layer) {  // Same layer
                if (a->Tex == b->Tex) {    // same tex
                  return a->Index < b->Index;
                }
                return a->Tex < b->Tex;  // order by address
              }
              return a->Layer < b->Layer;  // Order by layer
            });*/
}

PD_API Command::Ref DrawList::GetNewCmd() {
  Command::Ref cmd = pPool.NewCmd();
  cmd->Index = pPool.Size() - 1;
  cmd->Tex = CurrentTex->Address;
  pClipCmd(cmd);
  return cmd;
}

PD_API void DrawList::pClipCmd(Command::Ref cmd) {
  if (!pClipRects.empty()) {
    cmd->ScissorOn = true;
    cmd->ScissorRect = ivec4(pClipRects.top());
  }
}

PD_API void DrawList::PathArcToN(const fvec2& c, float radius, float a_min,
                                 float a_max, int segments) {
  // Path.push_back(c);
  PathReserve(segments + 1);
  for (int i = 0; i < segments; i++) {
    float a = a_min + ((float)i / (float)segments) * (a_max - a_min);
    PathAdd(vec2(c.x + std::cos(a) * radius, c.y + std::sin(a) * radius));
  }
}

PD_API void DrawList::PathFastArcToN(const fvec2& c, float r, float amin,
                                     float amax, int s) {
  /**
   * Funcion with less division overhead
   * Usefull for stuff where a lot of calculations are required
   */
  float d = (amax - amin) / s;
  PathReserve(s + 1);
  for (int i = 0; i <= s; i++) {
    float a = amin + i * d;
    PathAdd(fvec2(c.x + std::cos(a) * r, c.y + std::sin(a) * r));
  }
}

PD_API void DrawList::PathRect(fvec2 a, fvec2 b, float rounding) {
  if (rounding == 0.f) {
    PathAdd(a);
    PathAdd(vec2(b.x, a.y));
    PathAdd(b);
    PathAdd(vec2(a.x, b.y));
  } else {
    float r = std::min({rounding, (b.x - a.x) * 0.5f, (b.y - a.y) * 0.5f});
    /** Calculate Optimal segment count automatically */
    float corner = M_PI * 0.5f;
    int segments = std::max(3, int(std::ceil(corner / (6.0f * M_PI / 180.0f))));

    /**
     * To Correctly render filled shapes with Paths API
     * The Commands need to be setup clockwise
     */
    /** Top Left */
    PathAdd(vec2(a.x + r, a.y));
    PathFastArcToN(vec2(b.x - r, a.y + r), r, -M_PI / 2.0f, 0.0f, segments);
    /** Top Right */
    PathAdd(vec2(b.x, b.y - r));
    PathFastArcToN(vec2(b.x - r, b.y - r), r, 0.0f, M_PI / 2.0f, segments);
    /** Bottom Right */
    PathAdd(vec2(a.x + r, b.y));
    PathFastArcToN(vec2(a.x + r, b.y - r), r, M_PI / 2.0f, M_PI, segments);
    /** Bottom Left */
    PathAdd(vec2(a.x, a.y + r));
    PathFastArcToN(vec2(a.x + r, a.y + r), r, M_PI, 3.0f * M_PI / 2.0f,
                   segments);
  }
}

PD_API void DrawList::PathRectEx(fvec2 a, fvec2 b, float rounding, u32 flags) {
  if (rounding == 0.f) {
    PathAdd(a);
    PathAdd(vec2(b.x, a.y));
    PathAdd(b);
    PathAdd(vec2(a.x, b.y));
  } else {
    float r = std::min({rounding, (b.x - a.x) * 0.5f, (b.y - a.y) * 0.5f});
    /** Calculate Optimal segment count automatically */
    float corner = M_PI * 0.5f;
    int segments = std::max(3, int(std::ceil(corner / (6.0f * M_PI / 180.0f))));

    /**
     * To Correctly render filled shapes with Paths API
     * The Commands need to be setup clockwise
     */
    /** Top Left */
    if (flags & LiPathRectFlags_KeepTopLeft) {
      PathAdd(a);
    } else {
      PathAdd(vec2(a.x + r, a.y));
      PathFastArcToN(vec2(b.x - r, a.y + r), r, -M_PI / 2.0f, 0.0f, segments);
    }

    /** Top Right */
    if (flags & LiPathRectFlags_KeepTopRight) {
      PathAdd(vec2(b.x, a.y));
    } else {
      PathAdd(vec2(b.x, b.y - r));
      PathFastArcToN(vec2(b.x - r, b.y - r), r, 0.0f, M_PI / 2.0f, segments);
    }
    /** Bottom Right */
    if (flags & LiPathRectFlags_KeepBotRight) {
      PathAdd(b);
    } else {
      PathAdd(vec2(a.x + r, b.y));
      PathFastArcToN(vec2(a.x + r, b.y - r), r, M_PI / 2.0f, M_PI, segments);
    }
    /** Bottom Left */
    if (flags & LiPathRectFlags_KeepBotLeft) {
      PathAdd(vec2(a.x, b.y));
    } else {
      PathAdd(vec2(a.x, a.y + r));
      PathFastArcToN(vec2(a.x + r, a.y + r), r, M_PI, 3.0f * M_PI / 2.0f,
                     segments);
    }
  }
}

PD_API void DrawList::DrawRect(const fvec2& pos, const fvec2& size, u32 color,
                               int thickness) {
  PathRect(pos, pos + size);
  // Flags is currently hardcoded (1 = close)
  PathStroke(color, thickness, 1);
}
void DrawList::DrawRectFilled(const fvec2& pos, const fvec2& size, u32 color) {
  PathRect(pos, pos + size);
  PathFill(color);
}

PD_API void DrawList::DrawTriangle(const fvec2& a, const fvec2& b,
                                   const fvec2& c, u32 color, int thickness) {
  PathAdd(a);
  PathAdd(b);
  PathAdd(c);
  PathStroke(color, thickness, 1);
}

PD_API void DrawList::DrawTriangleFilled(const fvec2& a, const fvec2& b,
                                         const fvec2& c, u32 color) {
  PathAdd(a);
  PathAdd(b);
  PathAdd(c);
  PathFill(color);
}

PD_API void DrawList::DrawCircle(const fvec2& center, float rad, u32 color,
                                 int num_segments, int thickness) {
  if (num_segments <= 0) {
    // Auto Segment
  } else {
    float am = (M_PI * 2.0f) * ((float)num_segments) / (float)num_segments;
    PathArcToN(center, rad, 0.f, am, num_segments);
  }
  DrawSolid();  // Only Solid Color Supported
  PathStroke(color, thickness, (1 << 0));
}

PD_API void DrawList::DrawCircleFilled(const fvec2& center, float rad,
                                       u32 color, int num_segments) {
  if (num_segments <= 0) {
    // Auto Segment
  } else {
    float am = (M_PI * 2.0f) * ((float)num_segments) / (float)num_segments;
    PathArcToN(center, rad, 0.f, am, num_segments);
  }
  PathFill(color);
}

// TODO: Don't render OOS
PD_API void DrawList::DrawPolyLine(const std::vector<fvec2>& points, u32 clr,
                                   u32 flags, int thickness) {
  if (points.size() < 2) {
    return;
  }
  DrawSolid();
  auto cmd = GetNewCmd();
  bool close = (flags & (1 << 0));
  int num_points = close ? (int)points.size() : (int)points.size() - 1;
  if (flags & (1 << 1)) {
    // TODO: Find a way to draw less garbage looking lines
  } else {
    // Non antialiased lines look awful when rendering with thickness != 1
    for (int i = 0; i < num_points; i++) {
      int j = (i + 1) == (int)points.size() ? 0 : (i + 1);
      auto line = Renderer::PrimLine(points[i], points[j], thickness);
      Renderer::CmdQuad(cmd, line, vec4(0.f, 1.f, 1.f, 0.f), clr);
    }
  }
}

PD_API void DrawList::DrawConvexPolyFilled(const std::vector<fvec2>& points,
                                           u32 clr) {
  if (points.size() < 3) {
    return;  // Need at least three points
  }
  auto cmd = GetNewCmd();
  Renderer::CmdConvexPolyFilled(cmd, points, clr, CurrentTex);
}

PD_API void DrawList::DrawText(const fvec2& pos, const std::string& text,
                               u32 color) {
  if (!pCurrentFont) {
    return;
  }
  pCurrentFont->CmdTextEx(*this, pos, color, pFontScale, text);
}

PD_API void DrawList::DrawTextEx(const fvec2& p, const std::string& text,
                                 u32 color, LiTextFlags flags,
                                 const fvec2& box) {
  if (!pCurrentFont) {
    return;
  }
  pCurrentFont->CmdTextEx(*this, p, color, pFontScale, text, flags, box);
}

PD_API void DrawList::DrawLine(const fvec2& a, const fvec2& b, u32 color,
                               int t) {
  PathAdd(a);
  PathAdd(b);
  PathStroke(color, t);
}
}  // namespace Li
}  // namespace PD
