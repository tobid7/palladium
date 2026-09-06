#pragma once

#include <pd/common.hpp>

struct TBuiltInResource;
namespace PD {
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
  static std::vector<PD::u32> GLSL2SPV(Stage stage, const char* code,
                                       bool vulkan_mode = false);
  static std::string SPV2GLSL(const std::vector<PD::u32>& spirv, int version,
                              bool es = false);
  static std::string SPV2HLSL(const std::vector<PD::u32>& spirv, int version);
};
}  // namespace PD