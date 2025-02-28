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

#include <mpg123.h>

#include <pd/sound/decoder.hpp>

namespace PD {
namespace Music {
class Mp3Decoder : public Decoder {
 public:
  Mp3Decoder() {}
  ~Mp3Decoder() {}

  int Init(const std::string& path) override;
  void Deinit() override;
  u32 GetSampleRate() override;
  u8 GetChannels() override;
  size_t GetBufSize() override;
  u64 Decode(u16* buf_address) override;
  size_t GetFileSamples() override;

 private:
  mpg123_handle* handle = nullptr;
  size_t buf_size = 0;
  u32 rate = 0;
  u8 channels = 0;
};
}  // namespace Music
}  // namespace PD