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

#include <pd/lithium/backend.hpp>
#include <pd/lithium/drawlist.hpp>
#include <pd/lithium/font.hpp>
#include <pd/lithium/pd_p_api.hpp>

namespace PD {
namespace LI {
class PD_LITHIUM_API Renderer : public SmartCtor<Renderer> {
 public:
  Renderer(Backend::Ref backend);
  ~Renderer() = default;

  void Render();

  // SECTION: ADVANCED API

  void AddCommand(Command::Ref v) { DrawList.Add(v); }
  void RegisterDrawList(DrawList::Ref list) { pDrawLists.Add(list); }
  Command::Ref PreGenerateCmd();

  // SECTION: Open Command and Object creation API
  static void RotateCorner(fvec2& pos, float sinus, float cosinus);
  static Rect PrimRect(const fvec2& pos, const fvec2& size, float angle = 0.f);
  static Rect PrimLine(const fvec2& a, const fvec2& b, int thickness = 1);
  static void CmdQuad(Command::Ref cmd, const Rect& quad, const Rect& uv,
                      u32 color);
  static void CmdTriangle(Command::Ref cmd, const fvec2 a, const fvec2 b,
                          const fvec2 c, u32 clr);
  static void CmdPolyLine(const Vec<fvec2>& points, u32 clr, u32 flags = 0,
                          int thickness = 1);
  static void CmdConvexPolyFilled(Command::Ref cmd, const Vec<fvec2>& points,
                                  u32 clr, Texture::Ref tex);

  // SECTION: InBounds Checks

  static bool InBox(const fvec2& pos, const fvec2& size, const fvec4& area);
  static bool InBox(const fvec2& pos, const fvec4& area);
  static bool InBox(const fvec2& a, const fvec2& b, const fvec2& c,
                    const fvec4& area);

  // SECTION: Data //

  Texture::Ref WhitePixel = nullptr;
  Backend::Ref pBackend = nullptr;
  Texture::Ref CurrentTex = nullptr;
  int Layer = 0;

 private:
  PD::Vec<Command::Ref> DrawList;
  PD::Vec<DrawList::Ref> pDrawLists;
};
}  // namespace LI
}  // namespace PD