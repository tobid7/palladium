#pragma once

#include <pd/common/common.hpp>

namespace PD {
namespace IO {
std::vector<u8> LoadFile2Mem(const std::string& path);
u32 HashMemory(const std::vector<u8>& data);
}  // namespace IO
}  // namespace PD