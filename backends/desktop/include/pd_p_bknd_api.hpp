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

#pragma once

#ifdef PD_BKND_DESKTOP_STATIC
#if defined(__3DS__)  // 3ds Specific
#error "Desktop backend is not supported on the 3ds!"
#endif
#define PD_BKND_DESKTOP_API
#else
#ifdef _WIN32  // Windows (MSVC Tested)
#ifdef PD_BKND_DESKTOP_BUILD_SHARED
#define PD_BKND_DESKTOP_API __declspec(dllexport)
#else
#define PD_BKND_DESKTOP_API __declspec(dllimport)
#endif
#elif defined(__APPLE__)  // macOS (untested yet)
#ifdef PD_BKND_DESKTOP_BUILD_SHARED
#define PD_BKND_DESKTOP_API __attribute__((visibility("default")))
#else
#define PD_BKND_DESKTOP_API
#endif
#elif defined(__linux__)  // Linux (untested yet)
#ifdef PD_BKND_DESKTOP_BUILD_SHARED
#define PD_BKND_DESKTOP_API __attribute__((visibility("default")))
#else
#define PD_BKND_DESKTOP_API
#endif
#elif defined(__3DS__)  // 3ds Specific
#error "Desktop backend is not supported on the 3ds!"
#else
#define PD_BKND_DESKTOP_API
#endif
#endif