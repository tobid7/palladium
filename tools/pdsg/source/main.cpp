#include <format>
#include <fstream>
#include <iostream>
#include <spirv-helper.hpp>
#include <sstream>

template <typename... Args>
void FmtPrint(std::format_string<Args...> fmt, Args&&... args) {
  std::cout << std::format(fmt, std::forward<Args>(args)...);
}

std::string LoadFile(const std::string& path) {
  std::stringstream s;
  std::ifstream f(path);
  s << f.rdbuf();
  return s.str();
}

int main(int argc, char** argv) {
  if (argc != 4) return 0;
  SpirvHelper::Stage mode = SpirvHelper::Stage::Vertex;
  if (std::string(argv[3]) == "vertex") {
    mode = SpirvHelper::Stage::Vertex;
  } else if (std::string(argv[3]) == "fragment") {
    mode = SpirvHelper::Stage::Fragment;
  } else if (std::string(argv[3]) == "geometry") {
    mode = SpirvHelper::Stage::Geometry;
  }
  FmtPrint("ProcessingMode: {}\n", (int)mode);
  FmtPrint("Loading {}\n", argv[1]);
  std::string shader = LoadFile(argv[1]);
  FmtPrint("Generating SPIRV...\n");
  auto res = SpirvHelper::GLSL2SPV(SpirvHelper::Stage::Vertex, shader.c_str());
  FmtPrint("Result size: {}\n", res.size());
  std::ofstream ff(argv[2]);
  FmtPrint("Writing into {}", argv[2]);
  ff << "#pragma once\n\n#include <cinttypes>\n\n// clang-format off\nconst "
        "unsigned int pShader[] "
        "= { ";
  for (auto& it : res) {
    ff << std::format("0x{:X}, ", it);
  }
  ff << "};\n// clang-format on\n";
  return 0;
}