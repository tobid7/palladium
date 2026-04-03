#pragma once

#include <pd/common.hpp>

using UltraAlignment = PD::u32;
enum UltraAlignment_ {
  UltraAlignment_None = 0,
  UltraAlignment_Top = 1 << 0,
  UltraAlignment_Bot = 1 << 1,
  UltraAlignment_Left = 1 << 2,
  UltraAlignment_Right = 1 << 3,
  UltraAlignment_CenterVertical = 1 << 4,
  UltraAlignment_CenterHorizontal = 1 << 5,
  UltraAlignment_TopLeft = UltraAlignment_Top | UltraAlignment_Left,
  UltraAlignment_TopRight = UltraAlignment_Top | UltraAlignment_Right,
  UltraAlignment_BotLeft = UltraAlignment_Bot | UltraAlignment_Left,
  UltraAlignment_BotRight = UltraAlignment_Bot | UltraAlignment_Right,
  UltraAlignment_Center =
      UltraAlignment_CenterVertical | UltraAlignment_CenterHorizontal,
};