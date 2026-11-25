#include <algorithm>
#include <iostream>
#include <pd/drivers/gfx.hpp>
#include <pd/lithium/drawlist.hpp>
#include <pd/lithium/formatters.hpp>
#include <pd/lithium/math.hpp>

namespace PD {
namespace Li {
static fvec2 NormOZ(fvec2 v) {
  float d = v.x * v.x + v.y * v.y;
  if (d > 0.f) v *= 1.f / std::sqrt(d);
  return v;
}

static fvec2 FixNorm(fvec2 v) {
  float d = v.x * v.x + v.y * v.y;
  if (d > 0.000001f) v *= std::min(1.f / d, 100.f);
  return v;
}

PD_API Drawlist::Drawlist() { Clear(); }

PD_API Drawlist::~Drawlist() { Clear(); }

PD_API void Drawlist::Merge(Drawlist& other) {
  size_t start = pCommands.size();
  pCommands.AppendMove(other.pCommands);
  for (size_t i = start; i < pCommands.size(); i++) {
    pCommands[i].Layer += this->pCurrentLayer;
  }
  other.Clear();
}

PD_API void Drawlist::Copy(Drawlist& other) {
  int start = pCommands.size();
  pCommands.AppendCopy(other.pCommands);
  for (size_t i = start; i < pCommands.size(); i++) {
    pCommands[i].Layer += this->pCurrentLayer;
  }
}

PD_API void Drawlist::Optimize() {
  if (pCommands.size() <= 1) return;
  std::stable_sort(pCommands.begin(), pCommands.end(),
                   [](const Command& a, const Command& b) {
                     if (a.Layer != b.Layer) return a.Layer < b.Layer;
                     if (a.SDF != b.SDF) return a.SDF < b.SDF;
                     return a.Tex < b.Tex;
                   });
}

PD_API void Drawlist::Clear() {
  UnbindTexture();
  pPath.ResetFast();
  pCommands.NoReset();
  pCurrentLayer = 0;
  pClipRects.clear();
}

/** Command Allocation */
PD_API Command& Drawlist::NewCommand() {
  auto cmd = pCommands.Allocate(1);
  cmd->Reset();
  cmd->Layer = pCurrentLayer;
  cmd->Tex = pCurrentTexture.GetID();
  if (HasClipRect()) {
    cmd->ClipRectUsed = HasClipRect();
    cmd->ClipRect = pClipRects.back();
  }
  return *cmd;
}

PD_API void Drawlist::BindTexture(const Texture& tex) { pCurrentTexture = tex; }

/** Path API */
PD_API void Drawlist::PathStroke(const PD::Color& color, int t,
                                 LiDrawFlags flags) {
  DrawPolyLine(pPath, color, flags, t);
  PathClear();
}

PD_API void Drawlist::PathFill(const PD::Color& color, LiDrawFlags flags) {
  DrawConvexPolyFilled(pPath, color, flags);
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
                               const PD::Color& color, int t,
                               LiDrawFlags flags) {
  PathRect(pos, pos + size);
  PathStroke(color, t, flags | LiDrawFlags_Close);
}

PD_API void Drawlist::DrawRectFilled(const fvec2& pos, const fvec2& size,
                                     const PD::Color& color,
                                     LiDrawFlags flags) {
  PathRect(pos, pos + size);
  PathFill(color, flags);
}

PD_API void Drawlist::DrawRectFilledMultiColor(
    const fvec2& pos, const fvec2& size, const PD::Color& tl,
    const PD::Color& tr, const PD::Color& br, const PD::Color& bl) {
  UnbindTexture();
  fvec2 uv = pCurrentTexture.GetUV().TopLeft();
  auto& cmd = NewCommand();
  cmd.Reserve(4, 6);
  cmd.Add(2, 1, 0);
  cmd.Add(3, 2, 0);
  cmd.Add(Vertex(pos, uv, tl));
  cmd.Add(Vertex(pos + fvec2(size.x, 0.f), uv, tr));
  cmd.Add(Vertex(pos + size, uv, br));
  cmd.Add(Vertex(pos + fvec2(0.f, size.y), uv, bl));
}

PD_API void Drawlist::DrawTriangle(const fvec2& a, const fvec2& b,
                                   const fvec2& c, const PD::Color& color,
                                   int t, LiDrawFlags flags) {
  PathAdd(a);
  PathAdd(b);
  PathAdd(c);
  PathStroke(color, t, flags | LiDrawFlags_Close);
}

PD_API void Drawlist::DrawTriangleFilled(const fvec2& a, const fvec2& b,
                                         const fvec2& c, const PD::Color& color,
                                         LiDrawFlags flags) {
  PathAdd(a);
  PathAdd(b);
  PathAdd(c);
  PathFill(color, flags);
}

PD_API void Drawlist::DrawCircle(const fvec2& center, float rad,
                                 const PD::Color& color, int num_segments,
                                 int t, LiDrawFlags flags) {
  if (num_segments <= 0) {
    // Auto Segment
  } else {
    float am = (M_PI * 2.0f) * ((float)num_segments) / (float)num_segments;
    PathArcToN(center, rad, 0.f, am, num_segments);
  }
  UnbindTexture();  // Only Solid Color Supported
  PathStroke(color, t, flags | LiDrawFlags_Close);
}

PD_API void Drawlist::DrawCircleFilled(const fvec2& center, float rad,
                                       const PD::Color& color, int num_segments,
                                       LiDrawFlags flags) {
  if (num_segments <= 0) {
    // Auto Segment
  } else {
    float am = (M_PI * 2.0f) * ((float)num_segments) / (float)num_segments;
    PathArcToN(center, rad, 0.f, am, num_segments);
  }
  PathFill(color, flags);
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

PD_API void Drawlist::DrawLine(const fvec2& a, const fvec2& b,
                               const PD::Color& color, int thickness,
                               LiDrawFlags flags) {
  this->PathAdd(a);
  this->PathAdd(b);
  this->PathStroke(color, thickness, flags);
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
    int pc = (int)points.size();
    float aa = 1.f;
    bool th = t > aa;
    PD::Color ct = color;
    ct.a = 0;
    fvec2 uv = pCurrentTexture.GetUV().TopLeft();
    pTmp.resize(pc * (th ? 5 : 3));
    fvec2* n = pTmp.data();
    fvec2* tp = n + pc;
    for (int i = 0; i < num_points; i++) {
      int j = (i + 1) == pc ? 0 : (i + 1);
      fvec2 d = NormOZ(points[j] - points[i]);
      n[i] = fvec2(d.y, -d.x);
    }
    if (!close) n[pc - 1] = n[pc - 2];

    if (!th) {
      if (!close) {
        int l = pc - 1;
        tp[0] = points[0] + n[0] * aa;
        tp[1] = points[0] - n[0] * aa;
        tp[l * 2] = points[l] + n[l] * aa;
        tp[l * 2 + 1] = points[l] - n[l] * aa;
      }
      cmd.Reserve(pc * 3, num_points * 12);
      int a = 0;
      for (int i = 0; i < num_points; i++) {
        int j = (i + 1) == pc ? 0 : (i + 1);
        int b = (i + 1) == pc ? 0 : (a + 3);
        fvec2 dm = FixNorm((n[i] + n[j]) * 0.5f) * aa;
        tp[j * 2] = points[j] + dm;
        tp[j * 2 + 1] = points[j] - dm;
        cmd.Add(b, a, a + 2);
        cmd.Add(a + 2, b + 2, b);
        cmd.Add(b + 1, a + 1, a);
        cmd.Add(a, b, b + 1);
        a = b;
      }
      for (int i = 0; i < pc; i++) {
        cmd.Add(Vertex(points[i], uv, color));
        cmd.Add(Vertex(tp[i * 2], uv, ct));
        cmd.Add(Vertex(tp[i * 2 + 1], uv, ct));
      }
    } else {
      float hi = (t - aa) * 0.5f;
      if (!close) {
        int l = pc - 1;
        tp[0] = points[0] + n[0] * (hi + aa);
        tp[1] = points[0] + n[0] * hi;
        tp[2] = points[0] - n[0] * hi;
        tp[3] = points[0] - n[0] * (hi + aa);
        tp[l * 4] = points[l] + n[l] * (hi + aa);
        tp[l * 4 + 1] = points[l] + n[l] * hi;
        tp[l * 4 + 2] = points[l] - n[l] * hi;
        tp[l * 4 + 3] = points[l] - n[l] * (hi + aa);
      }
      cmd.Reserve(pc * 4, num_points * 18);
      int a = 0;
      for (int i = 0; i < num_points; i++) {
        int j = (i + 1) == pc ? 0 : (i + 1);
        int b = (i + 1) == pc ? 0 : (a + 4);
        fvec2 dm = FixNorm((n[i] + n[j]) * 0.5f);
        fvec2 o = dm * (hi + aa);
        fvec2 m = dm * hi;
        tp[j * 4] = points[j] + o;
        tp[j * 4 + 1] = points[j] + m;
        tp[j * 4 + 2] = points[j] - m;
        tp[j * 4 + 3] = points[j] - o;
        cmd.Add(b + 1, a + 1, a + 2);
        cmd.Add(a + 2, b + 2, b + 1);
        cmd.Add(b + 1, a + 1, a);
        cmd.Add(a, b, b + 1);
        cmd.Add(b + 2, a + 2, a + 3);
        cmd.Add(a + 3, b + 3, b + 2);
        a = b;
      }
      for (int i = 0; i < pc; i++) {
        cmd.Add(Vertex(tp[i * 4], uv, ct));
        cmd.Add(Vertex(tp[i * 4 + 1], uv, color));
        cmd.Add(Vertex(tp[i * 4 + 2], uv, color));
        cmd.Add(Vertex(tp[i * 4 + 3], uv, ct));
      }
    }
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
                                           const PD::Color& color,
                                           LiDrawFlags flags) {
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
  if (flags & LiDrawFlags_AA) {
    int pc = (int)points.size();
    float aa = 1.f;
    PD::Color ct = color;
    ct.a = 0;
    pTmp.resize(pc);
    for (int i = pc - 1, j = 0; j < pc; i = j++) {
      fvec2 d = NormOZ(points[j] - points[i]);
      pTmp[i] = fvec2(d.y, -d.x);
    }
    cmd.Reserve(pc * 2, (pc - 2) * 3 + pc * 6);
    for (int i = 2; i < pc; i++) {
      cmd.Add(0, (i - 1) << 1, i << 1);
    }
    for (int i = pc - 1, j = 0; j < pc; i = j++) {
      cmd.Add(j << 1, i << 1, (i << 1) + 1);
      cmd.Add((i << 1) + 1, (j << 1) + 1, j << 1);
    }
    for (int i = pc - 1, j = 0; j < pc; i = j++) {
      fvec2 dm = FixNorm((pTmp[i] + pTmp[j]) * 0.5f) * (aa * 0.5f);
      float u = uv_tl.x +
                ((points[j].x - minX) / (maxX - minX)) * (uv_tr.x - uv_tl.x);
      float v = uv_tl.y +
                ((points[j].y - minY) / (maxY - minY)) * (uv_bl.y - uv_tl.y);
      cmd.Add(Vertex(points[j] - dm, fvec2(u, v), color));
      cmd.Add(Vertex(points[j] + dm, fvec2(u, v), ct));
    }
    return;
  }
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