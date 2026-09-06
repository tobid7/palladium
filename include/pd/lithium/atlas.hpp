#pragma once

#include <pd/core/core.hpp>
#include <pd/lithium/texture.hpp>

namespace PD {
namespace Li {
enum class AtlasState {
  Invalid,
  ReqCreate,
  ReqUpdate,
  ReqDestroy,
};

class Atlas {
 public:
  Atlas() {}
  ~Atlas() {}

 private:
  AtlasState pState = AtlasState::Invalid;
  TextureID pID;
};
}  // namespace Li
}  // namespace PD