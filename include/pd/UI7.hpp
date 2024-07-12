#pragma once

#include <pd/Image.hpp>
#include <pd/NVec.hpp>
#include <pd/Render2.hpp>
#include <pd/smart_ctor.hpp>

#define UI7MAKEFLAG(x) (1 << x)

typedef int UI7MenuFlags;

enum UI7MenuFlags_ {
  UI7MenuFlags_None = 0,
  UI7MenuFlags_NoTitlebar = UI7MAKEFLAG(0),
  UI7MenuFlags_TitleMid = UI7MAKEFLAG(1),
  UI7MenuFlags_Scrolling = MAKEFLAG(2),
};

class DrawCmd;
class UI7DrawList {
 public:
  UI7DrawList() = default;
  ~UI7DrawList() = default;

  void AddRectangle(NVec2 pos, NVec2 szs, PDColor clr);
  void AddRectangle(NVec2 pos, NVec2 szs, unsigned int clr);
  void AddTriangle(NVec2 pos0, NVec2 pos1, NVec2 pos2, PDColor clr);
  void AddTriangle(NVec2 pos0, NVec2 pos1, NVec2 pos2, unsigned int clr);
  void AddText(NVec2 pos, const std::string &text, PDColor clr,
               PDTextFlags flags = 0, NVec2 box = NVec2());
  void AddText(NVec2 pos, const std::string &text, unsigned int clr,
               PDTextFlags flags = 0, NVec2 box = NVec2());
  void AddImage(NVec2 pos, Palladium::Image::Ref img);
  void AddCall(std::shared_ptr<DrawCmd> cmd);

  void Process(bool auto_clear = true);
  void Clear();

  PD_SMART_CTOR(UI7DrawList)

 private:
  void AddDebugCall(std::shared_ptr<DrawCmd> cmd);
  std::vector<std::shared_ptr<DrawCmd>> list;
};

namespace UI7 {
// Key functions
void Init();
void Deinit();
void Update();
float GetTime();
float GetDeltaTime();
bool &IsDebugging();
// Internal Function
// Should not be used
void Debug();
bool &DebugMenu();

bool Button(const std::string &label, NVec2 size = NVec2(0, 0));
void Checkbox(const std::string &label, bool &c);
void Label(const std::string &label, PDTextFlags flags = 0);
void Progressbar(float value);
/// @brief Draw Image in Menu
/// @param img Pointer f.e to Palladium::Image2
void Image(Palladium::Image::Ref img);
void BrowserList(const std::vector<std::string> &entrys, int &selection,
                 PDTextFlags txtflags = 0, NVec2 size = NVec2(0, 0),
                 int max_entrys = 13);
void InputText(const std::string &label, std::string &text,
               const std::string &hint = "");
bool BeginMenu(const std::string &title, NVec2 size = NVec2(0, 0),
               UI7MenuFlags flags = 0);
void EndMenu();
void Grid(const std::string &name, const NVec2 &size, const NVec2 &entry_size,
          void (*display_func)(void *, NVec2), void **data_array,
          size_t num_entrys);
void ColorSelector(const std::string &label, unsigned int &color);
bool BeginTree(const std::string &text);
void EndTree();
NVec2 GetCursorPos();
void SetCursorPos(NVec2 cp);
void RestoreCursor();
void SameLine();
// Internal API (For Creating Custom Objects)
bool InBox(NVec2 inpos, NVec2 boxpos, NVec2 boxsize);
void MoveCursor(NVec2 size);
bool HandleScrolling(NVec2 &pos, NVec2 size);
bool InMenu();
namespace Menu {
// All of them return the Main BG DrawList if Menu is null
UI7DrawList::Ref GetBackgroundList();
UI7DrawList::Ref GetList();
UI7DrawList::Ref GetForegroundList();
// Other Menu Specific Functions
float GetScrollingOffset();
void SetScrollingOffset(float off);
bool IsScrolling();
}  // namespace Menu
// DrawLists
UI7DrawList::Ref GetForegroundList();
UI7DrawList::Ref GetBackgroundList();
}  // namespace UI7
