#pragma once

#include <pd/common/common.hpp>
#include <pd/common/strings.hpp>
#include <pd/controls/hid.hpp>
#include <pd/maths/vec.hpp>
#include <pd/ui7/drawlist.hpp>

namespace PD {
namespace UI7 {
class Container : public SmartCtor<Container> {
 public:
  Container() {}
  Container(const vec2& pos, const vec2& size) : pos(pos), size(size) {}
  Container(const vec4& box) : pos(box.xy()), size(box.zw()) {}
  ~Container() {}

  void Init(LI::Renderer::Ref r, UI7::DrawList::Ref l, UI7::Theme* lt) {
    list = l;
    linked_theme = lt;
    ren = r;
  }

  void SetPos(const vec2& pos) { this->pos = pos; }
  void SetSize(const vec2& size) { this->size = size; }

  vec2 GetPos() { return pos; }
  vec2 GetSize() { return size; }

  bool Skippable() const { return skippable; }
  bool Removable() const { return rem; }

  void HandleScrolling(vec2 scrolling, vec4 viewport);
  virtual void HandleInput(Hid::Ref inp) {}
  virtual void Draw() {}

  void UnlockInput() { inp_done = false; }

  u32 GetID() const { return id; }
  void SetID(u32 id) { this->id = id; }

 protected:
  /// used to skip Input/Render preocessing ot not
  bool skippable = false;
  bool rem = false;
  u64 last_use = 0;
  bool inp_done = false;
  Screen::Ref screen;
  vec2 pos;
  vec2 size;
  UI7::DrawList::Ref list;
  UI7::Theme* linked_theme;
  LI::Renderer::Ref ren;
  u32 id = 0;
};
}  // namespace UI7
}  // namespace PD
