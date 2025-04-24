#pragma once

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
#include <pd/lithium/renderer.hpp>
#include <pd/ui7/flags.hpp>
#include <pd/ui7/pd_p_api.hpp>
#include <pd/ui7/theme.hpp>

namespace PD {
namespace UI7 {
class IO;
/** DrawList class */
class PD_UI7_API DrawList : public SmartCtor<DrawList> {
 public:
  /**
   * Constructor for a new Drawlist
   * @param r Renderer reference
   */
  DrawList(UI7::IO* io_ref) { pIO = io_ref; }
  ~DrawList() = default;

  /**
   * Draw a Rectangle (LINED)
   * @param pos position of the rect
   * @param size Size of the rect
   * @param clr Color of the rect
   * @param thickness Thickness of the lines
   */
  void AddRect(const fvec2& pos, const fvec2& size, const UI7Color& clr,
               int thickness = 1);
  /**
   * Render a Rectangle
   * @param pos Position
   * @param szs Size
   * @param clr Color
   */
  void AddRectangle(fvec2 pos, fvec2 szs, const UI7Color& clr);
  /**
   * Render a Triangle
   * @param a Position a
   * @param b Position b
   * @param c Position c
   * @param clr Color
   * @param thickness Thickness of the lines
   */
  void AddTriangle(const fvec2& a, const fvec2& b, const fvec2& c,
                   const UI7Color& clr, int thickness = 1);
  /**
   * Render a Filled Triangle
   * @param a Position a
   * @param b Position b
   * @param c Position c
   * @param clr Color
   */
  void AddTriangleFilled(const fvec2& a, const fvec2& b, const fvec2& c,
                         const UI7Color& clr);

  /**
   * Add a Lined Circle
   * @param pos Center position
   * @param rad radius of the circle
   * @param col Color of the Circle
   * @param num_segments Number of Segments (0 = auto)
   * @param thickness thickness of the line
   */
  void AddCircle(const fvec2& pos, float rad, UI7Color col,
                 int num_segments = 0, int thickness = 1);
  /**
   * Add a Circle
   * @param pos Center position
   * @param rad radius of the circle
   * @param col Color of the Circle
   * @param num_segments Number of Segments (0 = auto)
   */
  void AddCircleFilled(const fvec2& pos, float rad, UI7Color col,
                       int num_segments = 0);
  /**
   * Render a Text
   * @param pos Position
   * @param text Text
   * @param clr Color
   * @param flags Flags
   * @param box Aditional Text Box limit (for specific flags)
   */
  void AddText(fvec2 pos, const std::string& text, const UI7Color& clr,
               u32 flags = 0, fvec2 box = fvec2());
  /**
   * Render an Image
   * @param pos Position
   * @param img Image Texture Reference
   * @param size Optional Size of the Image
   * @param uv Custom UV coords
   */
  void AddImage(fvec2 pos, LI::Texture::Ref img, fvec2 size = 0.f,
                LI::Rect uv = fvec4(0.f));
  /**
   * Render a Line from Position A to Position B
   * @param a Pos a
   * @param b Pos b
   * @param clr Color
   * @param t Thcikness
   */
  void AddLine(const fvec2& a, const fvec2& b, const UI7Color& clr, int t = 1);

  /**
   * Take list of points and display it as a line on screen
   * @param points List of Positions
   * @param clr Color of the Line
   * @param flags Additional Flags (Close for go back to starting point)
   * @param thickness Thickness of the Line
   */
  void AddPolyLine(const Vec<fvec2>& points, const UI7Color& clr,
                   UI7DrawFlags flags = 0, int thickness = 1);
  /**
   * Take a List ofpoints and display it as Filled Shape
   * @note Keep in mind to setup the list of points clockwise
   * @param points List of Points
   * @param clr Color of the shape
   */
  void AddConvexPolyFilled(const Vec<fvec2>& points, const UI7Color& clr);

  /** Clear the Drawlist */
  void Clear();
  /** Process [Render] the Drawlist */
  void Process(LI::DrawList::Ref d);

  /** Push a Clip Rect */
  void PushClipRect(const fvec4& v) { pClipRects.Push(v); }

  /** Revert Last Clip Rect */
  void PopClipRect() { pClipRects.Pop(); }

  /** Path API */

  /**
   * Function to reserve Memory to prevent overhead on
   * pusing a lot of points with PathNext
   * @param num_points Number of Positions you want to add
   */
  void PathReserve(size_t num_points) {
    Path.Reserve(Path.Size() + num_points);
  }
  /**
   * Clear current Path
   * @note PathStroke and PathFill will automatically clear
   */
  void PathClear() { Path.Clear(); }
  /**
   * Add a Point to the Path
   * @note Keep in mind that this function is used for
   * setting the starting point
   * @param v Position to add
   */
  void PathNext(const fvec2& v) { Path.Add(v); }
  /**
   * Path Stroke Create Line from point to point
   * @note For Primitives like Rect or Triangle mak sure to use
   * UI7DrawFlags_Close to add a line back to the starting point
   * @param clr Color od the line
   * @param thickness Thickness of the line
   * @param flags Additional Drawflags
   */
  void PathStroke(const UI7Color& clr, int thickness = 1,
                  UI7DrawFlags flags = 0) {
    AddPolyLine(Path, clr, flags, thickness);
    Path.Clear();
  }
  /**
   * Fill a Path with a Color
   * @note **IMPORTANT: ** Paths need to be setup clockwise
   * to be rendered correctly
   * @param clr Fill Color
   */
  void PathFill(const UI7Color& clr) {
    AddConvexPolyFilled(Path, clr);
    Path.Clear();
  }

  void PathArcToN(const fvec2& c, float radius, float a_min, float a_max,
                  int segments);
  /// @brief Create a Path Rect (uses to Positions instead of Pos/Size)
  /// @param a Top Left Position
  /// @param b Bottom Right Position
  /// @param rounding rounding
  /// @param flags DrawFlags (for special rounding rules)
  void PathRect(fvec2 a, fvec2 b, float rounding = 0.f, UI7DrawFlags flags = 0);

  int Layer;                    ///< Layer
  int Base;                     ///< Base Layer
  Stack<fvec4> pClipRects;      ///< ClipRects
  u32 NumVertices;              ///< Num vertices
  u32 NumIndices;               ///< Num Indices
  UI7::IO* pIO;                 ///< IO Reference
  LI::Texture::Ref CurrentTex;  ///< Current Texture

 private:
  /**
   * One liner to setup command cliprect
   */
  void ClipCmd(LI::Command::Ref cmd);

  // Set friendclass here to not expose private functions as public
  friend class Menu;
  friend class Context;

  Vec<fvec2> Path;
  // Map for Auto Static Text
  // std::unordered_map<u32, LI::StaticText::Ref> static_text;
  // List of Drawcommands generated
  Vec<LI::Command::Ref> Commands;
};
}  // namespace UI7
}  // namespace PD