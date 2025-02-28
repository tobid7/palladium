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
/// @brief Container that holds top and bottom corners of a quad
class Rect {
 public:
  Rect() = default;
  Rect(const vec4& t, const vec4& b) {
    top = t;
    bot = b;
  }
  Rect(const vec2& tl, const vec2& tr, const vec2& bl, const vec2& br) {
    top = vec4(tl, tr);
    bot = vec4(bl, br);
  }
  /// This Constructor Fixes the issue of rewriting some Stuff in the Text
  /// Renderer
  Rect(const vec4& uv) {
    top = vec4(uv.x(), uv.y(), uv.z(), uv.y());
    bot = vec4(uv.x(), uv.w(), uv.z(), uv.w());
  }
  ~Rect() = default;

  vec4 Top() const { return top; }
  vec4 Bot() const { return bot; }

  Rect& Top(const vec4& v) {
    top = v;
    return *this;
  }
  Rect& Bot(const vec4& v) {
    bot = v;
    return *this;
  }

  vec2 TopLeft() const { return vec2(top[0], top[1]); }
  vec2 TopRight() const { return vec2(top[2], top[3]); }
  vec2 BotLeft() const { return vec2(bot[0], bot[1]); }
  vec2 BotRight() const { return vec2(bot[2], bot[3]); }

  Rect& TopLeft(const vec2& v) {
    top[0] = v[0];
    top[1] = v[1];
    return *this;
  }
  Rect& TopRight(const vec2& v) {
    top[2] = v[0];
    top[3] = v[1];
    return *this;
  }
  Rect& BotLeft(const vec2& v) {
    bot[0] = v[0];
    bot[1] = v[1];
    return *this;
  }
  Rect& BotRight(const vec2& v) {
    bot[2] = v[0];
    bot[3] = v[1];
    return *this;
  }

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
  vec4 top;
  vec4 bot;
};
}  // namespace LI
}  // namespace PD