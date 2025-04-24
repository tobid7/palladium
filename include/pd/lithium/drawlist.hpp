#pragma once

/*
MIT License

Copyright (c) 2024 - 2025 tobid7

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

#include <pd/lithium/command.hpp>
#include <pd/lithium/font.hpp>
#include <pd/lithium/pd_p_api.hpp>

namespace PD {
namespace LI {
class PD_LITHIUM_API DrawList : public SmartCtor<DrawList> {
 public:
  DrawList(Texture::Ref solid) {
    WhitePixel = solid;
    CurrentTex = solid;
  }
  ~DrawList() {}

  Command::Ref PreGenerateCmd();
  void AddCommand(Command::Ref v) { pDrawList.Add(v); }
  void Clear() { pDrawList.Clear(); }

  void SetFont(Font::Ref font) { pCurrentFont = font; }
  void SetFontScale(float scale) { pFontScale = scale; }

  void DrawSolid() { CurrentTex = WhitePixel; }
  void DrawTexture(Texture::Ref tex) { CurrentTex = tex; }

  // SECTION: Draw API //

  void DrawRect(const fvec2& pos, const fvec2& size, u32 color,
                int thickness = 1);
  void DrawRectFilled(const fvec2& pos, const fvec2& size, u32 color);
  void DrawTriangle(const fvec2& a, const fvec2& b, const fvec2& c, u32 color,
                    int thickness = 1);
  void DrawTriangleFilled(const fvec2& a, const fvec2& b, const fvec2& c,
                          u32 color);
  void DrawCircle(const fvec2& center, float rad, u32 color, int num_segments,
                  int thickness = 1);
  void DrawCircleFilled(const fvec2& center, float rad, u32 color,
                        int num_segments);
  void DrawText(const fvec2& p, const std::string& text, u32 color);

  /**
   * Take list of points and display it as a line on screen
   * @param points List of Positions
   * @param clr Color of the Line
   * @param flags Additional Flags (Close for go back to starting point)
   * @param thickness Thickness of the Line
   */
  void DrawPolyLine(const Vec<fvec2>& points, u32 clr, u32 flags = 0,
                    int thickness = 1);
  /**
   * Take a List ofpoints and display it as Filled Shape
   * @note Keep in mind to setup the list of points clockwise
   * @param points List of Points
   * @param clr Color of the shape
   */
  void DrawConvexPolyFilled(const Vec<fvec2>& points, u32 clr);

  // SECTION: PATH API //

  /**
   * Function to reserve Memory to prevent overhead on
   * pusing a lot of points with PathNext
   * @param num_points Number of Positions you want to add
   */
  void PathReserve(size_t num_points) {
    pPath.Reserve(pPath.Size() + num_points);
  }
  /**
   * Clear current Path
   * @note PathStroke and PathFill will automatically clear
   */
  void PathClear() { pPath.Clear(); }
  /**
   * Add a Point to the Path
   * @note Keep in mind that this function is used for
   * setting the starting point
   * @param v Position to add
   */
  void PathAdd(const fvec2& v) { pPath.Add(v); }
  /**
   * Path Stroke Create Line from point to point
   * @note For Primitives like Rect or Triangle mak sure to use
   * UI7DrawFlags_Close to add a line back to the starting point
   * @param clr Color od the line
   * @param thickness Thickness of the line
   * @param flags Additional Drawflags
   */
  void PathStroke(u32 clr, int thickness = 1, u32 flags = 0) {
    DrawPolyLine(pPath, clr, flags, thickness);
    pPath.Clear();
  }
  /**
   * Fill a Path with a Color
   * @note **IMPORTANT: ** Paths need to be setup clockwise
   * to be rendered correctly
   * @param clr Fill Color
   */
  void PathFill(u32 clr) {
    DrawConvexPolyFilled(pPath, clr);
    pPath.Clear();
  }
  void PathArcToN(const fvec2& c, float radius, float a_min, float a_max,
                  int segments);
  /// @brief Create a Path Rect (uses to Positions instead of Pos/Size)
  /// @param a Top Left Position
  /// @param b Bottom Right Position
  /// @param rounding rounding
  /// @param flags DrawFlags (for special rounding rules)
  void PathRect(fvec2 a, fvec2 b, float rounding = 0.f, u32 flags = 0);

  int Layer = 0;
  float pFontScale = 0.7f;
  Font::Ref pCurrentFont = nullptr;
  Texture::Ref CurrentTex = nullptr;
  Texture::Ref WhitePixel = nullptr;
  PD::Vec<Command::Ref> pDrawList;
  PD::Vec<fvec2> pPath;
};
}  // namespace LI
}  // namespace PD