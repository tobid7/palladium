#include <iostream>
#include <pd/drivers/gfx.hpp>
#include <pd/lithium/drawlist.hpp>
#include <pd/lithium/formatters.hpp>
#include <pd/lithium/math.hpp>

namespace PD {
namespace Li {
PD_API Drawlist::Drawlist() { Clear(); }

PD_API Drawlist::~Drawlist() { Clear(); }

PD_API void Drawlist::Merge(Drawlist& other) {
  pCommands.AppendMove(other.pCommands);
  other.Clear();
}

PD_API void Drawlist::Copy(Drawlist& other) {
  pCommands.AppendCopy(other.pCommands);
}

PD_API void Drawlist::Optimize() {}

PD_API void Drawlist::Clear() {
  UnbindTexture();
  pPath.ResetFast();
  pVertices.ResetFast();
  pIndices.ResetFast();
  pCommands.ResetFast();
}

/** Command Allocation */
PD_API Command& Drawlist::NewCommand() {
  auto cmd = pCommands.Allocate(1);
  cmd->Tex = pCurrentTexture.GetID();
  return *cmd;
}

PD_API void Drawlist::BindTexture(const Texture& tex) { pCurrentTexture = tex; }

/** Path API */
PD_API void Drawlist::PathStroke(const PD::Color& color, int t,
                                 LiDrawFlags flags) {
  DrawPolyLine(pPath, color, flags, t);
  PathClear();
}

PD_API void Drawlist::PathFill(const PD::Color& color) {
  DrawConvexPolyFilled(pPath, color);
  PathClear();
}

PD_API void Drawlist::PathFillGradient(const PD::Color& a, const PD::Color& b,
                                       float rad) {
  DrawConvexPolyFilled(pPath, a, b, rad);
  PathClear();
}

PD_API void Drawlist::PathArcToN(const fvec2& c, float r, float amin,
                                 float amax, int s) {
  // Path.push_back(c);
  PathReserve(s + 1);
  for (int i = 0; i < s; i++) {
    float a = amin + ((float)i / (float)s) * (amax - amin);
    PathAdd(vec2(c.x + std::cos(a) * r, c.y + std::sin(a) * r));
  }
}

PD_API void Drawlist::PathFastArcToN(const fvec2& c, float r, float amin,
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

PD_API void Drawlist::PathRect(const fvec2& tl, const fvec2& br,
                               float rounding) {
  if (rounding == 0.f) {
    PathAdd(tl);
    PathAdd(vec2(br.x, tl.y));
    PathAdd(br);
    PathAdd(vec2(tl.x, br.y));
  } else {
    float r = std::min({rounding, (br.x - tl.x) * 0.5f, (br.y - tl.y) * 0.5f});
    /** Calculate Optimal segment count automatically */
    float corner = M_PI * 0.5f;
    int segments = std::max(3, int(std::ceil(corner / (6.0f * M_PI / 180.0f))));

    /**
     * To Correctly render filled shapes with Paths API
     * The Commands need to be setup clockwise
     */
    /** Top Left */
    PathAdd(vec2(tl.x + r, tl.y));
    PathFastArcToN(vec2(br.x - r, tl.y + r), r, -M_PI / 2.0f, 0.0f, segments);
    /** Top Right */
    PathAdd(vec2(br.x, br.y - r));
    PathFastArcToN(vec2(br.x - r, br.y - r), r, 0.0f, M_PI / 2.0f, segments);
    /** Bottom Right */
    PathAdd(vec2(tl.x + r, br.y));
    PathFastArcToN(vec2(tl.x + r, br.y - r), r, M_PI / 2.0f, M_PI, segments);
    /** Bottom Left */
    PathAdd(vec2(tl.x, tl.y + r));
    PathFastArcToN(vec2(tl.x + r, tl.y + r), r, M_PI, 3.0f * M_PI / 2.0f,
                   segments);
  }
}

PD_API void Drawlist::PathRectEx(const fvec2& tl, const fvec2& br,
                                 float rounding, LiPathRectFlags flags) {
  if (rounding == 0.f) {
    PathAdd(tl);
    PathAdd(vec2(br.x, tl.y));
    PathAdd(br);
    PathAdd(vec2(tl.x, br.y));
  } else {
    float r = std::min({rounding, (br.x - tl.x) * 0.5f, (br.y - tl.y) * 0.5f});
    /** Calculate Optimal segment count automatically */
    float corner = M_PI * 0.5f;
    int segments = std::max(3, int(std::ceil(corner / (6.0f * M_PI / 180.0f))));

    /**
     * To Correctly render filled shapes with Paths API
     * The Commands need to be setup clockwise
     */
    /** Top Left */
    if (flags & LiPathRectFlags_KeepTopLeft) {
      PathAdd(tl);
    } else {
      PathAdd(vec2(tl.x + r, tl.y));
      PathFastArcToN(vec2(br.x - r, tl.y + r), r, -M_PI / 2.0f, 0.0f, segments);
    }

    /** Top Right */
    if (flags & LiPathRectFlags_KeepTopRight) {
      PathAdd(vec2(br.x, tl.y));
    } else {
      PathAdd(vec2(br.x, br.y - r));
      PathFastArcToN(vec2(br.x - r, br.y - r), r, 0.0f, M_PI / 2.0f, segments);
    }
    /** Bottom Right */
    if (flags & LiPathRectFlags_KeepBotRight) {
      PathAdd(br);
    } else {
      PathAdd(vec2(tl.x + r, br.y));
      PathFastArcToN(vec2(tl.x + r, br.y - r), r, M_PI / 2.0f, M_PI, segments);
    }
    /** Bottom Left */
    if (flags & LiPathRectFlags_KeepBotLeft) {
      PathAdd(vec2(tl.x, br.y));
    } else {
      PathAdd(vec2(tl.x, tl.y + r));
      PathFastArcToN(vec2(tl.x + r, tl.y + r), r, M_PI, 3.0f * M_PI / 2.0f,
                     segments);
    }
  }
}

/** Drawing functions */
PD_API void Drawlist::DrawRect(const fvec2& pos, const fvec2& size,
                               const PD::Color& color, int t) {
  PathRect(pos, pos + size);
  PathStroke(color, t, LiDrawFlags_Close);
}

PD_API void Drawlist::DrawRectFilled(const fvec2& pos, const fvec2& size,
                                     const PD::Color& color) {
  PathRect(pos, pos + size);
  PathFill(color);
}

PD_API void Drawlist::DrawTriangle(const fvec2& a, const fvec2& b,
                                   const fvec2& c, const PD::Color& color,
                                   int t) {
  PathAdd(a);
  PathAdd(b);
  PathAdd(c);
  PathStroke(color, t, LiDrawFlags_Close);
}

PD_API void Drawlist::DrawTriangleFilled(const fvec2& a, const fvec2& b,
                                         const fvec2& c,
                                         const PD::Color& color) {
  PathAdd(a);
  PathAdd(b);
  PathAdd(c);
  PathFill(color);
}

PD_API void Drawlist::DrawCircle(const fvec2& center, float rad,
                                 const PD::Color& color, int num_segments,
                                 int t) {
  if (num_segments <= 0) {
    // Auto Segment
  } else {
    float am = (M_PI * 2.0f) * ((float)num_segments) / (float)num_segments;
    PathArcToN(center, rad, 0.f, am, num_segments);
  }
  UnbindTexture();  // Only Solid Color Supported
  PathStroke(color, t, LiDrawFlags_Close);
}

PD_API void Drawlist::DrawCircleFilled(const fvec2& center, float rad,
                                       const PD::Color& color,
                                       int num_segments) {
  if (num_segments <= 0) {
    // Auto Segment
  } else {
    float am = (M_PI * 2.0f) * ((float)num_segments) / (float)num_segments;
    PathArcToN(center, rad, 0.f, am, num_segments);
  }
  PathFill(color);
}

PD_API void Drawlist::DrawText(const fvec2& p, const char* text,
                               const PD::Color& color) {
  if (!pFont) return;
  pFont->CmdTextEx(*this, p, color, pFontScale, text);
}

PD_API void Drawlist::DrawTextEx(const fvec2& p, const char* text,
                                 const PD::Color& color, LiTextFlags flags,
                                 const fvec2& box) {
  if (!pFont) return;
  pFont->CmdTextEx(*this, p, color, pFontScale, text, flags, box);
}

PD_API void Drawlist::DrawPolyLine(const Pool<fvec2>& points,
                                   const PD::Color& color, LiDrawFlags flags,
                                   int t) {
  if (points.size() < 2) {
    return;
  }
  UnbindTexture();
  auto& cmd = NewCommand();
  bool close = (flags & LiDrawFlags_Close);
  int num_points = close ? (int)points.size() : (int)points.size() - 1;
  if (flags & LiDrawFlags_AA) {
    // TODO: Find a way to draw less garbage looking lines
  } else {
    // Non antialiased lines look awful when rendering with thickness != 1
    for (int i = 0; i < num_points; i++) {
      int j = (i + 1) == (int)points.size() ? 0 : (i + 1);
      auto line = Math::PrimLine(points[i], points[j], t);
      this->PrimQuad(cmd, line, vec4(0.f, 1.f, 1.f, 0.f), color);
    }
  }
}

PD_API void Drawlist::DrawConvexPolyFilled(const Pool<fvec2>& points,
                                           const PD::Color& color) {
  if (points.size() < 3) {
    return;  // Need at least three points
  }

  // Support for Custom Textures (UV calculation)
  float minX = points[0].x, minY = points[0].y;
  float maxX = minX, maxY = minY;
  // Check for the max and min Positions
  for (const auto& it : points) {
    if (it.x < minX) minX = it.x;
    if (it.y < minY) minY = it.y;
    if (it.x > maxX) maxX = it.x;
    if (it.y > maxY) maxY = it.y;
  }
  // Get Short defines for UV
  // (Bottom Right is not required)
  auto uv_tl = pCurrentTexture.GetUV().TopLeft();
  auto uv_tr = pCurrentTexture.GetUV().TopRight();
  auto uv_bl = pCurrentTexture.GetUV().BotLeft();

  auto& cmd = NewCommand();
  cmd.Reserve(points.size(), (points.size() - 2) * 3);
  // Render
  for (int i = 2; i < (int)points.size(); i++) {
    cmd.Add(0, i, i - 1);
  }

  for (int i = 0; i < (int)points.size(); i++) {
    // Calculate U and V coords
    float u =
        uv_tl.x + ((points[i].x - minX) / (maxX - minX)) * (uv_tr.x - uv_tl.x);
    float v =
        uv_tl.y + ((points[i].y - minY) / (maxY - minY)) * (uv_bl.y - uv_tl.y);
    cmd.Add(Vertex(points[i], fvec2(u, v), color));
  }
}

PD_API void Drawlist::DrawConvexPolyFilled(const Pool<fvec2>& points,
                                           const PD::Color& a,
                                           const PD::Color b, float rad) {
  if (points.size() < 3) {
    return;  // Need at least three points
  }

  fvec2 dir = fvec2(std::cos(rad), std::sin(rad));
  // Support for Custom Textures (UV calculation)
  float minX = points[0].x, minY = points[0].y;
  float maxX = minX, maxY = minY;
  // Check for the max and min Positions
  for (const auto& it : points) {
    if (it.x < minX) minX = it.x;
    if (it.y < minY) minY = it.y;
    if (it.x > maxX) maxX = it.x;
    if (it.y > maxY) maxY = it.y;
  }
  // Get Short defines for UV
  // (Bottom Right is not required)
  auto uv_tl = pCurrentTexture.GetUV().TopLeft();
  auto uv_tr = pCurrentTexture.GetUV().TopRight();
  auto uv_bl = pCurrentTexture.GetUV().BotLeft();

  // Gradient
  float tmin = std::numeric_limits<float>::max();
  float tmax = std::numeric_limits<float>::lowest();
  for (const auto& p : points) {
    float t = p.x * dir.x + p.y * dir.y;
    if (t < tmin) tmin = t;
    if (t > tmax) tmax = t;
  }
  // potential div0
  float irange = (tmax != tmin) ? (1.0f / (tmax - tmin)) : 0.0f;
  // Command building (oder so)
  auto& cmd = NewCommand();
  cmd.Reserve(points.size(), (points.size() - 2) * 3);
  // Render
  for (int i = 2; i < (int)points.size(); i++) {
    cmd.Add(0, i, i - 1);
  }
  // Why was this for loop not used in normal Convex Poly filled???
  for (auto& it : points) {
    // Calculate U and V coords
    float u = uv_tl.x + ((it.x - minX) / (maxX - minX)) * (uv_tr.x - uv_tl.x);
    float v = uv_tl.y + ((it.y - minY) / (maxY - minY)) * (uv_bl.y - uv_tl.y);
    float t = it.x * dir.x + it.y * dir.y;
    cmd.Add(Vertex(it, fvec2(u, v), PD::Color(a).Lerp(b, (t - tmin) * irange)));
  }
}

PD_API void Drawlist::PrimQuad(Command& cmd, const Rect& quad, const Rect& uv,
                               const PD::Color& color) {
  cmd.Reserve(4, 6);
  cmd.Add(2, 1, 0);
  cmd.Add(3, 2, 0);
  cmd.Add(Vertex(quad.TopLeft(), uv.TopLeft(), color));
  cmd.Add(Vertex(quad.TopRight(), uv.TopRight(), color));
  cmd.Add(Vertex(quad.BotRight(), uv.BotRight(), color));
  cmd.Add(Vertex(quad.BotLeft(), uv.BotLeft(), color));
}

PD_API void Drawlist::PrimTriangle(Command& cmd, const fvec2& a, const fvec2& b,
                                   const fvec2& c, const PD::Color& color) {
  cmd.Reserve(3, 3);
  cmd.Add(2, 1, 0);
  cmd.Add(Vertex(a, vec2(0.f, 1.f), color));
  cmd.Add(Vertex(b, vec2(1.f, 1.f), color));
  cmd.Add(Vertex(c, vec2(1.f, 0.f), color));
}
}  // namespace Li
}  // namespace PD