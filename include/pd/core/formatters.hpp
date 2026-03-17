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

#include <pd/core/color.hpp>

template <>
struct std::formatter<PD::Color> : std::formatter<std::string> {
  enum class ColorFmt {
    Hex,
    Rgbf,
    Rgb,
    Rgba,
    Rgbaf,
  };
  ColorFmt mode = ColorFmt::Hex;
  constexpr auto parse(std::format_parse_context& ctx) {
    auto it = ctx.begin();
    auto end = ctx.end();

    if (it == end || *it == '}') return it;

    if (check(it, end, 5, "rgbaf")) {
      mode = ColorFmt::Rgbaf;
      it += 5;
    } else if (check(it, end, 4, "rgba")) {
      mode = ColorFmt::Rgba;
      it += 4;
    } else if (check(it, end, 4, "rgbf")) {
      mode = ColorFmt::Rgbf;
      it += 4;
    } else if (check(it, end, 3, "rgb")) {
      mode = ColorFmt::Rgb;
      it += 3;
    } else if (check(it, end, 3, "hex")) {
      mode = ColorFmt::Hex;
      it += 3;
    } else {
      throw std::format_error("invalid format for PD::Color");
    }

    if (it == end || *it != '}')
      throw std::format_error("invalid format for PD::Color");

    return it;
  }

  template <typename FormatContext>
  auto format(const PD::Color& value, FormatContext& ctx) const {
    switch (mode) {
      case ColorFmt::Hex:
        return std::format_to(ctx.out(), "#{:02X}{:02X}{:02X}{:02X}", value.r,
                              value.g, value.b, value.a);
      case ColorFmt::Rgb:
        return std::format_to(ctx.out(), "{}, {}, {}", value.r, value.g,
                              value.b);
      case ColorFmt::Rgba:
        return std::format_to(ctx.out(), "{}, {}, {}, {}", value.r, value.g,
                              value.b, value.a);
      case ColorFmt::Rgbf:
        return std::format_to(ctx.out(), "{}, {}, {}", value.rf(), value.gf(),
                              value.bf());
      case ColorFmt::Rgbaf:
        return std::format_to(ctx.out(), "{}, {}, {}, {}", value.rf(),
                              value.gf(), value.bf(), value.af());
      default:
        return ctx.out();
    }
  }

  constexpr bool check(std::format_parse_context::const_iterator it,
                       std::format_parse_context::const_iterator end,
                       size_t len, const char* what) {
    return (end - it >= static_cast<std::ptrdiff_t>(len) &&
            std::string_view(&*it, len) == what);  // msvc things...
  }
};