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

#include <pd/core/core.hpp>
#include <pd/drivers/gfx.hpp>
#include <pd/drivers/hid.hpp>
#include <pd/drivers/os.hpp>

namespace PD {
class PD_API Context {
 public:
  Context()
      : pOs(OsDriver::New()), pGfx(GfxDriver::New()), pHid(HidDriver::New()) {}
  ~Context() {}

  PD_RAW(Context);

  static Context::Ref Create();

  template <typename Driver>
  void UseGfxDriver(PDDriverData data) {
    static_assert(std::is_base_of<GfxDriver, Driver>::value,
                  "Driver must extend GfxDriver");
    pGfx.reset();
    pGfx = Driver::New(data);
  }

  template <typename Driver>
  void UseHidDriver(PDDriverData data) {
    static_assert(std::is_base_of<HidDriver, Driver>::value,
                  "Driver must extend HidDriver");
    pHid.reset();
    pHid = Driver::New(data);
  }

  template <typename Driver>
  void UseOsDriver(PDDriverData data) {
    static_assert(std::is_base_of<OsDriver, Driver>::value,
                  "Driver must extend OsDriver");
    pOs.reset();
    pOs = Driver::New(data);
  }

  PD::Li::Texture::Ref GetSolidTex();

  OsDriver::Ref Os() { return pOs; }
  GfxDriver::Ref Gfx() { return pGfx; }
  HidDriver::Ref Hid() { return pHid; }

 private:
  OsDriver::Ref pOs = nullptr;
  GfxDriver::Ref pGfx = nullptr;
  HidDriver::Ref pHid = nullptr;

  PD::Li::Texture::Ref pSolidTex = nullptr;
};
}  // namespace PD