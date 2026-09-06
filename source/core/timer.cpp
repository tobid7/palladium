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

#include <pd/core/timer.hpp>
#include <pd/drivers/drivers.hpp>

namespace PD {
PD_API Timer::Timer(OsDriver& os, bool autostart) : pOs(os) {
  pIsRunning = autostart;
  Reset();
}

PD_API void Timer::Reset() {
  pStart = pOs.GetTime();
  pNow = pStart;
}

PD_API void Timer::Update() {
  if (pIsRunning) {
    pNow = pOs.GetTime();
  }
}

PD_API void Timer::Pause() { pIsRunning = false; }
PD_API void Timer::Rseume() { pIsRunning = true; }
PD_API bool Timer::IsRunning() const { return pIsRunning; }
PD_API u64 Timer::Get() { return pNow - pStart; }
PD_API double Timer::GetSeconds() { return (double)Get() / 1000.0; }
}  // namespace PD