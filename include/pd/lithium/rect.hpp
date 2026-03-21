#pragma once

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

#include <pd/core/core.hpp>

namespace PD {
namespace Li {
class Rect {
 public:
  constexpr Rect() : Top(0), Bot(0) {}
  ~Rect() = default;
  /**
   * Constructor that initializes the rectangle using top and bottom positions.
   * @param t Top left and right corner positions.
   * @param b Bottom left and right corner positions.
   */
  constexpr Rect(const fvec4& t, const fvec4& b) {
    Top = t;
    Bot = b;
  }
  /**
   * Constructor that initializes the rectangle using individual corner
   * positions.
   * @param tl Top left corner position.
   * @param tr Top right corner position.
   * @param bl Bottom left corner position.
   * @param br Bottom right corner position.
   */
  constexpr Rect(const fvec2& tl, const fvec2& tr, const fvec2& bl,
                 const fvec2& br) {
    Top = fvec4(tl, tr);
    Bot = fvec4(bl, br);
  }

  /**
   * Constructor that initializes the rectangle using a UV mapping vector.
   *
   * - The old API used vec4 for UV mapping.
   * - Spritesheets have rotated images, so this was updated to use Rect for UV.
   *
   * @param uv Vec4 UV map.
   */
  constexpr Rect(const fvec4& uv) {
    Top = vec4(uv.x, uv.y, uv.z, uv.y);
    Bot = vec4(uv.x, uv.w, uv.z, uv.w);
  }

  /**
   * Get the top-left corner position.
   * @return Top-left position as vec2.
   */
  constexpr fvec2 TopLeft() const { return fvec2(Top.x, Top.y); }
  /**
   * Get the top-right corner position.
   * @return Top-right position as vec2.
   */
  constexpr fvec2 TopRight() const { return fvec2(Top.z, Top.w); }
  /**
   * Get the bottom-left corner position.
   * @return Bottom-left position as vec2.
   */
  constexpr fvec2 BotLeft() const { return fvec2(Bot.x, Bot.y); }
  /**
   * Get the bottom-right corner position.
   * @return Bottom-right position as vec2.
   */
  constexpr fvec2 BotRight() const { return fvec2(Bot.z, Bot.w); }

  /**
   * Set the top-left corner position.
   * @param v New top-left position.
   * @return Reference to the updated Rect.
   */
  constexpr Rect& TopLeft(const fvec2& v) {
    Top.x = v.x;
    Top.y = v.y;
    return *this;
  }

  /**
   * Set the top-right corner position.
   * @param v New top-right position.
   * @return Reference to the updated Rect.
   */
  constexpr Rect& TopRight(const fvec2& v) {
    Top.z = v.x;
    Top.w = v.y;
    return *this;
  }

  /**
   * Set the bottom-left corner position.
   * @param v New bottom-left position.
   * @return Reference to the updated Rect.
   */
  constexpr Rect& BotLeft(const fvec2& v) {
    Bot.x = v.x;
    Bot.y = v.y;
    return *this;
  }

  /**
   * Set the bottom-right corner position.
   * @param v New bottom-right position.
   * @return Reference to the updated Rect.
   */
  constexpr Rect& BotRight(const fvec2& v) {
    Bot.z = v.x;
    Bot.w = v.y;
    return *this;
  }

  constexpr bool operator==(const Rect& r) const {
    return Top == r.Top && Bot == r.Bot;
  }

  constexpr void SwapVec2XY() {
    Top.SwapXY();
    Top.SwapZW();
    Bot.SwapXY();
    Bot.SwapZW();
  }

  /** Data Section */

  fvec4 Top;
  fvec4 Bot;
};
}  // namespace Li
}  // namespace PD