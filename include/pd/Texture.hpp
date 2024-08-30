#pragma once
#include <citro3d.h>

#include <pd/maths/NVec.hpp>
#include <pd/smart_ctor.hpp>
#include <string>
#include <vector>

namespace Palladium {
class Texture {
 public:
  // Define Types supported by the texture loader
  // For example usefull to waste not as much space
  // in linear mem
  enum Type {
    RGBA32,
    RGB24,
    A8,
  };
  Texture() {
    // Set Default UV
    this->uvs[0] = 0.0f;
    this->uvs[1] = 1.0f;
    this->uvs[2] = 1.0f;
    this->uvs[3] = 0.0f;
  };
  ~Texture() { Delete(); }
  PD_SMART_CTOR(Texture)

  void Delete();

  /// @brief Loads an Image [png, jpg, bmp] as texture
  /// by default it sets up the texture as RGBA32 and
  /// only supports RGB24 and RGBA32 images
  /// @param path Path to file
  void LoadFile(const std::string& path);
  /// @brief Loads an Image [png, jpg, bmp] as texture
  /// by default it sets up the texture as RGBA32 and
  /// only supports RGB24 and RGBA32 images
  /// @param data Data of file
  void LoadFromMemory(const std::vector<unsigned char>& data);
  /// @brief Create Texture by pixel Data. This function supports
  /// [RGBA32, RGB24, A8]
  /// @param data Pixel Data
  /// @param w Width of data
  /// @param h Height of Data
  /// @param type Type of Data (default is RGBA32)
  void LoadPixels(const std::vector<unsigned char>& data, int w, int h, Type type = RGBA32);

  /// @brief This function sets up a texture Object based on the input
  /// Data and a self setup C3D_Tex. You dont need to delete it as 
  /// This class does this automatically
  void ExternalLoad(C3D_Tex* tex, NVec2 rszs, NVec4 uvs);

  C3D_Tex* Get() { return this->tex; }
  NVec2 GetTexSize();
  NVec2 GetSize() { return img_size; }
  // As the texture is a pow of 2 we need a uv
  NVec4 GetUV() { return uvs; }

  void AutoDelete(bool enable) { ad = enable; }

 private:
  void MakeTex(std::vector<unsigned char>& buf, int w, int h, Type type = RGBA32);
  C3D_Tex* tex = nullptr;
  NVec2 img_size;
  NVec4 uvs;
  bool ad = true;
};
}  // namespace Palladium