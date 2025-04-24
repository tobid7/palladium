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

// Core
#include <pd/core/core.hpp>
// Image
#include <pd/image/image.hpp>
#include <pd/image/img_blur.hpp>
#include <pd/image/img_convert.hpp>
#include <pd/ui7/ui7.hpp>
// Net
#include <pd/net/backend.hpp>
#include <pd/net/socket.hpp>

/// Setup these as non Namespaced access by default
#ifndef PD_MATH_NAMESPACED
using fvec2 = PD::fvec2;
using fvec3 = PD::fvec3;
using fvec4 = PD::fvec4;
using dvec2 = PD::dvec2;
using dvec3 = PD::dvec3;
using dvec4 = PD::dvec4;
using ivec2 = PD::ivec2;
using ivec3 = PD::ivec3;
using ivec4 = PD::ivec4;
#endif

// namespace Palladium = PD;