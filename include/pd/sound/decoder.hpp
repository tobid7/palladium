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

#include <pd/core/common.hpp>

namespace PD {
namespace Music {
/**
 * Decoder Template class
 */
class Decoder : public SmartCtor<Decoder> {
 public:
  Decoder() = default;
  virtual ~Decoder() = default;

  /** Template Init function */
  virtual int Init(const std::string& path) = 0;
  /** Template deinit function */
  virtual void Deinit() = 0;
  /** Template function to get sample rate */
  virtual u32 GetSampleRate() = 0;
  /** template function to get number of channels */
  virtual u8 GetChannels() = 0;
  /** template function to get buffer size */
  virtual size_t GetBufSize() = 0;
  /** template decode function */
  virtual u64 Decode(u16* buf_address) = 0;
  /** template function to get file sanples if exist */
  virtual size_t GetFileSamples() = 0;
};
}  // namespace Music
}  // namespace PD