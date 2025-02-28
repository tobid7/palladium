#pragma once

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

#include <pd/lib3ds/memory.hpp>
#include <pd/sound/decoder.hpp>
#include <pd/sound/metadata.hpp>

namespace PD {
namespace Music {
class Player : public SmartCtor<Player> {
 public:
  Player() {}
  ~Player() {}

 private:
  MetaData meta;
  size_t samples_total = 0;
  size_t samples_played = 0;
  size_t samples_per_sec = 0;
  std::string file;
  std::vector<signed short, LinearAllocator<signed short>> buffers[2];
  ndspWaveBuf wave_buf[2] = {0};
  bool last_buf = false;
  int ret = -1;
  bool done = false;
  bool playing = false;
  bool stop = false;
};
}  // namespace Music
}  // namespace PD