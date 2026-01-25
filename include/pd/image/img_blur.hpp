#pragma once

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

#include <pd/core/core.hpp>
#include <pd/image/pd_p_api.hpp>

namespace PD {
/**
 * Namepace containing functions to blur images
 */
namespace ImgBlur {
/**
 * Function to create Gaussian Kernel List
 * @param radius Rasius to use
 * @param si sigma value to use
 * @return list of kernel values
 */
PD_IMAGE_API std::vector<float> GaussianKernel(int radius, float si);
/**
 * Gaussian Blur for basic Image Buffer
 * @param buf Image Buffer (unsigned char)
 * @param w width of the image
 * @param h width of the image
 * @param radius Blur radius
 * @param si Blur sigma
 * @param idxfn Indexing function
 */
PD_IMAGE_API void GaussianBlur(
    std::vector<u8>& buf, int w, int h, float radius, float si,
    std::function<int(int, int, int)> idxfn = [](int x, int y, int w) -> int {
      return y * w + x;
    });
/**
 * Advanced func to access memory directly
 * @param buf Referenvce to the buffer
 * @param w width of the image
 * @param h width of the image
 * @param bpp Bytes per Pixels (RGB[A], RGB565, etc)
 * @param radius Blur radius
 * @param si Blur sigma
 * @param idxfn Indexing function
 */
PD_IMAGE_API void GaussianBlur(
    void* buf, int w, int h, int bpp, float radius, float si,
    std::function<int(int, int, int)> idxfn = [](int x, int y, int w) -> int {
      return y * w + x;
    });
}  // namespace ImgBlur
}  // namespace PD