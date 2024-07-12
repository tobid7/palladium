#pragma once

#include <pd/external/json.hpp>
#include <pd/palladium.hpp>

namespace Palladium {
namespace IDB {
void Start();
void Stop();
void Restart();
}  // namespace IDB
}  // namespace Palladium

using PDFlags = int;
enum PDFlags_ {
  PDFlags_None = 0,
  PDFlags_MemTrack = 1 << 0,
  PDFlags_SceneSystem = 1 << 1,
  PDFlags_ShowSplash = 1 << 2,
  PDFlags_Default = PDFlags_SceneSystem,
};

// Outdated HidApi (HidV2Patched)
extern u32 d7_hDown;
extern u32 d7_hHeld;
extern u32 d7_hUp;
extern u32 d7_hRepeat;  // Inofficial lol
extern touchPosition d7_touch;

// Modern Global Api
extern int pd_max_objects;
extern C3D_RenderTarget *pd_top;
extern C3D_RenderTarget *pd_top_right;
extern C3D_RenderTarget *pd_bottom;
extern PDFlags pd_flags;

// Draw2
extern float pd_draw2_tsm;