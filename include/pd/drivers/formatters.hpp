#pragma once

#include <pd/drivers/gfx.hpp>

template <>
struct std::formatter<PD::TextureFilter> {
  constexpr auto parse(std::format_parse_context& ctx) { return ctx.begin(); }

  template <typename FormatContext>
  auto format(const PD::TextureFilter& value, FormatContext& ctx) const {
    std::string_view ret = "Unknown";
    switch (value) {
      case PD::TextureFilter::Linear:
        ret = "Linear";
        break;
      case PD::TextureFilter::Nearest:
        ret = "Nearest";
        break;
    }
    return std::format_to(ctx.out(), "{}", ret);
  }
};

template <>
struct std::formatter<PD::TextureFormat> {
  constexpr auto parse(std::format_parse_context& ctx) { return ctx.begin(); }

  template <typename FormatContext>
  auto format(const PD::TextureFormat& value, FormatContext& ctx) const {
    std::string_view ret = "Unknown";
    switch (value) {
      case PD::TextureFormat::RGBA32:
        ret = "RGBA32";
        break;
      case PD::TextureFormat::RGB24:
        ret = "RGB24";
        break;
      case PD::TextureFormat::A8:
        ret = "A8";
        break;
    }
    return std::format_to(ctx.out(), "{}", ret);
  }
};