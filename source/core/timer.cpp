/*
MIT License

Copyright (c) 2024 - 2025 tobid7

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

namespace PD {
PD_CORE_API Timer::Timer(bool autostart) {
  is_running = autostart;
  Reset();
}

PD_CORE_API void Timer::Reset() {
  start = Sys::GetTime();
  now = start;
}

PD_CORE_API void Timer::Update() {
  if (is_running) {
    now = Sys::GetTime();
  }
}

PD_CORE_API void Timer::Pause() { is_running = false; }
PD_CORE_API void Timer::Rseume() { is_running = true; }
PD_CORE_API bool Timer::IsRunning() const { return is_running; }
PD_CORE_API u64 Timer::Get() { return now - start; }
PD_CORE_API double Timer::GetSeconds() { return double(Get()) / 1000.0; }
}  // namespace PD