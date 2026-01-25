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
/**
 * Set of File Functions
 */
namespace IO {
/**
 * Load a File into an 8Bit Memory Buffer
 * @param path Path to the File
 * @return 8Bit FileBuffer
 */
PD_API std::vector<u8> LoadFile2Mem(const std::string& path);
/**
 * Load a File into a std::string
 * @param path Path to the File
 * @return std::string file content
 */
PD_API std::string LoadFile2Str(const std::string& path);
/**
 * Hash a 8Bit Memory Buffer
 * @param data 8Bit input Buffer
 * @return 32Bit Hash
 */
PD_API u32 HashMemory(const std::vector<u8>& data);
/**
 * Function to decrompress RLE buffer
 * @param data Data buffer to decompress
 */
PD_API void DecompressRLE(std::vector<u8>& data);
/**
 * Function to compress data with RLE Algorithm
 * @param data Data buf
 */
PD_API void CompressRLE(std::vector<u8>& data);
}  // namespace IO
}  // namespace PD