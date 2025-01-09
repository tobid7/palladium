#pragma once

/*
MIT License
Copyright (c) 2024 René Amthor (tobid7)

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

#include <3ds.h>
#include <citro3d.h>

#include <pd/common/common.hpp>
#include <pd/maths/vec.hpp>

namespace PD {
class Screen : public SmartCtor<Screen> {
 public:
  enum Screen_ { Top, Bottom, TopRight };
  Screen(Screen_ screen) {
    if (screen == Top) {
      target = C3D_RenderTargetCreate(240, 400, GPU_RB_RGBA8,
                                      GPU_RB_DEPTH24_STENCIL8);
      C3D_RenderTargetSetOutput(target, GFX_TOP, GFX_LEFT,
                                DisplayTransferFlags);
    } else if (screen == Bottom) {
      target = C3D_RenderTargetCreate(240, 320, GPU_RB_RGBA8,
                                      GPU_RB_DEPTH24_STENCIL8);
      C3D_RenderTargetSetOutput(target, GFX_BOTTOM, GFX_LEFT,
                                DisplayTransferFlags);
    } else if (screen == TopRight) {
      target = C3D_RenderTargetCreate(240, 400, GPU_RB_RGBA8,
                                      GPU_RB_DEPTH24_STENCIL8);
      C3D_RenderTargetSetOutput(target, GFX_TOP, GFX_RIGHT,
                                DisplayTransferFlags);
    }
  }
  ~Screen() {}

  void Clear() { C3D_RenderTargetClear(target, C3D_CLEAR_ALL, 0x00000000, 0); }
  void Use() { C3D_FrameDrawOn(target); }

  vec2 GetSize() const {
    return vec2(target->frameBuf.height, target->frameBuf.width);
  }

  C3D_RenderTarget* Get() const { return target; }
  operator C3D_RenderTarget*() const { return target; }

 private:
  const u32 DisplayTransferFlags =
      (GX_TRANSFER_FLIP_VERT(0) | GX_TRANSFER_OUT_TILED(0) |
       GX_TRANSFER_RAW_COPY(0) | GX_TRANSFER_IN_FORMAT(GX_TRANSFER_FMT_RGBA8) |
       GX_TRANSFER_OUT_FORMAT(GX_TRANSFER_FMT_RGB8) |
       GX_TRANSFER_SCALING(GX_TRANSFER_SCALE_NO));
  C3D_RenderTarget* target;
};
}  // namespace PD