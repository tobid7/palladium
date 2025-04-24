#include <pd/lithium/renderer.hpp>

namespace PD {
namespace LI {
PD_LITHIUM_API Renderer::Renderer(Backend::Ref backend) {
  pBackend = backend;
  std::vector<PD::u8> white(16 * 16 * 4, 0xff);
  WhitePixel = pBackend->LoadTexture(white, 16, 16);
  CurrentTex = WhitePixel;  // Make sure to have a texture set
}

PD_LITHIUM_API void Renderer::Render() {
  pBackend->NewFrame();
  pBackend->RenderDrawData(DrawList);
  DrawList.Clear();
  for (auto it = pDrawLists.Begin(); it != pDrawLists.End(); it++) {
    pBackend->RenderDrawData((*it)->pDrawList);
    (*it)->Clear();
  }
  pDrawLists.Clear();
}

PD_LITHIUM_API bool Renderer::InBox(const fvec2& pos, const fvec2& szs,
                                    const fvec4& rect) {
  return (pos.x + szs.x >= rect.x && pos.y + szs.y >= rect.y &&
          pos.x <= rect.z && pos.y <= rect.w);
}

PD_LITHIUM_API bool Renderer::InBox(const fvec2& pos, const fvec4& rect) {
  return (pos.x > rect.x && pos.x < rect.x + rect.z && pos.y > rect.y &&
          pos.y < rect.y + rect.w);
}

PD_LITHIUM_API bool Renderer::InBox(const fvec2& alpha, const fvec2& bravo,
                                    const fvec2& charlie, const fvec4& rect) {
  return ((alpha.x < rect.z && bravo.x < rect.z && charlie.x < rect.z) ||
          (alpha.y < rect.w && bravo.y < rect.w && charlie.y < rect.w) ||
          (alpha.x > 0 && bravo.x > 0 && charlie.x > 0) ||
          (alpha.y > 0 && bravo.y > 0 && charlie.y > 0));
}

PD_LITHIUM_API void Renderer::RotateCorner(fvec2& pos, float sinus,
                                           float cosinus) {
  float x = pos.x * cosinus - pos.y * sinus;
  float y = pos.y * cosinus - pos.x * sinus;
  pos = fvec2(x, y);
}

PD_LITHIUM_API Rect Renderer::PrimRect(const fvec2& pos, const fvec2& size,
                                       float angle) {
  fvec2 c = size * 0.5f;  // Center
  fvec2 corner[4] = {
      fvec2(-c.x, -c.y),
      fvec2(-c.x + size.x, -c.y),
      fvec2(-c.x, -c.y + size.y),
      fvec2(-c.x + size.x, -c.y + size.y),
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

PD_LITHIUM_API Rect Renderer::PrimLine(const fvec2& a, const fvec2& b,
                                       int thickness) {
  // Using the vec maths api makes the code as short as it is
  vec2 dir = a - b;
  float len = dir.Len();
  vec2 unit_dir = dir / len;
  vec2 perpendicular(-unit_dir.y, unit_dir.x);
  vec2 off = perpendicular * ((float)thickness * 0.5f);

  return Rect(a + off, b + off, a - off, b - off);
}

PD_LITHIUM_API void Renderer::CmdQuad(Command::Ref cmd, const Rect& quad,
                                      const Rect& uv, u32 color) {
  cmd->AppendIndex(0).AppendIndex(1).AppendIndex(2);
  cmd->AppendIndex(0).AppendIndex(2).AppendIndex(3);
  cmd->AppendVertex(Vertex(quad.BotRight(), uv.BotRight(), color));
  cmd->AppendVertex(Vertex(quad.TopRight(), uv.TopRight(), color));
  cmd->AppendVertex(Vertex(quad.TopLeft(), uv.TopLeft(), color));
  cmd->AppendVertex(Vertex(quad.BotLeft(), uv.BotLeft(), color));
}

PD_LITHIUM_API void Renderer::CmdTriangle(Command::Ref cmd, const fvec2 a,
                                          const fvec2 b, const fvec2 c,
                                          u32 clr) {
  cmd->AppendIndex(2).AppendIndex(1).AppendIndex(0);
  cmd->AppendVertex(Vertex(a, vec2(0.f, 1.f), clr));
  cmd->AppendVertex(Vertex(b, vec2(1.f, 1.f), clr));
  cmd->AppendVertex(Vertex(c, vec2(1.f, 0.f), clr));
}

PD_LITHIUM_API Command::Ref Renderer::PreGenerateCmd() {
  Command::Ref res = Command::New();
  res->Index = DrawList.Size();
  res->Layer = Layer;
  res->Tex = CurrentTex;
  return res;
}

// TODO: Don't render OOS (Probably make it with a define as it
// would probably be faster to render out of screen than checking if
// it could be skipped)
PD_LITHIUM_API void Renderer::CmdConvexPolyFilled(Command::Ref cmd,
                                                  const Vec<fvec2>& points,
                                                  u32 clr, Texture::Ref tex) {
  if (points.Size() < 3 || tex == nullptr) {
    return;  // Need at least three points
  }

  // Support for Custom Textures (UV calculation)
  float minX = points[0].x, minY = points[0].y;
  float maxX = minX, maxY = minY;
  // Check for the max and min Positions
  for (auto it = points.Begin(); it != points.End(); it++) {
    if ((*it).x < minX) minX = (*it).x;
    if ((*it).y < minY) minY = (*it).y;
    if ((*it).x > maxX) maxX = (*it).x;
    if ((*it).y > maxY) maxY = (*it).y;
  }
  // Get Short defines for UV
  // (Bottom Right is not required)
  auto uv_tl = tex->UV.TopLeft();
  auto uv_tr = tex->UV.TopRight();
  auto uv_bl = tex->UV.BotLeft();

  // Render
  for (int i = 2; i < (int)points.Size(); i++) {
    cmd->AppendIndex(0).AppendIndex(i).AppendIndex(i - 1);
  }
  for (int i = 0; i < (int)points.Size(); i++) {
    // Calculate U and V coords
    float u =
        uv_tl.x + ((points[i].x - minX) / (maxX - minX)) * (uv_tr.x - uv_tl.x);
    float v =
        uv_tl.y + ((points[i].y - minY) / (maxY - minY)) * (uv_bl.y - uv_tl.y);
    cmd->AppendVertex(LI::Vertex(points[i], fvec2(u, v), clr));
  }
}
}  // namespace LI
}  // namespace PD