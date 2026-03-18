#pragma once

#include <cstdint>
#include <string>
#include <vector>

struct TBuiltInResource;
class SpirvHelper {
 public:
  enum class Format {
    GLSL,
    HLSL,
    MSL,  // Not supported yet
    SPIRV,
  };

  enum class Stage {
    Vertex = 0,
    Geometry = 3,
    Fragment = 4,
  };

  SpirvHelper() = default;
  ~SpirvHelper() = default;

  static void Init();
  static void Finalize();
  static void SetupResources(TBuiltInResource& resources);
  static std::vector<unsigned int> GLSL2SPV(Stage stage, const char* code);
  static std::string SPV2GLSL(const std::vector<unsigned int>& spirv,
                              int version, bool es = false);
  static std::string SPV2HLSL(const std::vector<unsigned int>& spirv,
                              int version);
};