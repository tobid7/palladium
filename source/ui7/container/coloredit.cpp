/*
MIT License
Copyright (c) 2024 - 2026 René Amthor (tobid7)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
 */

#include <algorithm>
#include <cmath>
#include <pd/ui7/containers.hpp>

namespace PD {
namespace UI7 {
static Color Hsv(float h, float s, float v, u8 a = 255) {
  float r = v, g = v, b = v;
  if (s > 0.f) {
    float hh = std::fmod(h, 1.f) * 6.f;
    int i = (int)hh;
    float f = hh - i;
    float p = v * (1.f - s);
    float q = v * (1.f - s * f);
    float t = v * (1.f - s * (1.f - f));
    switch (i) {
      case 0: r = v; g = t; b = p; break;
      case 1: r = q; g = v; b = p; break;
      case 2: r = p; g = v; b = t; break;
      case 3: r = p; g = q; b = v; break;
      case 4: r = t; g = p; b = v; break;
      default: r = v; g = p; b = q; break;
    }
  }
  return Color((int)(r * 255.f + 0.5f), (int)(g * 255.f + 0.5f),
               (int)(b * 255.f + 0.5f), (int)a);
}

static void Checker(Li::Drawlist* l, const fvec4& r, float c) {
  l->DrawRectFilled(fvec2(r.x, r.y), fvec2(r.z, r.w), Color(204, 204, 204));
  c = std::max(c, 2.f);
  for (float y = 0.f; y < r.w; y += c) {
    for (float x = 0.f; x < r.z; x += c) {
      if ((((int)(x / c)) + ((int)(y / c))) & 1) continue;
      l->DrawRectFilled(fvec2(r.x + x, r.y + y),
                        fvec2(std::min(c, r.z - x), std::min(c, r.w - y)),
                        Color(128, 128, 128));
    }
  }
}

static fvec2 Pos(const fvec4& r) { return fvec2(r.x, r.y); }
static fvec2 Size(const fvec4& r) { return fvec2(r.z, r.w); }

static void MarkHz(Li::Drawlist* l, const fvec4& r, float y) {
  y = std::round(y);
  l->DrawRectFilled(fvec2(r.x - 1.f, y - 2.f), fvec2(r.z + 2.f, 4.f),
                    Color(0, 0, 0));
  l->DrawRectFilled(fvec2(r.x, y - 1.f), fvec2(r.z, 2.f),
                    Color(255, 255, 255));
}

static void MarkVt(Li::Drawlist* l, const fvec4& r, float x) {
  x = std::round(x);
  l->DrawRectFilled(fvec2(x - 2.f, r.y - 1.f), fvec2(4.f, r.w + 2.f),
                    Color(0, 0, 0));
  l->DrawRectFilled(fvec2(x - 1.f, r.y), fvec2(2.f, r.w),
                    Color(255, 255, 255));
}

static const char* sTabs[2] = {"HSV", "RGBA"};

/** ColorEdit */

PD_API void ColorEdit::HandleInput() {
  if (inp_done) {
    return;
  }
  if (io->InputHandler.DragObject(this->GetID(), fvec4(FinalPos(), size))) {
    if (io->InputHandler.DragReleased) {
      open_req = true;
    }
  }
  inp_done = true;
}

PD_API void ColorEdit::Draw() {
  list->SetFont(GetFont());
  float rh = io->ItemRowHeight;
  fvec2 p = FinalPos();
  Color c(*color_ref);
  if (c.a != 255) {
    Checker(list, fvec4(p, fvec2(rh)), std::floor(rh * 0.5f));
  }
  list->PathRect(p, p + rh, io->FrameRounding);
  list->PathFill(c, LiDrawFlags_AA);
  bool held = io->InputHandler.IsHeld(this->GetID());
  if (held || io->InputHandler.IsHovered(this->GetID(), fvec4(p, size))) {
    list->PathRect(p + 0.5f, p + rh - 0.5f, io->FrameRounding);
    list->PathStroke(io->Theme.Get(held ? UI7Color_Text : UI7Color_Border), 1,
                     LiDrawFlags_Close | LiDrawFlags_AA);
  }
  list->LayerUp();
  list->DrawText(p + fvec2(rh + io->ItemSpace.x, 0), label.c_str(),
                 io->Theme.Get(UI7Color_Text));
  list->LayerDown();
}

PD_API void ColorEdit::Update() {
  this->SetSize(
      fvec2(tdim.x + io->ItemSpace.x + io->ItemRowHeight, io->ItemRowHeight));
}

/** ColorPicker */

PD_API void ColorPicker::pSync() {
  if (synced && *color_ref == last) {
    return;
  }
  Color c(*color_ref);
  float r = c.r / 255.f, g = c.g / 255.f, b = c.b / 255.f;
  float mx = std::max({r, g, b});
  float mn = std::min({r, g, b});
  float d = mx - mn;
  v = mx;
  if (mx > 0.f) {
    s = d / mx;
  }
  if (d > 0.f) {
    if (mx == r) {
      h = (g - b) / d;
    } else if (mx == g) {
      h = 2.f + (b - r) / d;
    } else {
      h = 4.f + (r - g) / d;
    }
    h /= 6.f;
    if (h < 0.f) h += 1.f;
  }
  last = *color_ref;
  synced = true;
}

PD_API void ColorPicker::pApplyHsv() {
  *color_ref = Hsv(h, s, v, Color(*color_ref).a);
  last = *color_ref;
}

PD_API void ColorPicker::pCalc() {
  float rh = io->ItemRowHeight;
  fvec2 g = io->ItemSpace;
  float sq = std::round(rh * 7.f);
  float bw = std::round(rh * 0.8f);
  float w = sq + (g.x + bw) * 2.f;
  fvec2 p = FinalPos();
  float x = p.x;
  for (int i = 0; i < 2; i++) {
    float tw = io->Font->GetTextBounds(sTabs[i], io->FontScale).x +
               io->FramePadding.x * 2.f;
    rTab[i] = fvec4(x, p.y, tw, rh);
    x += tw + g.x;
  }
  float y = p.y + rh + g.y;
  rSV = fvec4(p.x, y, sq, sq);
  rHue = fvec4(p.x + sq + g.x, y, bw, sq);
  rAlpha = fvec4(rHue.x + bw + g.x, y, bw, sq);
  vw = io->Font->GetTextBounds("000", io->FontScale).x + g.x;
  for (int i = 0; i < 4; i++) {
    rBar[i] = fvec4(p.x, y + i * (rh + g.y), w - vw, rh);
  }
  y += (io->ColorPickerMode ? rh * 4.f + g.y * 3.f : sq) + g.y;
  rNew = fvec4(p.x, y, rh * 2.f, rh);
  rOld = fvec4(p.x + rNew.z, y, backup ? rh * 2.f : 0.f, rh);
  pHex = fvec2(rOld.x + rOld.z + g.x, y);
  size = fvec2(w, y + rh - p.y);
}

PD_API void ColorPicker::HandleInput() {
  if (inp_done) {
    return;
  }
  pSync();
  pCalc();
  auto& ih = io->InputHandler;
  u32 id = this->GetID();
  for (int i = 0; i < 2; i++) {
    if (ih.DragObject(id + 8 + i, rTab[i]) && ih.DragReleased) {
      io->ColorPickerMode = i;
    }
  }
  if (backup && ih.DragObject(id + 10, rOld) && ih.DragReleased) {
    *color_ref = *backup;
    pSync();
  }
  fvec2 m = PD::Hid::MousePos();
  if (io->ColorPickerMode == 0) {
    ih.DragObject(id + 1, rSV);
    ih.DragObject(id + 2, rHue);
    ih.DragObject(id + 3, rAlpha);
    if (ih.IsHeld(id + 1)) {
      s = std::clamp((m.x - rSV.x) / rSV.z, 0.f, 1.f);
      v = 1.f - std::clamp((m.y - rSV.y) / rSV.w, 0.f, 1.f);
      pApplyHsv();
    }
    if (ih.IsHeld(id + 2)) {
      h = std::clamp((m.y - rHue.y) / rHue.w, 0.f, 1.f);
      pApplyHsv();
    }
    if (ih.IsHeld(id + 3)) {
      Color c(*color_ref);
      float a = 1.f - std::clamp((m.y - rAlpha.y) / rAlpha.w, 0.f, 1.f);
      c.a = (u8)std::round(a * 255.f);
      *color_ref = c;
      last = *color_ref;
    }
  } else {
    for (int i = 0; i < 4; i++) {
      ih.DragObject(id + 4 + i, rBar[i]);
      if (ih.IsHeld(id + 4 + i)) {
        Color c(*color_ref);
        u8* ch[4] = {&c.r, &c.g, &c.b, &c.a};
        float t = std::clamp((m.x - rBar[i].x) / rBar[i].z, 0.f, 1.f);
        *ch[i] = (u8)std::round(t * 255.f);
        *color_ref = c;
        pSync();
      }
    }
  }
  inp_done = true;
}

PD_API void ColorPicker::Draw() {
  list->SetFont(GetFont());
  list->UnbindTexture();
  pSync();
  pCalc();
  auto& ih = io->InputHandler;
  auto& th = io->Theme;
  u32 id = this->GetID();
  Color c(*color_ref);
  Color o = c;
  o.a = 255;

  for (int i = 0; i < 2; i++) {
    UI7Color tc = UI7Color_Button;
    if (io->ColorPickerMode == i || ih.IsHeld(id + 8 + i)) {
      tc = UI7Color_ButtonActive;
    } else if (ih.IsHovered(id + 8 + i, rTab[i])) {
      tc = UI7Color_ButtonHovered;
    }
    list->PathRect(Pos(rTab[i]), Pos(rTab[i]) + Size(rTab[i]),
                   io->FrameRounding);
    list->PathFill(th.Get(tc), LiDrawFlags_AA);
  }

  if (io->ColorPickerMode == 0) {
    Color hc = Hsv(h, 1.f, 1.f);
    list->DrawRectFilledMultiColor(Pos(rSV), Size(rSV), Color(255, 255, 255),
                                   hc, hc, Color(255, 255, 255));
    list->DrawRectFilledMultiColor(Pos(rSV), Size(rSV), Color(0, 0, 0, 0),
                                   Color(0, 0, 0, 0), Color(0, 0, 0),
                                   Color(0, 0, 0));
    fvec2 sp(rSV.x + s * rSV.z, rSV.y + (1.f - v) * rSV.w);
    fvec4 cr = rSV;
    if (pCLipRectUsed) {
      float x0 = std::max(cr.x, pClipRect.x);
      float y0 = std::max(cr.y, pClipRect.y);
      float x1 = std::min(cr.x + cr.z, pClipRect.x + pClipRect.z);
      float y1 = std::min(cr.y + cr.w, pClipRect.y + pClipRect.w);
      cr = fvec4(x0, y0, std::max(x1 - x0, 0.f), std::max(y1 - y0, 0.f));
    }
    list->PushClipRect(cr);
    list->DrawCircle(sp, 5.f, Color(0, 0, 0), 16, 2, LiDrawFlags_AA);
    list->DrawCircle(sp, 4.f, Color(255, 255, 255), 16, 1, LiDrawFlags_AA);
    list->PopClipRect();

    float sg = rHue.w / 6.f;
    for (int i = 0; i < 6; i++) {
      Color a = Hsv(i / 6.f, 1.f, 1.f);
      Color b = Hsv((i + 1) / 6.f, 1.f, 1.f);
      list->DrawRectFilledMultiColor(fvec2(rHue.x, rHue.y + i * sg),
                                     fvec2(rHue.z, sg), a, a, b, b);
    }
    MarkHz(list, rHue, rHue.y + h * rHue.w);

    Checker(list, rAlpha, std::floor(rAlpha.z * 0.5f));
    Color t = o;
    t.a = 0;
    list->DrawRectFilledMultiColor(Pos(rAlpha), Size(rAlpha), o, o, t, t);
    MarkHz(list, rAlpha, rAlpha.y + (1.f - c.a / 255.f) * rAlpha.w);
  } else {
    u8 cv[4] = {c.r, c.g, c.b, c.a};
    for (int i = 0; i < 4; i++) {
      Color lo = i == 3 ? c : o;
      Color hi = lo;
      u8* ll[4] = {&lo.r, &lo.g, &lo.b, &lo.a};
      u8* hh[4] = {&hi.r, &hi.g, &hi.b, &hi.a};
      *ll[i] = 0;
      *hh[i] = 255;
      if (i == 3) {
        Checker(list, rBar[i], std::floor(rBar[i].w * 0.5f));
      }
      list->DrawRectFilledMultiColor(Pos(rBar[i]), Size(rBar[i]), lo, hi, hi,
                                     lo);
      MarkVt(list, rBar[i], rBar[i].x + cv[i] / 255.f * rBar[i].z);
    }
  }

  Checker(list, rNew, std::floor(rNew.w * 0.5f));
  list->DrawRectFilled(Pos(rNew), Size(rNew), c);
  if (backup) {
    Checker(list, rOld, std::floor(rOld.w * 0.5f));
    list->DrawRectFilled(Pos(rOld), Size(rOld), *backup);
    bool held = ih.IsHeld(id + 10);
    if (held || ih.IsHovered(id + 10, rOld)) {
      list->DrawRect(Pos(rOld) + 0.5f, Size(rOld) - 1.f,
                     th.Get(held ? UI7Color_Text : UI7Color_Border));
    }
  }

  list->LayerUp();
  for (int i = 0; i < 2; i++) {
    list->DrawTextEx(Pos(rTab[i]), sTabs[i], th.Get(UI7Color_Text),
                     LiTextFlags_AlignMid, Size(rTab[i]));
  }
  if (io->ColorPickerMode == 1) {
    u8 cv[4] = {c.r, c.g, c.b, c.a};
    for (int i = 0; i < 4; i++) {
      list->DrawText(fvec2(rBar[i].x + rBar[i].z + io->ItemSpace.x, rBar[i].y),
                     std::to_string(cv[i]).c_str(), th.Get(UI7Color_Text));
    }
  }
  list->DrawText(pHex, c.Hex(true).c_str(), th.Get(UI7Color_Text));
  list->LayerDown();
}

PD_API void ColorPicker::Update() { pCalc(); }
}  // namespace UI7
}  // namespace PD
