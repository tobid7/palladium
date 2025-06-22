/*
MIT License
Copyright (c) 2024 - 2025 René Amthor (tobid7)

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

#include <pd/ui7/container/container.hpp>

namespace PD {
namespace UI7 {
PD_UI7_API void Container::HandleScrolling(fvec2 scrolling, fvec4 viewport) {
  if (last_use != 0 && OS::GetTime() - last_use > 5000) {
    rem = true;
  }
  last_use = OS::GetTime();
  pos -= fvec2(0, scrolling.y);
  skippable = !Li::Renderer::InBox(
      pos, size,
      fvec4(viewport.x, viewport.y, viewport.x + viewport.z,
            viewport.y + viewport.w));
}

PD_UI7_API void Container::HandleInternalInput() {
  /** Requires Handle Scrolling First */
}
}  // namespace UI7
}  // namespace PD