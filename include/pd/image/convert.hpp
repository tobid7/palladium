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
#include <pd/image/image.hpp>
#include <pd/pd_p_api.hpp>

namespace PD {
/**
 * Namespace containing function to convert images
 */
namespace ImgConvert {
/**
 * Convert RGB24 to RGBA32 by adding a 4th alpha value set to 255
 * to every pixel
 * @param out Result List
 * @param in Input Buffer List (rgb24)
 * @param w width of the image
 * @param h height of the image
 */
PD_API
void RGB24toRGBA32(std::vector<PD::u8>& out, const std::vector<u8>& in,
                   const int& w, const int& h);
PD_API
void RGB32toRGBA24(std::vector<u8>& out, const std::vector<u8>& in,
                   const int& w, const int& h);
/**
 * Reverse 32 (RGBA -> ABGR || ABGR -> RGBA)
 * @param buf Buffer to convert
 * @param w width
 * @param h height
 */
PD_API void Reverse32(std::vector<u8>& buf, const int& w, const int& h);
PD_API void ReverseBuf(std::vector<u8>& buf, size_t bpp, int w, int h);
}  // namespace ImgConvert
}  // namespace PD