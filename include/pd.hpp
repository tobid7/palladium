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

// Common
#include <pd/common/app.hpp>
#include <pd/common/lang.hpp>
#include <pd/common/strings.hpp>
#include <pd/common/sys.hpp>
#include <pd/common/timetrace.hpp>
// Graphics
#include <pd/graphics/lithium.hpp>
#include <pd/graphics/spritesheet.hpp>
#include <pd/graphics/texture.hpp>
// Maths
#include <pd/maths/bit_util.hpp>
#include <pd/maths/color.hpp>
#include <pd/maths/img_blur.hpp>
#include <pd/maths/img_convert.hpp>
#include <pd/maths/vec.hpp>
// Overlays
#include <pd/overlays/keyboard.hpp>
#include <pd/overlays/message_mgr.hpp>
#include <pd/overlays/overlay_mgr.hpp>
#include <pd/overlays/performance.hpp>
#include <pd/overlays/settings.hpp>
// UI7
#include <pd/ui7/ui7.hpp>

/// Setup these as non Namespaced access by default
#ifndef PD_MATH_NAMESPACED
using vec2 = PD::vec2;
using vec3 = PD::vec3;
using vec4 = PD::vec4;
#endif

// namespace Palladium = PD;