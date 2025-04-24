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

#include <pd/sound/mp3.hpp>

namespace PD {
namespace Music {
int Mp3Decoder::Init(const std::string& path) {
  int ret = 0;
  int encoding = 0;
  if ((ret = mpg123_init() != MPG123_OK)) {
    return ret;
  }
  if ((handle = mpg123_new(nullptr, &ret)) == nullptr) {
    return ret;
  }
  int cnls = 0;
  long _rate = 0;
  if (mpg123_open(handle, path.c_str()) != MPG123_OK ||
      mpg123_getformat(handle, &_rate, &cnls, &encoding)) {
    return ret;
  }
  rate = _rate;
  channels = cnls;
  mpg123_format_none(handle);
  mpg123_format(handle, rate, channels, encoding);
  buf_size = mpg123_outblock(handle) * 16;
  return ret;
}

void Mp3Decoder::Deinit() {
  mpg123_close(handle);
  mpg123_delete(handle);
  mpg123_exit();
}

u32 Mp3Decoder::GetSampleRate() { return rate; }
u8 Mp3Decoder::GetChannels() { return channels; }
u64 Mp3Decoder::Decode(u16* buf_address) {
  size_t done = 0;
  mpg123_read(handle, buf_address, buf_size, &done);
  return done / sizeof(u16);
}
size_t Mp3Decoder::GetFileSamples() {
  off_t len = mpg123_length(handle);
  if (len != MPG123_ERR) {
    return len * size_t(channels);
  }
  return -1;  // NotExist
}
}  // namespace Music
}  // namespace PD