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

#include <pd/core/common.hpp>
#include <pd/core/vec.hpp>

namespace PD {
namespace LI {
/**
 * Container that holds position of a rectangle's corners.
 */
class Rect {
 public:
  Rect() = default;

  /**
   * Constructor that initializes the rectangle using top and bottom positions.
   * @param t Top left and right corner positions.
   * @param b Bottom left and right corner positions.
   */
  Rect(const vec4& t, const vec4& b) {
    top = t;
    bot = b;
  }

  /**
   * Constructor that initializes the rectangle using individual corner
   * positions.
   * @param tl Top left corner position.
   * @param tr Top right corner position.
   * @param bl Bottom left corner position.
   * @param br Bottom right corner position.
   */
  Rect(const vec2& tl, const vec2& tr, const vec2& bl, const vec2& br) {
    top = vec4(tl, tr);
    bot = vec4(bl, br);
  }

  /**
   * Constructor that initializes the rectangle using a UV mapping vector.
   *
   * - The old API used vec4 for UV mapping.
   * - Spritesheets have rotated images, so this was updated to use Rect for UV.
   *
   * @param uv Vec4 UV map.
   */
  Rect(const vec4& uv) {
    top = vec4(uv.x(), uv.y(), uv.z(), uv.y());
    bot = vec4(uv.x(), uv.w(), uv.z(), uv.w());
  }

  ~Rect() = default;

  /**
   * Get the top left and right corner positions.
   * @return Top positions.
   */
  vec4 Top() const { return top; }

  /**
   * Get the bottom left and right corner positions.
   * @return Bottom positions.
   */
  vec4 Bot() const { return bot; }

  /**
   * Set the top left and right corner positions.
   * @param v New top positions.
   * @return Reference to the updated Rect.
   */
  Rect& Top(const vec4& v) {
    top = v;
    return *this;
  }

  /**
   * Set the bottom left and right corner positions.
   * @param v New bottom positions.
   * @return Reference to the updated Rect.
   */
  Rect& Bot(const vec4& v) {
    bot = v;
    return *this;
  }

  /**
   * Get the top-left corner position.
   * @return Top-left position as vec2.
   */
  vec2 TopLeft() const { return vec2(top[0], top[1]); }

  /**
   * Get the top-right corner position.
   * @return Top-right position as vec2.
   */
  vec2 TopRight() const { return vec2(top[2], top[3]); }

  /**
   * Get the bottom-left corner position.
   * @return Bottom-left position as vec2.
   */
  vec2 BotLeft() const { return vec2(bot[0], bot[1]); }

  /**
   * Get the bottom-right corner position.
   * @return Bottom-right position as vec2.
   */
  vec2 BotRight() const { return vec2(bot[2], bot[3]); }

  /**
   * Set the top-left corner position.
   * @param v New top-left position.
   * @return Reference to the updated Rect.
   */
  Rect& TopLeft(const vec2& v) {
    top[0] = v[0];
    top[1] = v[1];
    return *this;
  }

  /**
   * Set the top-right corner position.
   * @param v New top-right position.
   * @return Reference to the updated Rect.
   */
  Rect& TopRight(const vec2& v) {
    top[2] = v[0];
    top[3] = v[1];
    return *this;
  }

  /**
   * Set the bottom-left corner position.
   * @param v New bottom-left position.
   * @return Reference to the updated Rect.
   */
  Rect& BotLeft(const vec2& v) {
    bot[0] = v[0];
    bot[1] = v[1];
    return *this;
  }

  /**
   * Set the bottom-right corner position.
   * @param v New bottom-right position.
   * @return Reference to the updated Rect.
   */
  Rect& BotRight(const vec2& v) {
    bot[2] = v[0];
    bot[3] = v[1];
    return *this;
  }

  /**
   * Swap X and Y coordinates for all corners.
   *
   * - Used in SpiteSheet for the rotated images.
   */
  void SwapVec2XY() {
    for (int i = 0; i < 4; i += 2) {
      float t = top[i];
      top[i] = top[i + 1];
      top[i + 1] = t;
      t = bot[i];
      bot[i] = bot[i + 1];
      bot[i + 1] = t;
    }
  }

 private:
  vec4 top;  ///< Top left and right corner positions.
  vec4 bot;  ///< Bottom left and right corner positions.
};
}  // namespace LI
}  // namespace PD