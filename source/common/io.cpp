#include <pd/common/io.hpp>

namespace PD {
namespace IO {
std::vector<u8> LoadFile2Mem(const std::string& path) {
  std::ifstream iff(path, std::ios::binary);
  if (!iff) {
    return std::vector<u8>();
  }
  iff.seekg(0, std::ios::end);
  size_t szs = iff.tellg();
  iff.seekg(0, std::ios::beg);
  std::vector<u8> res(szs, 0);
  iff.read(reinterpret_cast<char*>(res.data()), res.size());
  iff.close();
  return res;
}
u32 HashMemory(const std::vector<u8>& data) {
  u32 hash = 4477;
  for (auto& it : data) {
    hash = (hash * 33) + it;
  }
  return hash;
}
}  // namespace IO
}  // namespace PD