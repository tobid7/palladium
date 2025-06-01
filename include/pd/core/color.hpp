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

#include <pd/core/common.hpp>

namespace PD {
/**
 * Color class
 *
 * - Supports hex input starting with a # and 6 or 8 digits
 * - Supports rgb(a) 8Bit unsigned number input
 * - Supports rgb(a) float input from 0.0 to 1.0
 * - Supports 32Bit input color
 * @note Safetey checks are disabled for maximum performance
 */
class PD_CORE_API Color {
 private:
  /** Red Value */
  u8 m_r;
  /** Green Value */
  u8 m_g;
  /** Blue Value */
  u8 m_b;
  /** Alpha Value */
  u8 m_a;

 public:
  /**
   * Default Constructor (all variables are set to 0)
   */
  // Color() : m_r(0), m_g(0), m_b(0), m_a(0) {}
  constexpr Color() : m_r(0), m_g(0), m_b(0), m_a(0) {}
  constexpr ~Color() {}
  /**
   * Constructor for 32Bit Color Input
   * @param color 32Bit Color value
   */
  constexpr Color(u32 color) {
    m_a = (color >> 24) & 0xff;
    m_b = (color >> 16) & 0xff;
    m_g = (color >> 8) & 0xff;
    m_r = color & 0xff;
  }
  /**
   * Constructor for 8Bit Input
   * @param r Red Value
   * @param g Green Value
   * @param b Blue Value
   * @param a Optional Alpha Value (Defaults to 255)
   */
  constexpr Color(int r, int g, int b, int a = 255) {
    m_r = r;
    m_g = g;
    m_b = b;
    m_a = a;
  }
  /**
   * Constructor for float Input
   * @param r Red Value
   * @param g Green Value
   * @param b Blue Value
   * @param a Optional Alpha Value (Defaults to 1.0f)
   * @note There is no Check if the number is between 0.0 and 1.0
   */
  constexpr Color(float r, float g, float b, float a = 1.f) {
    m_r = static_cast<u8>(255.f * r);
    m_g = static_cast<u8>(255.f * g);
    m_b = static_cast<u8>(255.f * b);
    m_a = static_cast<u8>(255.f * a);
  }
  /**
   * Constructor for Hex Input
   * @param hex Hex String in `#ffffff` or `#ffffffff` format
   */
  Color(const std::string& hex) { Hex(hex); }
  /**
   * Unused Deconstructor
   */
  // ~Color() {}

  /**
   * Create Color Object by Hex String
   * @param hex Hex String in `#ffffff` or `#ffffffff` format
   * @return Color class itself
   */
  Color& Hex(const std::string& hex);
  /**
   * Convert this Color Object to Hex string
   * @param rgba [default false] sets if 8 or 6 digit color should be returned
   * @return Color Hex String
   */
  std::string Hex(bool rgba = false) const;

  /**
   * Setter for Red
   * @param v value
   * @return Color class reference
   */
  Color& r(u8 v) {
    m_r = v;
    return *this;
  }
  /**
   * Getter for Red
   * @return Red Value
   */
  u8 r() const { return m_r; }
  /**
   * Setter for Green
   * @param v value
   * @return Color class reference
   */
  Color& g(u8 v) {
    m_g = v;
    return *this;
  }
  /**
   * Getter for Green
   * @return Green Value
   */
  u8 g() const { return m_g; }
  /**
   * Setter for Blue
   * @param v value
   * @return Color class reference
   */
  Color& b(u8 v) {
    m_b = v;
    return *this;
  }
  /**
   * Getter for Blue
   * @return Blue Value
   */
  u8 b() const { return m_b; }
  /**
   * Setter for Alpha
   * @param v value
   * @return Color class reference
   */
  Color& a(u8 v) {
    m_a = v;
    return *this;
  }
  /**
   * Getter for Alpha
   * @return Alpha Value
   */
  u8 a() const { return m_a; }

  /**
   * Fade from Current to another Color
   * @param color Color to fade to
   * @param p Amount (supports -1.0 to 1.0 for use of sine)
   * @return Class Reference
   */
  Color& Fade(const Color& color, float p) {
    m_a = static_cast<u8>((color.a() - m_a) * ((p + 1.f) / 2));
    m_b = static_cast<u8>((color.b() - m_b) * ((p + 1.f) / 2));
    m_g = static_cast<u8>((color.g() - m_g) * ((p + 1.f) / 2));
    m_r = static_cast<u8>((color.r() - m_r) * ((p + 1.f) / 2));
    return *this;
  }
  /**
   * Get 32Bit Color Value
   * @return 32Bit Color Value
   */
  u32 Get() const { return (m_a << 24) | (m_b << 16) | (m_g << 8) | m_r; }
  /**
   * Get The Luminance of the Color
   * @return luminance (from 0.0 to 1.0)
   */
  float Luminance() const {
    // For Reference https://en.wikipedia.org/wiki/HSL_and_HSV#Lightness
    return (0.3 * (m_r / 255.f) + 0.59 * (m_g / 255.f) + 0.11 * (m_b / 255.f));
  }
  /**
   * Check if the Color is Light or Dark
   * @return true if light
   */
  bool IsLight() const { return (Luminance() >= 0.5); }

  /**
   * Operator to cast Color to 32Bit Value
   * @return 32Bit Color Value
   */
  operator u32() const { return Get(); }
};
namespace Colors {
constexpr Color White = Color(1.f, 1.f, 1.f, 1.f);
constexpr Color Black = Color(0.f, 0.f, 0.f, 1.f);
constexpr Color Red = Color(1.f, 0.f, 0.f, 1.f);
constexpr Color Green = Color(0.f, 1.f, 0.f, 1.f);
constexpr Color Blue = Color(0.f, 0.f, 1.f, 1.f);
constexpr Color Yellow = Color(1.f, 1.f, 0.f, 1.f);
constexpr Color Cyan = Color(0.f, 1.f, 1.f, 1.f);
constexpr Color Magenta = Color(1.f, 0.f, 1.f, 1.f);
constexpr Color Gray = Color(0.5f, 0.5f, 0.5f, 1.f);
constexpr Color LightGray = Color(0.75f, 0.75f, 0.75f, 1.f);
constexpr Color DarkGray = Color(0.25f, 0.25f, 0.25f, 1.f);
constexpr Color Orange = Color(1.f, 0.65f, 0.f, 1.f);
constexpr Color Pink = Color(1.f, 0.75f, 0.8f, 1.f);
constexpr Color Brown = Color(0.6f, 0.4f, 0.2f, 1.f);
constexpr Color Purple = Color(0.5f, 0.f, 0.5f, 1.f);
constexpr Color Teal = Color(0.f, 0.5f, 0.5f, 1.f);
constexpr Color Transparent = Color(0.f, 0.f, 0.f, 0.f);
}  // namespace Colors
}  // namespace PD