#pragma once

#include <pd/lithium/atlas.hpp>
#include <pd/lithium/rect.hpp>
#include <pd/lithium/texture.hpp>
#include <pd/lithium/vertex.hpp>

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

template <>
struct std::formatter<PD::Li::Rect> : std::formatter<std::string> {
  constexpr auto parse(std::format_parse_context& ctx) { return ctx.begin(); }

  template <typename FormatContext>
  auto format(const PD::Li::Rect& value, FormatContext& ctx) const {
    // {l, t, r, t, l, b, r, b}
    return std::format_to(ctx.out(), "{}, {}, {}, {}, {}, {}, {}, {}",
                          value.Top.x, value.Top.y, value.Top.z, value.Top.w,
                          value.Bot.x, value.Bot.y, value.Bot.z, value.Bot.w);
  }
};

template <>
struct std::formatter<PD::Li::Texture> : std::formatter<std::string> {
  constexpr auto parse(std::format_parse_context& ctx) { return ctx.begin(); }

  template <typename FormatContext>
  auto format(const PD::Li::Texture& value, FormatContext& ctx) const {
    // {ID, [w, h], [l, t, r, t, l, b, r, b]}
    return std::format_to(ctx.out(), "{}, [{}], [{}]", value.GetID(),
                          value.GetSize(), value.GetUV());
  }
};

template <>
struct std::formatter<PD::Li::Vertex> : std::formatter<std::string> {
  constexpr auto parse(std::format_parse_context& ctx) { return ctx.begin(); }

  template <typename FormatContext>
  auto format(const PD::Li::Vertex& value, FormatContext& ctx) const {
    // { [x, y], [u, v], colorhex }
    return std::format_to(ctx.out(), "[{}], [{}], #{:08X}", value.pos, value.uv,
                          value.color);
  }
};

template <>
struct std::formatter<PD::Li::AtlasState> : std::formatter<std::string> {
  constexpr auto parse(std::format_parse_context& ctx) { return ctx.begin(); }

  template <typename FormatContext>
  auto format(const PD::Li::AtlasState& value, FormatContext& ctx) const {
    std::string_view ret = "Unknown";
    switch (value) {
      case PD::Li::AtlasState::Invalid:
        ret = "Invalid";
        break;
      case PD::Li::AtlasState::ReqCreate:
        ret = "RequestCreate";
        break;
      case PD::Li::AtlasState::ReqUpdate:
        ret = "RequestUpdate";
        break;
      case PD::Li::AtlasState::ReqDestroy:
        ret = "RequestDestroy";
        break;
    }
    return std::format_to(ctx.out(), "{}", ret);
  }
};