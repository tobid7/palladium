#pragma once

#include <functional>
#include <pd/lithium/lithium.hpp>
#include <pd/ultra/flags.hpp>

namespace PD {
namespace Ultra {
class Canvas;
class Container;
using EventFunc = std::function<void()>;
class PD_API ElementBase {
 public:
  ElementBase() {}
  ~ElementBase() {}

  virtual void Draw(PD::Li::Drawlist& dl) = 0;
  /**
   * Reset Function (for PD::Pool::FastReset)
   */
  virtual void Reset() {}
  virtual void Update();

  void SetAlignment(UltraAlignment a) { pAlignment = a; }
  void SetPosition(const PD::fvec2& pos) { pPos = pos; }
  void SetPosition(float x, float y) { pPos = PD::fvec2(x, y); }
  void SetSize(const PD::fvec2& size) { pSize = size; }
  void SetSize(float w, float h) { pSize = PD::fvec2(w, h); }
  /**
   * Executed on Hovering
   * Elemnents can override / discard this func
   */
  virtual void OnFocus(EventFunc func) { pHover = func; }
  /**
   * Executed on Mocing out of the space
   * Elemnents can override / discard this func
   */
  virtual void OnUnFocus(EventFunc func) { pUnHover = func; }
  /**
   * Executrd on KeyUp event
   * Elemnents can override / discard this func
   */
  virtual void OnPress(EventFunc func) { pPress = func; }

  void SetFontScale(float s = 1.f) { pFontScale = 1.f; }
  float GetFontScale() const { return pFontScale; }

  void SetFont(PD::Li::Font& font) { pFont = &font; }

  void SetFontIfNull(PD::Li::Font& font) {
    if (!pFont) pFont = &font;
  }

  virtual void UpdateInput();

 protected:
  friend class Container;
  void SetParent(Container* c) { pParent = c; }
  bool RevisionUpdate(PD::u32 req);
  Container* pParent = nullptr;
  PD::u32 pCanvasRev = 0;
  UltraAlignment pAlignment = 0;
  PD::fvec2 pPos;
  PD::fvec2 pSize;
  PD::Li::Rect pRenderspace;
  EventFunc pHover = nullptr;
  EventFunc pUnHover = nullptr;
  EventFunc pPress = nullptr;
  bool pFocued = false;
  // Not used by every object btw
  PD::Li::Font* pFont = nullptr;
  float pFontScale = 1.f;
};
}  // namespace Ultra
}  // namespace PD