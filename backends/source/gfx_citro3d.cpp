// Well yes, i finally try it

#include <pd_system/gfx_citro3d.hpp>

#if defined(PD_ENABLE_CITRO3D)  // && defined(__3DS__)

#include <3ds.h>
#include <citro3d.h>

#include <pd/drivers/drivers.hpp>

namespace PD {
struct GfxCitro3D::Impl {};

void GfxCitro3D::SysInit() {
  if (impl) return;
  PDLOG("GfxCitro3D::SysInit();");
  impl = new Impl();
}

void GfxCitro3D::SysDeinit() {
  if (!impl) return;
  delete impl;
  impl = nullptr;
  PDLOG("GfxCitro3D::SysDeinit()");
}

void GfxCitro3D::Submit(size_t count, size_t start) {
  if (!impl) return;
}

void GfxCitro3D::BindTexture(TextureID id) {
  if (!impl) return;
}

void GfxCitro3D::SysReset() {
  if (!impl) return;
}

TextureID GfxCitro3D::LoadTexture(const std::vector<PD::u8>& pixels, int w,
                                  int h, TextureFormat type,
                                  TextureFilter filter) {
  if (!impl) return 0;
  return 0;
}

void GfxCitro3D::DeleteTexture(const TextureID& tex) {
  if (!tex) return;
}
}  // namespace PD
#else
namespace PD {
void GfxCitro3D::SysInit() {
  PDLOG(
      "GfxCitro3D::SysInit: Citro3D Driver is not included in "
      "palladium-system");
}
void GfxCitro3D::SysDeinit() {}
void GfxCitro3D::Submit(size_t count, size_t start) {}
void GfxCitro3D::BindTexture(TextureID id) {}
void GfxCitro3D::SysReset() {}
TextureID GfxCitro3D::LoadTexture(const std::vector<PD::u8>& pixels, int w,
                                  int h, TextureFormat type,
                                  TextureFilter filter) {
  return 0;
}
void GfxCitro3D::DeleteTexture(const TextureID& tex) {}
}  // namespace PD
#endif