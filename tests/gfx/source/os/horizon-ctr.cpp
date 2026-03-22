#include <os/horizon-ctr.hpp>

#if defined(__3DS__)
#include <3ds.h>
#include <citro3d.h>
const u32 DisplayTransferFlags =
    (GX_TRANSFER_FLIP_VERT(0) | GX_TRANSFER_OUT_TILED(0) |
     GX_TRANSFER_RAW_COPY(0) | GX_TRANSFER_IN_FORMAT(GX_TRANSFER_FMT_RGBA8) |
     GX_TRANSFER_OUT_FORMAT(GX_TRANSFER_FMT_RGB8) |
     GX_TRANSFER_SCALING(GX_TRANSFER_SCALE_NO));

#include <pdsystem>

namespace PD {
struct HorizonCtr::Impl {
  C3D_RenderTarget* Top = nullptr;
  C3D_RenderTarget* Bottom = nullptr;
  uint32_t* pSocBuf = nullptr;
};

void HorizonCtr::Init() {
  if (impl) return;
  impl = new Impl();
  impl->pSocBuf = (uint32_t*)std::aligned_alloc(0x1000, 0x100000);
  if (impl->pSocBuf) {
    Result ret = socInit(impl->pSocBuf, 0x100000);
    if (R_FAILED(ret)) {
      free(impl->pSocBuf);
      impl->pSocBuf = nullptr;
    }
  }
  link3dsStdio();
  romfsInit();
  gfxInitDefault();
  consoleInit(GFX_BOTTOM, nullptr);
  C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
  impl->Top =
      C3D_RenderTargetCreate(240, 400, GPU_RB_RGBA8, GPU_RB_DEPTH24_STENCIL8);
  impl->Bottom =
      C3D_RenderTargetCreate(240, 320, GPU_RB_RGBA8, GPU_RB_DEPTH24_STENCIL8);
  C3D_RenderTargetSetOutput(impl->Top, GFX_TOP, GFX_LEFT, DisplayTransferFlags);
  C3D_RenderTargetSetOutput(impl->Bottom, GFX_BOTTOM, GFX_LEFT,
                            DisplayTransferFlags);
  PD::Gfx::UseDriver<PD::GfxCitro3D>();
}

void HorizonCtr::Deinit() {
  if (impl) {
    C3D_RenderTargetDelete(impl->Top);
    C3D_RenderTargetDelete(impl->Bottom);
    C3D_Fini();
    gfxExit();
    romfsExit();
    socExit();
    delete impl;
    impl = nullptr;
  }
}

bool HorizonCtr::Mainloop() {
  pViewPort = ivec2(400, 240);
  hidScanInput();
  bool _kill = hidKeysUp() & KEY_START;
  return aptMainLoop() && !_kill;
}

void HorizonCtr::ClearViewPort() {
  PD::Gfx::SetViewPort(pViewPort);
  C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
  C3D_FrameDrawOn(impl->Top);
  C3D_RenderTargetClear(impl->Top, C3D_CLEAR_ALL, PD::Color(25, 25, 25, 25), 0);
}

void HorizonCtr::SwapBuffers() { C3D_FrameEnd(0); }

}  // namespace PD
#endif