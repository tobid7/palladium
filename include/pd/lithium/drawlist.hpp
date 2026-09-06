#pragma once

#include <pd/lithium/command.hpp>
#include <pd/lithium/font.hpp>
#include <pd/lithium/texture.hpp>

using LiPathRectFlags = PD::u32;

enum LiPathRectFlags_ : PD::u32 {
  LiPathRectFlags_None = 0,
  LiPathRectFlags_KeepTopLeft = 1 << 0,
  LiPathRectFlags_KeepTopRight = 1 << 1,
  LiPathRectFlags_KeepBotRight = 1 << 2,
  LiPathRectFlags_KeepBotLeft = 1 << 3,
  LiPathRectFlags_KeepTop =
      LiPathRectFlags_KeepTopLeft | LiPathRectFlags_KeepTopRight,
  LiPathRectFlags_KeepBot =
      LiPathRectFlags_KeepBotLeft | LiPathRectFlags_KeepBotRight,
  LiPathRectFlags_KeepLeft =
      LiPathRectFlags_KeepTopLeft | LiPathRectFlags_KeepBotLeft,
  LiPathRectFlags_KeepRight =
      LiPathRectFlags_KeepTopRight | LiPathRectFlags_KeepBotRight,
};

using LiDrawFlags = PD::u32;
enum LiDrawFlags_ : PD::u32 {
  LiDrawFlags_None = 0,
  LiDrawFlags_Close = 1 << 0,
};

namespace PD {
namespace Li {
class PD_API Drawlist {
 public:
  Drawlist();
  ~Drawlist();

  /** Baisc */

  void Merge(Drawlist& other);
  void Copy(Drawlist& other);
  void Optimize();
  void Clear();

  /** Command Allocation */
  Command& NewCommand();

  /** Path API */
  void PathAdd(const fvec2& point) { pPath.Push(point); }
  void PathAdd(float x, float y) { pPath.Push(fvec2(x, y)); }
  void PathClear() { pPath.Reset(); }
  /**
   * @brief Reserve memory for the next PathAdd uses
   * @note As path will autoexpant and keep the size this func is
   *       only for special use cases
   */
  void PathReserve(size_t num) { pPath.ExpandIf(num); }
  void PathStroke(u32 color, int t = 1, LiDrawFlags flags = LiDrawFlags_None);
  void PathFill(u32 color);
  void PathArcToN(const fvec2& c, float r, float amin, float amax, int s);
  void PathFastArcToN(const fvec2& c, float r, float amin, float amax, int s);
  void PathRect(const fvec2& tl, const fvec2& br, float r = 0.f);
  void PathRectEx(const fvec2& tl, const fvec2& br, float r = 0.f,
                  LiPathRectFlags flags = LiPathRectFlags_None);

  /** Texture Handling */
  void BindTexture(const Texture& tex);
  void UnbindTexture() { pCurrentTexture = Texture(); }

  /** Font Handling */
  void SetFont(Font* font) { pFont = font; }
  void SetFontscale(float fontscale = 1.f) { pFontScale = fontscale; }
  const float& GetFontScale() const { return pFontScale; }

  /** Data geters */
  const Pool<Command>& Data() const { return pCommands; }
  operator const Pool<Command>&() const { return pCommands; }

  /** Drawing functions */
  void DrawRect(const fvec2& pos, const fvec2& size, u32 color, int t = 1);
  void DrawRectFilled(const fvec2& pos, const fvec2& size, u32 color);
  void DrawTriangle(const fvec2& a, const fvec2& b, const fvec2& c, u32 color,
                    int t = 1);
  void DrawTriangleFilled(const fvec2& a, const fvec2& b, const fvec2& c,
                          u32 color);
  void DrawCircle(const fvec2& center, float rad, u32 color, int num_segments,
                  int t = 1);
  void DrawCircleFilled(const fvec2& center, float rad, u32 color,
                        int num_segments);
  void DrawText(const fvec2& p, const char* text, u32 color);
  void DrawTextEx(const fvec2& p, const char* text, u32 color,
                  LiTextFlags flags, const fvec2& box = fvec2(0.f));

  void DrawPolyLine(const Pool<fvec2>& points, u32 color,
                    LiDrawFlags flags = LiDrawFlags_None, int t = 1);
  void DrawConvexPolyFilled(const Pool<fvec2>& points, u32 color);

  void PrimQuad(Command& cmd, const Rect& quad, const Rect& uv, u32 color);
  void PrimTriangle(Command& cmd, const fvec2& a, const fvec2& b,
                    const fvec2& c, u32 color);

 private:
  Texture pCurrentTexture;
  Pool<Command> pCommands;
  Pool<fvec2> pPath;
  Pool<Vertex> pVertices;
  Pool<u16> pIndices;
  Font* pFont = nullptr;
  float pFontScale = 1.f;
};
}  // namespace Li
}  // namespace PD