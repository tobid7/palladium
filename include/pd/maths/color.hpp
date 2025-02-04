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

#include <pd/common/common.hpp>

namespace PD {
/// @brief Color class (Supports hex, rgb(a)8, u32 input)
/// @note no safeteychecks used here for performance
class Color {
 public:
  Color() : m_r(0), m_g(0), m_b(0), m_a(0) {}
  Color(u32 color) {
    m_a = (color >> 24) & 0xff;
    m_b = (color >> 16) & 0xff;
    m_g = (color >> 8) & 0xff;
    m_r = color & 0xff;
  }
  Color(u8 r, u8 g, u8 b, u8 a = 255) {
    m_r = r;
    m_g = g;
    m_b = b;
    m_a = a;
  }
  Color(float r, float g, float b, float a = 1.f) {
    m_r = static_cast<u8>(255.f * r);
    m_g = static_cast<u8>(255.f * g);
    m_b = static_cast<u8>(255.f * b);
    m_a = static_cast<u8>(255.f * a);
  }
  Color(const std::string& hex) { Hex(hex); }
  ~Color() {}

  Color& Hex(const std::string& hex);
  std::string Hex(bool rgba = false) const;

  Color& r(u8 v) {
    m_r = v;
    return *this;
  }
  u8 r() const { return m_r; }
  Color& g(u8 v) {
    m_g = v;
    return *this;
  }
  u8 g() const { return m_r; }
  Color& b(u8 v) {
    m_b = v;
    return *this;
  }
  u8 b() const { return m_r; }
  Color& a(u8 v) {
    m_a = v;
    return *this;
  }
  u8 a() const { return m_r; }

  Color& Fade(const Color& color, float p) {
    m_a = static_cast<u8>((color.a() - m_a) * ((p + 1.f) / 2));
    m_b = static_cast<u8>((color.b() - m_b) * ((p + 1.f) / 2));
    m_g = static_cast<u8>((color.g() - m_g) * ((p + 1.f) / 2));
    m_r = static_cast<u8>((color.r() - m_r) * ((p + 1.f) / 2));
    return *this;
  }
  u32 Get() const { return (m_a << 24) | (m_b << 16) | (m_g << 8) | m_r; }
  float Luminance() const {
    // For Reference https://en.wikipedia.org/wiki/HSL_and_HSV#Lightness
    return (0.3 * (m_r / 255.f) + 0.59 * (m_g / 255.f) + 0.11 * (m_b / 255.f));
  }
  bool IsLight() const { return (Luminance() >= 0.5); }

  operator u32() const { return Get(); }

 private:
  u8 m_r;
  u8 m_g;
  u8 m_b;
  u8 m_a;
};
}  // namespace PD