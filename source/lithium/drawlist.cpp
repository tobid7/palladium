#include <pd/lithium/drawlist.hpp>
#include <pd/lithium/renderer.hpp>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace PD {
namespace LI {
PD_LITHIUM_API Command::Ref DrawList::PreGenerateCmd() {
  Command::Ref cmd = Command::New();
  cmd->Layer = Layer;
  cmd->Index = pDrawList.Size();
  cmd->Tex = CurrentTex;
  return cmd;
}

PD_LITHIUM_API void DrawList::PathArcToN(const fvec2& c, float radius,
                                         float a_min, float a_max,
                                         int segments) {
  // Path.push_back(c);
  PathReserve(segments + 1);
  for (int i = 0; i < segments; i++) {
    float a = a_min + ((float)i / (float)segments) * (a_max - a_min);
    PathAdd(vec2(c.x + std::cos(a) * radius, c.y + std::sin(a) * radius));
  }
}

PD_LITHIUM_API void DrawList::PathRect(fvec2 a, fvec2 b, float rounding,
                                       u32 flags) {
  if (rounding == 0.f) {
    PathAdd(a);
    PathAdd(vec2(b.x, a.y));
    PathAdd(b);
    PathAdd(vec2(a.x, b.y));
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

PD_LITHIUM_API void DrawList::DrawRect(const fvec2& pos, const fvec2& size,
                                       u32 color, int thickness) {
  PathRect(pos, pos + size);
  // Flags is currently hardcoded (1 = close)
  PathStroke(color, thickness, 1);
}
void DrawList::DrawRectFilled(const fvec2& pos, const fvec2& size, u32 color) {
  PathRect(pos, pos + size);
  PathFill(color);
}

PD_LITHIUM_API void DrawList::DrawTriangle(const fvec2& a, const fvec2& b,
                                           const fvec2& c, u32 color,
                                           int thickness) {
  PathAdd(a);
  PathAdd(b);
  PathAdd(c);
  PathStroke(color, thickness, 1);
}

PD_LITHIUM_API void DrawList::DrawTriangleFilled(const fvec2& a, const fvec2& b,
                                                 const fvec2& c, u32 color) {
  PathAdd(a);
  PathAdd(b);
  PathAdd(c);
  PathFill(color);
}

PD_LITHIUM_API void DrawList::DrawCircle(const fvec2& center, float rad,
                                         u32 color, int num_segments,
                                         int thickness) {
  if (num_segments <= 0) {
    // Auto Segment
  } else {
    float am = (M_PI * 2.0f) * ((float)num_segments) / (float)num_segments;
    PathArcToN(center, rad, 0.f, am, num_segments);
  }
  DrawSolid();  // Only Solid Color Supported
  PathStroke(color, thickness, (1 << 0));
}

PD_LITHIUM_API void DrawList::DrawCircleFilled(const fvec2& center, float rad,
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
PD_LITHIUM_API void DrawList::DrawPolyLine(const Vec<fvec2>& points, u32 clr,
                                           u32 flags, int thickness) {
  if (points.Size() < 2) {
    return;
  }
  CurrentTex = WhitePixel;
  auto cmd = PreGenerateCmd();
  bool close = (flags & (1 << 0));
  int num_points = close ? (int)points.Size() : (int)points.Size() - 1;
  if (flags & (1 << 1)) {
    // TODO: Find a way to draw less garbage looking lines
  } else {
    // Non antialiased lines look awful when rendering with thickness != 1
    for (int i = 0; i < num_points; i++) {
      int j = (i + 1) == (int)points.Size() ? 0 : (i + 1);
      auto line = Renderer::PrimLine(points[i], points[j], thickness);
      Renderer::CmdQuad(cmd, line, vec4(0.f, 1.f, 1.f, 0.f), clr);
    }
  }
  AddCommand(cmd);
}

PD_LITHIUM_API void DrawList::DrawConvexPolyFilled(const Vec<fvec2>& points,
                                                   u32 clr) {
  if (points.Size() < 3) {
    return;  // Need at least three points
  }
  auto cmd = PreGenerateCmd();
  Renderer::CmdConvexPolyFilled(cmd, points, clr, CurrentTex);
  AddCommand(cmd);
}

PD_LITHIUM_API void DrawList::DrawText(const fvec2& pos,
                                       const std::string& text, u32 color) {
  if (!pCurrentFont) {
    return;
  }
  PD::Vec<Command::Ref> cmds;
  pCurrentFont->CmdTextEx(cmds, pos, color, pFontScale, text);
  for (size_t i = 0; i < cmds.Size(); i++) {
    cmds[i]->Index = pDrawList.Size();
    cmds[i]->Layer = Layer;
    AddCommand(cmds[i]);
  }
}
}  // namespace LI
}  // namespace PD