#pragma once

#include <pd/core/core.hpp>
#include <pd/lithium/rect.hpp>

namespace PD {
using TextureID = ptr;
enum class TextureFilter {
  Linear,
  Nearest,
};

enum class TextureFormat {
  RGBA32,
  RGB24,
  A8,
};
namespace Li {
class Texture {
 public:
  Texture() : pID(0), pSize(0, 0), pUV(fvec4(0, 0, 1, 1)) {}
  Texture(TextureID id, ivec2 size)
      : pID(id), pSize(size), pUV(fvec4(0, 0, 1, 1)) {}

  const ivec2& GetSize() const { return pSize; }
  TextureID GetID() { return pID; }
  const TextureID& GetID() const { return pID; }
  const Rect& GetUV() const { return pUV; }

  void SetSize(int x, int y) {
    pSize.x = x;
    pSize.y = y;
  }
  void SetSize(const ivec2& size) { pSize = size; }
  void SetID(TextureID id) { pID = id; }
  void SetUV(const Rect& uv) { pUV = uv; }
  void SetUV(const fvec4& uv) { pUV = uv; }
  void SetUV(float t, float l, float b, float r) { SetUV(fvec4(t, l, b, r)); }

 private:
  TextureID pID;
  ivec2 pSize;
  Rect pUV;
};
}  // namespace Li
}  // namespace PD