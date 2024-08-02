#include <ctime>
#include <pd/Color.hpp>
#include <pd/Hid.hpp>
#include <pd/Message.hpp>
#include <pd/Overlays.hpp>
#include <pd/UI7.hpp>
#include <pd/internal_db.hpp>
#include <pd/palladium.hpp>
#include <unordered_map>

template <typename T>
inline T d7max(T a, T b) {
  return a > b ? a : b;
}

template <typename T>
inline T d7min(T a, T b) {
  return a < b ? a : b;
}

// As the 3ds doesn't support std::chrono
#ifdef __3DS__
/// @brief 3ds System Ticks per milli second
/// Already defined in FTrace ik
#define TICKS_PER_MSEC 268111.856
#include <3ds.h>
#define __get_time() (float)svcGetSystemTick() / (float)TICKS_PER_MSEC
#else
#include <chrono>
#define __get_time()                                        \
  std::chrono::duration_cast<std::chrono::milliseconds>(    \
      std::chrono::system_clock().now().time_since_epoch()) \
      .count()
#endif

// Basement structs enums etc
struct UI7ID {
  UI7ID(const std::string &id) {
    real_id = id;
    title = id;
    if (id.substr(0, 2) == "##") {
      title = "";
      has_title = false;
    }
    // Ensure the id is lowercase
    std::transform(real_id.begin(), real_id.end(), real_id.begin(),
                   [](unsigned char c) { return std::tolower(c); });
  }
  UI7ID() {
    title = "";
    real_id = "";
    has_title = false;
  }

  std::string Title() { return title; }

  std::string ID() { return real_id; }

  bool operator==(const UI7ID &in) { return (real_id == in.real_id); }

  bool operator!=(const UI7ID &in) { return !(*this == in); }

  std::string title;
  std::string real_id;
  bool has_title;
};

// Non Touch Control
struct NTCtrl {
  // 0x0 = Obj
  // 0x1 = Selector
  // 0x2 = fill obj
  std::vector<unsigned char> grid;
  std::vector<unsigned char> cgrid;
  int hz = 0;   // Horizontal maximum obj
  int vt = 0;   // Vertical maximum Obj
  int chz = 0;  // current hz
  int selection = 0;
  void AddObj() {
    chz++;
    if (chz > hz) hz = chz;
    grid.push_back(0x0);
  }
  void NewRow() {
    chz = 0;
    vt++;
  }
  void Clear() {
    cgrid = grid;
    grid.clear();
  }
  PD_SMART_CTOR(NTCtrl)
};

using DrawCmdType = int;
enum DrawCmdType_ {
  DrawCmdType_Skip,
  DrawCmdType_Rect,
  DrawCmdType_Triangle,
  DrawCmdType_Text,
  DrawCmdType_Image,
  DrawCmdType_Debug,
};

void UI7CtxPushDebugCmd(std::shared_ptr<DrawCmd> ref);

class DrawCmd {
 public:
  // Empty Command
  DrawCmd() {}

  // Process the Command
  void Process() {
    if (type == DrawCmdType_Skip) {
      return;
    }
    Palladium::R2::OnScreen(screen ? R2Screen_Top : R2Screen_Bottom);
    if (type == DrawCmdType_Rect) {
      Palladium::R2::AddRect(NVec2(rect.x, rect.y), NVec2(rect.z, rect.w), clr);
    } else if (type == DrawCmdType_Triangle) {
      Palladium::R2::AddTriangle(NVec2(rect.x, rect.y), NVec2(rect.z, rect.w),
                                 add_coords, clr);
    } else if (type == DrawCmdType_Text) {
      Palladium::R2::AddText(NVec2(rect.x, rect.y), text, clr, text_flags,
                             text_box);
    } else if (type == DrawCmdType_Image) {
      Palladium::R2::AddImage(NVec2(rect.x, rect.y), img);
    } else if (type == DrawCmdType_Debug) {
      Debug();
    }
  }
  void Debug() {
    Palladium::R2::OnScreen(screen ? R2Screen_Top : R2Screen_Bottom);
    if (stype == DrawCmdType_Skip && type != DrawCmdType_Debug) return;
    if (stype == DrawCmdType_Rect) {
      Palladium::R2::DrawNextLined();
      Palladium::R2::AddTriangle(NVec2(rect.x, rect.y),
                                 NVec2(rect.x + rect.z, rect.y),
                                 NVec2(rect.x, rect.y + rect.w), 0xff0000ff);
      Palladium::R2::DrawNextLined();
      Palladium::R2::AddTriangle(NVec2(rect.x + rect.z, rect.y + rect.w),
                                 NVec2(rect.x + rect.z, rect.y),
                                 NVec2(rect.x, rect.y + rect.w), 0xff0000ff);
    } else if (stype == DrawCmdType_Triangle) {
      Palladium::R2::DrawNextLined();
      Palladium::R2::AddTriangle(NVec2(rect.x, rect.y), NVec2(rect.z, rect.w),
                                 add_coords, 0xff00ff00);
    } else if (stype == DrawCmdType_Text) {
      auto szs = Palladium::R2::GetTextDimensions(text);
      if (text_flags & PDTextFlags_AlignRight) {
        rect.x -= szs.x;
      }
      Palladium::R2::DrawNextLined();
      Palladium::R2::AddTriangle(NVec2(rect.x, rect.y),
                                 NVec2(rect.x + szs.x, rect.y),
                                 NVec2(rect.x, rect.y + szs.y), 0xff00ffff);
      Palladium::R2::DrawNextLined();
      Palladium::R2::AddTriangle(NVec2(rect.x + szs.x, rect.y + szs.y),
                                 NVec2(rect.x + szs.x, rect.y),
                                 NVec2(rect.x, rect.y + szs.y), 0xff00ffff);
    } else if (stype == DrawCmdType_Image) {
      if (!img) return;
      rect.z = img->GetSize().x;
      rect.w = img->GetSize().y;
      Palladium::R2::DrawNextLined();
      Palladium::R2::AddTriangle(NVec2(rect.x, rect.y),
                                 NVec2(rect.x + rect.z, rect.y),
                                 NVec2(rect.x, rect.y + rect.w), 0xff0000ff);
      Palladium::R2::DrawNextLined();
      Palladium::R2::AddTriangle(NVec2(rect.x + rect.z, rect.y + rect.w),
                                 NVec2(rect.x + rect.z, rect.y),
                                 NVec2(rect.x, rect.y + rect.w), 0xff0000ff);
    }
  }
  PD_SMART_CTOR(DrawCmd)

  NVec4 rect = NVec4();                  // Position / Size
  NVec2 add_coords = NVec2();            // Additional Coords
  unsigned int clr = 0;                  // Color
  std::string text = "";                 // Text
  Palladium::Image::Ref img;             // Image
  DrawCmdType type = DrawCmdType_Skip;   // DrawCmd Type
  DrawCmdType stype = DrawCmdType_Skip;  // Second Type
  PDTextFlags text_flags = 0;            // Flags for Text Rendering
  NVec2 text_box = NVec2();              // Maximum text Box
  bool screen = false;                   // Defines Top or Bottom
};

void UI7DrawList::AddRectangle(NVec2 pos, NVec2 szs, PDColor clr) {
  auto cmd = DrawCmd::New();
  cmd->screen = Palladium::R2::GetCurrentScreen();
  cmd->rect.x = pos.x;
  cmd->rect.y = pos.y;
  cmd->rect.z = szs.x;
  cmd->rect.w = szs.y;
  cmd->clr = Palladium::ThemeActive()->Get(clr);
  cmd->type = DrawCmdType_Rect;
  AddDebugCall(cmd);
  AddCall(cmd);
}

void UI7DrawList::AddRectangle(NVec2 pos, NVec2 szs, unsigned int clr) {
  auto cmd = DrawCmd::New();
  cmd->screen = Palladium::R2::GetCurrentScreen();
  cmd->rect.x = pos.x;
  cmd->rect.y = pos.y;
  cmd->rect.z = szs.x;
  cmd->rect.w = szs.y;
  cmd->clr = clr;
  cmd->type = DrawCmdType_Rect;
  AddDebugCall(cmd);
  AddCall(cmd);
}

void UI7DrawList::AddTriangle(NVec2 pos0, NVec2 pos1, NVec2 pos2, PDColor clr) {
  auto cmd = DrawCmd::New();
  cmd->screen = Palladium::R2::GetCurrentScreen();
  cmd->rect.x = pos0.x;
  cmd->rect.y = pos0.y;
  cmd->rect.z = pos1.x;
  cmd->rect.w = pos1.y;
  cmd->add_coords = pos2;
  cmd->clr = Palladium::ThemeActive()->Get(clr);
  cmd->type = DrawCmdType_Triangle;
  AddDebugCall(cmd);
  AddCall(cmd);
}

void UI7DrawList::AddTriangle(NVec2 pos0, NVec2 pos1, NVec2 pos2,
                              unsigned int clr) {
  auto cmd = DrawCmd::New();
  cmd->screen = Palladium::R2::GetCurrentScreen();
  cmd->rect.x = pos0.x;
  cmd->rect.y = pos0.y;
  cmd->rect.z = pos1.x;
  cmd->rect.w = pos1.y;
  cmd->add_coords = pos2;
  cmd->clr = clr;
  cmd->type = DrawCmdType_Triangle;
  AddDebugCall(cmd);
  AddCall(cmd);
}

void UI7DrawList::AddText(NVec2 pos, const std::string &text, PDColor clr,
                          PDTextFlags flags, NVec2 box) {
  auto cmd = DrawCmd::New();
  cmd->screen = Palladium::R2::GetCurrentScreen();
  cmd->rect.x = pos.x;
  cmd->rect.y = pos.y;
  cmd->text = text;
  cmd->clr = Palladium::ThemeActive()->Get(clr);
  cmd->text_flags = flags;
  cmd->text_box = box;
  cmd->type = DrawCmdType_Text;
  AddDebugCall(cmd);
  AddCall(cmd);
}

void UI7DrawList::AddText(NVec2 pos, const std::string &text, unsigned int clr,
                          PDTextFlags flags, NVec2 box) {
  auto cmd = DrawCmd::New();
  cmd->screen = Palladium::R2::GetCurrentScreen();
  cmd->rect.x = pos.x;
  cmd->rect.y = pos.y;
  cmd->text = text;
  cmd->text_flags = flags;
  cmd->text_box = box;
  cmd->clr = clr;
  cmd->type = DrawCmdType_Text;
  AddDebugCall(cmd);
  AddCall(cmd);
}

void UI7DrawList::AddImage(NVec2 pos, Palladium::Image::Ref img) {
  auto cmd = DrawCmd::New();
  cmd->screen = Palladium::R2::GetCurrentScreen();
  cmd->rect.x = pos.x;
  cmd->rect.y = pos.y;
  cmd->img = img;
  cmd->type = DrawCmdType_Image;
  AddDebugCall(cmd);
  AddCall(cmd);
}

void UI7DrawList::AddCall(std::shared_ptr<DrawCmd> cmd) {
  this->list.push_back(cmd);
}

void UI7DrawList::Process(bool auto_clear) {
  for (auto it : this->list) {
    it->Process();
  }
  if (auto_clear) this->Clear();
}

void UI7DrawList::Clear() { this->list.clear(); }

void UI7DrawList::AddDebugCall(std::shared_ptr<DrawCmd> cmd) {
  auto dcmd = DrawCmd::New();
  dcmd->add_coords = cmd->add_coords;
  dcmd->clr = cmd->clr;
  dcmd->rect = cmd->rect;
  dcmd->stype = cmd->type;
  dcmd->text = cmd->text;
  dcmd->text_box = cmd->text_box;
  dcmd->text_flags = cmd->text_flags;
  dcmd->img = cmd->img;
  dcmd->type = DrawCmdType_Debug;
  dcmd->screen = Palladium::R2::GetCurrentScreen();
  UI7CtxPushDebugCmd(dcmd);
}

struct UI7Menu {
  UI7Menu() {}
  UI7ID menuid;                    // menu ID
  NVec2 cursor;                    // cursor
  NVec2 cb;                        // backup cursor
  NVec2 slc;                       // sameline cursor
  float scrolling_offset = 0.f;    //  MenuScrolling Pos
  bool enable_scrolling = false;   // Menu Scrolling
  float scrolling_mod = 0.f;       // For Menu Scrolling effect
  float tbh;                       // TabBar Height
  bool show_scroolbar = true;      // Show Scrollbar
  bool scrolling_possible = true;  // Scrolling Possible?
  bool has_touch = false;          // To Disable touch on Top Screen
  NTCtrl::Ref ctrl;                // NonTouchControl

  // SubMenu
  std::string submenu;

  // DrawLists
  UI7DrawList::Ref background;
  UI7DrawList::Ref main;
  UI7DrawList::Ref front;

  NVec2 ms;    // Max Size
  NVec2 mdp;   // Mouse/Touch Initial pos
  NVec2 bslp;  // Before SameLine Pos
  NVec2 lszs;  // Last Size
  // For Smart Pointer
  PD_SMART_CTOR(UI7Menu)
};

struct UI7_Ctx {
  UI7_Ctx() {
    delta = 0.0f;
    time = 0.0f;
    is_activated = false;
    _last = 0;
    in_menu = false;
    debugging = false;
    debug_menu = false;
  }
  float delta;
  float time;
  bool is_activated;
  float _last;
  bool in_menu;
  bool debugging;
  bool debug_menu;
  std::map<std::string, UI7Menu::Ref> menus;
  std::vector<UI7Menu::Ref> active_menus;
  UI7DrawList::Ref debug_calls;
  UI7DrawList::Ref bdl;
  UI7DrawList::Ref fdl;
  UI7Menu::Ref cm;

  PD_SMART_CTOR(UI7_Ctx)
};

UI7_Ctx::Ref ui7_ctx;

void UI7CtxPushDebugCmd(DrawCmd::Ref ref) {
  if (ui7_ctx->debugging) ui7_ctx->debug_calls->AddCall(ref);
}

bool UI7CtxValidate() {
  if (ui7_ctx == nullptr) return false;
  if (!ui7_ctx->is_activated) return false;
  return true;
}

bool UI7CtxInMenu() {
  if (!UI7CtxValidate()) return false;
  return ui7_ctx->in_menu;
}

bool UI7CtxBeginMenu(const std::string &lb) {
  if (!UI7CtxValidate()) return false;
  auto id = UI7ID(lb);
  if (ui7_ctx->menus.find(id.ID()) == ui7_ctx->menus.end())
    ui7_ctx->menus.insert(std::make_pair(id.ID(), UI7Menu::New()));
  ui7_ctx->cm = ui7_ctx->menus[id.ID()];
  if (!ui7_ctx->cm->ctrl) ui7_ctx->cm->ctrl = NTCtrl::New();
  ui7_ctx->cm->menuid = id;
  ui7_ctx->cm->cursor = NVec2(0, 0);
  ui7_ctx->cm->has_touch = !Palladium::R2::GetCurrentScreen();
  if (!ui7_ctx->cm->background) ui7_ctx->cm->background = UI7DrawList::New();
  if (!ui7_ctx->cm->main) ui7_ctx->cm->main = UI7DrawList::New();
  if (!ui7_ctx->cm->front) ui7_ctx->cm->front = UI7DrawList::New();
  ui7_ctx->in_menu = true;
  return true;
}

void UI7CtxEndMenu() {
  if (!UI7CtxValidate()) return;
  if (!UI7CtxInMenu()) return;
  Palladium::Ftrace::ScopedTrace tr("ui7", "EndMenu");
  // Draw Scrollbar
  if (ui7_ctx->cm->enable_scrolling) {
    ui7_ctx->cm->scrolling_possible = (ui7_ctx->cm->ms.y < 235 ? false : true);
    ui7_ctx->cm->show_scroolbar = ui7_ctx->cm->scrolling_possible;

    if (ui7_ctx->cm->show_scroolbar) {
      // Screen Width
      int sw = Palladium::R2::GetCurrentScreenSize().x;
      // Top Start Pos
      int tsp = 5 + ui7_ctx->cm->tbh;
      // Slider Width
      int slider_w = 4;
      // Height of Slider
      int szs = 240 - tsp - 5;
      // Lowest Height of Slider Obj
      int lszs = 20;  // Lowest Slider size
      // Calculate Slider Height
      float slider_h = (szs - 4) * (static_cast<float>(szs - 4) /
                                    static_cast<float>(ui7_ctx->cm->ms.y));
      // Create Real Slider Height
      int slider_rh = d7min(d7max(slider_h, (float)lszs), (float)(szs - 4));
      auto slider_clr = PDColor_Button;
      // Process Slider Dragging
      /// TODO: Optimize
      if (Palladium::Hid::IsEvent("touch", Palladium::Hid::Held)) {
        auto tp = Palladium::Hid::GetTouchPosition();
        if (UI7::InBox(tp, NVec2(sw - 10, tsp), NVec2(8, szs))) {
          slider_clr = PDColor_ButtonHovered;
          ui7_ctx->cm->scrolling_offset = d7max(
              0.f, d7min(ui7_ctx->cm->ms.y - 240,
                         ((tp.y - tsp) / szs) * (ui7_ctx->cm->ms.y - 240)));
        }
      }
      // Calculate Slider Position
      int slider_pos =
          tsp +
          d7min(static_cast<float>(szs - slider_rh - 4),
                d7max(0.f,
                      static_cast<float>(
                          (szs - slider_rh) *
                          (static_cast<float>(ui7_ctx->cm->scrolling_offset) /
                           static_cast<float>(ui7_ctx->cm->ms.y - 240.f)))));
      // Render Slider
      ui7_ctx->cm->front->AddRectangle(NVec2(sw - 12, tsp),
                                       NVec2(slider_w * 2, szs), PDColor_List0);
      ui7_ctx->cm->front->AddRectangle(NVec2(sw - 10, slider_pos + 2),
                                       NVec2(slider_w, slider_rh), slider_clr);
    }
  }
  // Debug Print Menu Values
  if (ui7_ctx->debug_menu) {
    std::stringstream str;
    str << "Menu: " << ui7_ctx->cm->menuid.ID() << std::endl;
    str << "ui7_ctx->cm->cursor: (" << ui7_ctx->cm->cursor.x << ", "
        << ui7_ctx->cm->cursor.y << ")" << std::endl;
    str << "ui7_ctx->cm->cb: (" << ui7_ctx->cm->cb.x << ", "
        << ui7_ctx->cm->cb.y << ")" << std::endl;
    str << "ui7_ctx->cm->slc: (" << ui7_ctx->cm->slc.x << ", "
        << ui7_ctx->cm->slc.y << ")" << std::endl;
    str << "ui7_ctx->cm->scrolling_offset: " << ui7_ctx->cm->scrolling_offset
        << std::endl;
    str << "ui7_ctx->cm->enable_scrolling: " << ui7_ctx->cm->enable_scrolling
        << std::endl;
    str << "ui7_ctx->cm->scrolling_mod: " << ui7_ctx->cm->scrolling_mod
        << std::endl;
    str << "ui7_ctx->cm->tbh: " << ui7_ctx->cm->tbh << std::endl;
    str << "ui7_ctx->cm->show_scroolbar: " << ui7_ctx->cm->show_scroolbar
        << std::endl;
    str << "ui7_ctx->cm->has_touch: " << ui7_ctx->cm->has_touch << std::endl;
    std::string submenu;

    str << "ui7_ctx->cm->ms: (" << ui7_ctx->cm->ms.x << ", "
        << ui7_ctx->cm->ms.y << ")" << std::endl;
    str << "ui7_ctx->cm->mdp: (" << ui7_ctx->cm->mdp.x << ", "
        << ui7_ctx->cm->mdp.y << ")" << std::endl;
    str << "ui7_ctx->cm->bslp: (" << ui7_ctx->cm->bslp.x << ", "
        << ui7_ctx->cm->bslp.y << ")" << std::endl;
    str << "ui7_ctx->cm->lszs: (" << ui7_ctx->cm->lszs.x << ", "
        << ui7_ctx->cm->lszs.y << ")" << std::endl;
    UI7::GetForegroundList()->AddRectangle(
        NVec2(), Palladium::R2::GetTextDimensions(str.str()),
        (unsigned int)RGBA8(0, 0, 0, 110));
    UI7::GetForegroundList()->AddText(NVec2(), str.str(),
                                      (unsigned int)RGBA8(255, 255, 255, 110));
  }
  ui7_ctx->active_menus.push_back(ui7_ctx->cm);
  ui7_ctx->cm = nullptr;
  ui7_ctx->in_menu = false;
}

namespace UI7 {
bool InBox(NVec2 inpos, NVec2 boxpos, NVec2 boxsize) {
  if ((inpos.x > boxpos.x) && (inpos.y > boxpos.y) &&
      (inpos.x < boxpos.x + boxsize.x) && (inpos.y < boxpos.y + boxsize.y))
    return true;
  return false;
}

void Init() {
  // If Context is valid it makes no sense to reinit lol
  if (UI7CtxValidate()) return;
  ui7_ctx = UI7_Ctx::New();
  ui7_ctx->delta = 0.0f;
  ui7_ctx->time = 0.0f;
  ui7_ctx->_last = __get_time();
  ui7_ctx->bdl = UI7DrawList::New();
  ui7_ctx->fdl = UI7DrawList::New();
  ui7_ctx->debug_calls = UI7DrawList::New();
  ui7_ctx->is_activated = true;
}

void Deinit() {
  if (!UI7CtxValidate()) return;
  ui7_ctx->is_activated = false;
  ui7_ctx->menus.clear();
  ui7_ctx->debug_calls->Clear();
  ui7_ctx->active_menus.clear();
}

void Update() {
  // Dont do anithing without ctx;
  if (!UI7CtxValidate()) return;
  ui7_ctx->bdl->Process();
  for (auto &it : ui7_ctx->active_menus) {
    it->background->Process();
    it->main->Process();
    it->front->Process();
    it->ctrl->Clear();
  }
  ui7_ctx->fdl->Process();
  ui7_ctx->active_menus.clear();
  float current = __get_time();
  ui7_ctx->delta = (current - ui7_ctx->_last) / 1000.f;
  ui7_ctx->_last = current;
  ui7_ctx->time += ui7_ctx->delta;
}

float GetTime() {
  if (!UI7CtxValidate()) return 0;
  return ui7_ctx->time;
}

float GetDeltaTime() {
  if (!UI7CtxValidate()) return 0;
  return ui7_ctx->delta;
}

bool Button(const std::string &label, NVec2 size) {
  bool ret = false;
  if (!UI7CtxValidate()) return ret;
  NVec2 textdim = Palladium::R2::GetTextDimensions(label);
  if (size.x == 0) {
    size.x = textdim.x + 8;
  }
  if (size.y == 0) {
    size.y = textdim.y + 4;
  }
  PDColor btn = PDColor_Button;
  NVec2 pos = GetCursorPos();

  MoveCursor(size);
  ui7_ctx->cm->ctrl->AddObj();

  if (HandleScrolling(pos, size)) return false;

  if (ui7_ctx->cm->has_touch) {
    if (Palladium::Hid::IsEvent("touch", Palladium::Hid::Up) &&
        InBox(Palladium::Hid::GetLastTouchPosition(), pos, size)) {
      btn = PDColor_ButtonActive;
      ret = true;
    } else if (Palladium::Hid::IsEvent("touch", Palladium::Hid::Held) &&
               InBox(Palladium::Hid::GetTouchPosition(), pos, size)) {
      btn = PDColor_ButtonHovered;
    }
  }
  ui7_ctx->cm->main->AddRectangle(pos, size, btn);
  pos = NVec2(pos.x + size.x * 0.5f - textdim.x * 0.5,
              pos.y + size.y * 0.5f - textdim.y * 0.5);
  ui7_ctx->cm->main->AddText(pos, label,
                             Palladium::ThemeActive()->AutoText(btn));
  return ret;
}

void Checkbox(const std::string &label, bool &c) {
  if (!UI7CtxValidate()) return;
  float sv = (Palladium::R2::GetTextSize() * 40) * 0.9;
  NVec2 cbs = NVec2(sv, sv);
  NVec2 txtdim = Palladium::R2::GetTextDimensions(label);
  NVec2 inp = cbs + NVec2(txtdim.x + 5, 0);
  PDColor bg = PDColor_FrameBg;

  NVec2 pos = GetCursorPos();

  MoveCursor(inp);
  ui7_ctx->cm->ctrl->AddObj();

  if (HandleScrolling(pos, inp)) return;

  if (ui7_ctx->cm->has_touch) {
    if (Palladium::Hid::IsEvent("touch", Palladium::Hid::Up) &&
        InBox(Palladium::Hid::GetLastTouchPosition(), pos, inp)) {
      bg = PDColor_FrameBgHovered;
      c = !c;
    } else if (Palladium::Hid::IsEvent("touch", Palladium::Hid::Held) &&
               InBox(Palladium::Hid::GetTouchPosition(), pos, inp)) {
      bg = PDColor_FrameBgHovered;
    }
  }

  ui7_ctx->cm->main->AddRectangle(pos, cbs, bg);
  if (c == true) {
    ui7_ctx->cm->main->AddRectangle(pos + NVec2(2, 2), cbs - NVec2(4, 4),
                                    PDColor_Checkmark);
  }
  ui7_ctx->cm->main->AddText(
      pos + NVec2(cbs.x + 5, 1), label,
      Palladium::ThemeActive()->AutoText(PDColor_Background));
}

void Label(const std::string &label, PDTextFlags flags) {
  if (!UI7CtxValidate()) return;
  NVec2 textdim = Palladium::R2::GetTextDimensions(label);
  NVec2 pos = GetCursorPos();
  auto upos = pos;
  // Remove some y offset cause texts have some offset
  MoveCursor(textdim - NVec2(0, 4));
  ui7_ctx->cm->ctrl->AddObj();

  if (HandleScrolling(pos, textdim)) return;

  float tbh = Palladium::R2::GetTextSize() * 40;
  auto &list =
      (upos.y + textdim.y < tbh) ? ui7_ctx->cm->front : ui7_ctx->cm->main;

  list->AddText(
      pos, label,
      Palladium::ThemeActive()->AutoText(
          (upos.y + textdim.y < tbh ? PDColor_Header : PDColor_Background)),
      flags);
}

void Progressbar(float value) {
  if (!UI7CtxValidate()) return;
  NVec2 pos = GetCursorPos();
  NVec2 size = NVec2(Palladium::R2::GetCurrentScreenSize().x - (pos.x * 2), 20);
  if (ui7_ctx->cm->show_scroolbar && ui7_ctx->cm->enable_scrolling)
    size.x -= 16;
  MoveCursor(size);
  ui7_ctx->cm->ctrl->AddObj();

  if (HandleScrolling(pos, size)) return;

  ui7_ctx->cm->main->AddRectangle(pos, size, PDColor_FrameBg);
  ui7_ctx->cm->main->AddRectangle(pos + NVec2(2, 2), size - NVec2(4, 4),
                                  PDColor_FrameBgHovered);
  if (!(value != value) && !(value < 0.0) && !(value > 1.0)) {
    ui7_ctx->cm->main->AddRectangle(pos + NVec2(2, 2),
                                    NVec2((size.x - 4) * value, size.y - 4),
                                    PDColor_Progressbar);
  }
}

void Image(Palladium::Image::Ref img) {
  if (!UI7CtxValidate()) return;
  NVec2 pos = GetCursorPos();
  MoveCursor(NVec2(img->GetSize().x, img->GetSize().y));
  ui7_ctx->cm->ctrl->AddObj();

  if (HandleScrolling(pos, img->GetSize())) return;

  ui7_ctx->cm->main->AddImage(pos, img);
}

void BrowserList(const std::vector<std::string> &entrys, int &selection,
                 PDTextFlags txtflags, NVec2 size, int max_entrys) {
  if (!UI7CtxValidate()) return;
  if (selection < 0) return;
  float tmp_txt = Palladium::R2::GetTextSize();
  Palladium::R2::DefaultTextSize();
  NVec2 pos = GetCursorPos();
  if (pos.y + 15 * max_entrys > 230) max_entrys = (int)((230 - pos.y) / 15);
  if (size.x == 0)
    size.x = Palladium::R2::GetCurrentScreenSize().x - (pos.x * 2);
  if (size.y == 0) size.y = (max_entrys * 15);
  MoveCursor(size);
  ui7_ctx->cm->ctrl->AddObj();
  int selindex = (selection < max_entrys ? selection : (max_entrys - 1));

  for (int i = 0; i < max_entrys; i++) {
    ui7_ctx->cm->main->AddRectangle(
        pos + NVec2(0, 15 * i), NVec2(size.x, 15),
        (i % 2 == 0 ? PDColor_List0 : PDColor_List1));
  }
  for (size_t i = 0;
       i < ((entrys.size() < (size_t)max_entrys) ? entrys.size()
                                                 : (size_t)max_entrys);
       i++) {
    int list_index =
        (selection < max_entrys ? i : (i + selection - (max_entrys - 1)));
    if (i == (size_t)selindex) {
      ui7_ctx->cm->main->AddRectangle(
          pos + NVec2(0, 15 * i), NVec2(size.x, 15),
          (unsigned int)Palladium::Color::RGBA(PDColor_Selector)
              .fade_to(PDColor_SelectorFade, std::sin(Palladium::GetTime()))
              .toRGBA());
    }
    ui7_ctx->cm->main->AddText(
        pos + NVec2(5, 15 * i), entrys[list_index],
        Palladium::ThemeActive()->AutoText(
            selindex == (int)i ? PDColor_Selector
                               : (i % 2 == 0 ? PDColor_List0 : PDColor_List1)),
        txtflags | PDTextFlags_Short, NVec2(size.x, 15));
  }
  Palladium::R2::SetTextSize(tmp_txt);
}

void InputText(const std::string &label, std::string &text,
               const std::string &hint) {
  if (!UI7CtxValidate()) return;
  float sv = (Palladium::R2::GetTextSize() * 40) * 0.9;
  NVec2 cbs = NVec2(144, sv);
  NVec2 txtdim = Palladium::R2::GetTextDimensions(label);
  NVec2 inp = cbs + NVec2(txtdim.x + 5, 0);
  PDColor bg = PDColor_FrameBg;
  auto id = UI7ID(label);
  PDKeyboardState kbd_state;  // tmp (goes out of scope)

  NVec2 pos = GetCursorPos();
  MoveCursor(inp);
  ui7_ctx->cm->ctrl->AddObj();

  if (HandleScrolling(pos, inp)) return;

  if (ui7_ctx->cm->has_touch) {
    if (Palladium::Hid::IsEvent("touch", Palladium::Hid::Up) &&
        InBox(Palladium::Hid::GetLastTouchPosition(), pos, inp)) {
      bg = PDColor_FrameBgHovered;
      Palladium::AddOvl(
          std::make_unique<Palladium::Ovl_Keyboard>(text, kbd_state, hint));
    } else if (Palladium::Hid::IsEvent("touch", Palladium::Hid::Held) &&
               InBox(Palladium::Hid::GetTouchPosition(), pos, inp)) {
      bg = PDColor_FrameBgHovered;
    }
  }

  ui7_ctx->cm->main->AddRectangle(pos, cbs, bg);
  ui7_ctx->cm->main->AddText(pos + NVec2(5, 1), (text != "" ? text : hint),
                             Palladium::ThemeActive()->AutoText(bg));
  ui7_ctx->cm->main->AddText(
      pos + NVec2(cbs.x + 5, 1), id.Title(),
      Palladium::ThemeActive()->AutoText(PDColor_Background));
}

bool BeginMenu(const std::string &title, NVec2 size, UI7MenuFlags flags) {
  if (!UI7CtxValidate()) return false;
  if (UI7CtxInMenu()) return false;
  auto id = UI7ID(title);
  auto ret = UI7CtxBeginMenu(title);
  if (!ret) return ret;
  bool titlebar = true;
  if (size.x == 0) {
    size.x = Palladium::R2::GetCurrentScreen() ? 400 : 320;
  }
  if (size.y == 0) {
    size.y = 240;
  }
  PDTextFlags txtflags = 0;
  float tbh = Palladium::R2::GetTextSize() * 40;
  ui7_ctx->cm->tbh = tbh;

  if (flags & UI7MenuFlags_NoTitlebar) {
    titlebar = false;
    ui7_ctx->cm->tbh = 0.f;
  }
  if (flags & UI7MenuFlags_TitleMid) txtflags = PDTextFlags_AlignMid;
  ui7_ctx->cm->enable_scrolling = (flags & UI7MenuFlags_Scrolling);
  if (ui7_ctx->cm->enable_scrolling && !Palladium::R2::GetCurrentScreen() &&
      ui7_ctx->cm->scrolling_possible) {
    // Patch that sets scrolling to 0 if max pos is not out of screen
    if (ui7_ctx->cm->scrolling_offset != 0.f && ui7_ctx->cm->ms.y < 235) {
      ui7_ctx->cm->scrolling_offset = 0.f;
    }
    // Auto scroll back if last object is on screen
    if (ui7_ctx->cm->scrolling_offset > ui7_ctx->cm->ms.y - 240 &&
        ui7_ctx->cm->ms.y != 0 && ui7_ctx->cm->ms.y >= 235) {
      ui7_ctx->cm->scrolling_offset -= 3;
      // Patch to Scroll to perfect pos
      if (ui7_ctx->cm->scrolling_offset < ui7_ctx->cm->ms.y - 240) {
        ui7_ctx->cm->scrolling_offset = ui7_ctx->cm->ms.y - 240;
      }
    }
    // Auto Scroll back if offset gets below 0
    if (ui7_ctx->cm->scrolling_offset < 0) {
      ui7_ctx->cm->scrolling_offset += 3;
      if (ui7_ctx->cm->scrolling_offset > 0) ui7_ctx->cm->scrolling_offset = 0;
    }

    // Zero out scrolling_mod if it goeas < -40
    // or > 40 over the max size
    if (ui7_ctx->cm->scrolling_offset < -40 ||
        ui7_ctx->cm->scrolling_offset > ui7_ctx->cm->ms.y - 200) {
      ui7_ctx->cm->scrolling_mod = 0.f;
    }
    if (ui7_ctx->cm->has_touch) {
      auto np = Palladium::Hid::GetTouchPosition();
      if (Palladium::Hid::IsEvent("touch", Palladium::Hid::Down)) {
        // Set the mdp Value as Start Pos
        ui7_ctx->cm->mdp = np;
      } else if (Palladium::Hid::IsEvent("touch", Palladium::Hid::Up)) {
        // 0 out the start pos
        ui7_ctx->cm->mdp = NVec2();
      }
      if (Palladium::Hid::IsEvent("touch", Palladium::Hid::Held)) {
        // Set modifier
        if (!InBox(np,
                   NVec2(Palladium::R2::GetCurrentScreenSize().x - 8 - 5,
                         5 + ui7_ctx->cm->tbh),
                   NVec2(8, 240 - ui7_ctx->cm->tbh - 10))) {
          // Check if and do nothing if the scrolling ofset goes out of screen
          if (ui7_ctx->cm->scrolling_offset < ui7_ctx->cm->ms.y - 200 &&
              ui7_ctx->cm->scrolling_offset > -40) {
            float cursor_mod = (ui7_ctx->cm->mdp.y - np.y);
            if (ui7_ctx->cm->scrolling_mod <= 4.f &&
                ui7_ctx->cm->scrolling_mod >= -4 && cursor_mod != 0.0f) {
              if (cursor_mod > 2) {
                ui7_ctx->cm->scrolling_mod = cursor_mod;
              } else if (cursor_mod < -2) {
                ui7_ctx->cm->scrolling_mod = cursor_mod;
              }
            }
          }
        }
        // Update Start pos
        ui7_ctx->cm->mdp = np;
      }
    }
    // New Scrolling efect
    if (ui7_ctx->cm->scrolling_mod != 0)
      ui7_ctx->cm->scrolling_offset += ui7_ctx->cm->scrolling_mod;
    // Slow out the effect
    if (ui7_ctx->cm->scrolling_mod < 0.f) {
      ui7_ctx->cm->scrolling_mod += 0.4f;
      if (ui7_ctx->cm->scrolling_mod > 0.f) {
        ui7_ctx->cm->scrolling_mod = 0.f;
      }
    } else if (ui7_ctx->cm->scrolling_mod > 0.f) {
      ui7_ctx->cm->scrolling_mod -= 0.4f;
      if (ui7_ctx->cm->scrolling_mod < 0.f) {
        ui7_ctx->cm->scrolling_mod = 0.f;
      }
    }
  } else {
    // Set scrollingoffset and mod to 0 if not scrolling enabled
    ui7_ctx->cm->scrolling_offset = 0.f;
    ui7_ctx->cm->scrolling_mod = 0.f;
  }
  ui7_ctx->cm->background->AddRectangle(NVec2(), size, PDColor_Background);
  if (titlebar) {
    ui7_ctx->cm->front->AddRectangle(NVec2(), NVec2(size.x, tbh),
                                     PDColor_Header);

    ui7_ctx->cm->front->AddText(
        NVec2(5, 2), id.Title(),
        Palladium::ThemeActive()->AutoText(PDColor_Header), txtflags,
        NVec2(size.x, 0));
  }

  SetCursorPos(NVec2(5, ui7_ctx->cm->tbh + 5));

  return ret;
}

void EndMenu() { UI7CtxEndMenu(); }

void Grid(const std::string &name, const NVec2 &size, const NVec2 &entry_size,
          void (*display_func)(void *, NVec2), void **data_array,
          size_t num_entrys) {
  if (!UI7CtxValidate()) return;
  if (num_entrys <= 0) return;
  if (data_array == nullptr) return;
  NVec2 pos = GetCursorPos();
  NVec2 cpos(pos);

  int neh = std::floor(size.x / (entry_size.x + 4));
  int nev = std::floor(size.y / (entry_size.y + 4));

  // Inside Grid Offset
  NVec2 igoff = NVec2();
  if (neh >= 2 && nev >= 2) {
    igoff = NVec2(
        ((size.x) / 2 - (((neh - 1) * (entry_size.x + 4)) + entry_size.x) / 2),
        ((size.y) / 2 - ((nev - 1) * ((entry_size.y + 4)) + entry_size.y) / 2));
  }
  // Y-Offset
  int yoff = 0;

  pos += igoff;
  for (size_t i = 0; i < num_entrys; i++) {
    display_func(data_array[i], pos);
    // if (ui7_ctx->debugging)
    //  Palladium::Draw2::Text(pos + NVec2(4, 4), std::to_string(i));
    if (pos.x + (entry_size.x * 2) > (cpos.x + size.x) &&
        pos.y + (entry_size.y * 2) > cpos.y + size.y) {
      break;
    } else if (pos.x + (entry_size.x * 2) > (cpos.x + size.x)) {
      pos = NVec2(5 + igoff.x, pos.y + entry_size.y + 4);
      yoff++;
    } else {
      pos += NVec2(entry_size.x + 4, 0);
    }
  }

  MoveCursor(size);
}

void ColorSelector(const std::string &label, unsigned int &color) {
  if (!UI7CtxValidate()) return;
  float sv = (Palladium::R2::GetTextSize() * 40) * 0.9;
  NVec2 cbs = NVec2(sv, sv);
  NVec2 txtdim = Palladium::R2::GetTextDimensions(label);
  NVec2 inp = cbs + NVec2(txtdim.x + 5, 0);
  auto outline =
      Palladium::Color::RGBA(color).is_light() ? 0xff000000 : 0xffffffff;
  auto id = UI7ID(label);

  NVec2 pos = GetCursorPos();
  MoveCursor(inp);
  ui7_ctx->cm->ctrl->AddObj();

  if (HandleScrolling(pos, inp)) return;

  if (ui7_ctx->cm->has_touch) {
    if (Palladium::Hid::IsEvent("touch", Palladium::Hid::Up) &&
        InBox(Palladium::Hid::GetLastTouchPosition(), pos, inp)) {
      ui7_ctx->cm->submenu = id.ID();
      // Nullify scrolling mod to fix freeze
      ui7_ctx->cm->scrolling_mod = 0.0f;
      Palladium::Hid::Lock();
      Palladium::Hid::Clear();
    }
  }

  if (ui7_ctx->cm->submenu == id.ID()) {
    // Base
    auto clr = Palladium::Color::RGBA(color);
    // Keyboard logic
    static bool inkbd;
    static std::string kbd_txt;
    static PDKeyboardState kbd_state;
    static int kbd_rgba = 0;
    if (kbd_state == PDKeyboardState_Confirm) {
      int num = std::atoi(kbd_txt.c_str());
      if (num < 0 || num > 255) {
        Palladium::PushMessage("UI7", "Error: Value must\nbe 0 - 255!");
      } else {
        if (kbd_rgba == 1) {
          clr.m_r = num;
        } else if (kbd_rgba == 2) {
          clr.m_g = num;
        } else if (kbd_rgba == 3) {
          clr.m_b = num;
        } else if (kbd_rgba == 4) {
          clr.m_a = num;
        }
      }
    }
    if (kbd_state) {
      inkbd = false;
      kbd_state = PDKeyboardState_None;
      kbd_txt = "";
      kbd_rgba = 0;
    }

    if (!inkbd) Palladium::Hid::Unlock();
    bool isunlock = false;
    NVec2 npos = pos;
    if (npos.y < ui7_ctx->cm->tbh + 2) npos.y = ui7_ctx->cm->tbh;
    if (npos.y + 97 > 235) npos.y = 137;
    // Input
    if (ui7_ctx->cm->has_touch) {
      auto ltp = Palladium::Hid::GetLastTouchPosition();
      if (Palladium::Hid::IsEvent("touch", Palladium::Hid::Up) &&
          (InBox(ltp, npos + NVec2(2, 2), cbs) ||
           !InBox(ltp, npos, NVec2(107, 97)))) {
        ui7_ctx->cm->submenu = "";
        isunlock = true;
      } else if (Palladium::Hid::IsEvent("cancel", Palladium::Hid::Down)) {
        ui7_ctx->cm->submenu = "";
        isunlock = true;
        Palladium::Hid::Clear();
      }
      if (Palladium::Hid::IsEvent("touch", Palladium::Hid::Up)) {
        if (InBox(ltp, npos + NVec2(2, cbs.y * 3 + 4), NVec2(50, cbs.y))) {
          inkbd = true;
          kbd_txt = std::to_string(clr.m_r);
          kbd_rgba = 1;
          Palladium::AddOvl(std::make_unique<Palladium::Ovl_Keyboard>(
              kbd_txt, kbd_state, "", PDKeyboard_Numpad));
        } else if (InBox(ltp, npos + NVec2(54, cbs.y * 3 + 4),
                         NVec2(50, cbs.y))) {
          inkbd = true;
          kbd_txt = std::to_string(clr.m_g);
          kbd_rgba = 2;
          Palladium::AddOvl(std::make_unique<Palladium::Ovl_Keyboard>(
              kbd_txt, kbd_state, "", PDKeyboard_Numpad));
        } else if (InBox(ltp, npos + NVec2(2, cbs.y * 4 + 4),
                         NVec2(50, cbs.y))) {
          inkbd = true;
          kbd_txt = std::to_string(clr.m_b);
          kbd_rgba = 3;
          Palladium::AddOvl(std::make_unique<Palladium::Ovl_Keyboard>(
              kbd_txt, kbd_state, "", PDKeyboard_Numpad));
        } else if (InBox(ltp, npos + NVec2(54, cbs.y * 4 + 4),
                         NVec2(50, cbs.y))) {
          inkbd = true;
          kbd_txt = std::to_string(clr.m_a);
          kbd_rgba = 4;
          Palladium::AddOvl(std::make_unique<Palladium::Ovl_Keyboard>(
              kbd_txt, kbd_state, "", PDKeyboard_Numpad));
        }
      }
      color = clr.toRGBA();
    }
    if (!isunlock && !inkbd) Palladium::Hid::Lock();
    // Draw Frame
    ui7_ctx->cm->front->AddRectangle(npos, NVec2(107, 97), PDColor_FrameBg);
    // Draw Color Button
    ui7_ctx->cm->front->AddRectangle(npos + NVec2(2, 2), cbs, outline);
    ui7_ctx->cm->front->AddRectangle(npos + NVec2(4, 4), cbs - NVec2(4, 4),
                                     color);
    // Draw Color Name Shorted if needed
    ui7_ctx->cm->front->AddText(
        npos + NVec2(cbs.x + 7, 1), label,
        Palladium::ThemeActive()->AutoText(PDColor_FrameBg), PDTextFlags_Short);
    // Add luminance text
    ui7_ctx->cm->front->AddText(
        npos + NVec2(2, cbs.y + 4), "lum: " + std::to_string(clr.luminance()),
        Palladium::ThemeActive()->AutoText(PDColor_FrameBg));
    // Add Hex value
    ui7_ctx->cm->front->AddText(
        npos + NVec2(2, cbs.y * 2 + 4),
        "hex: " + Palladium::Color::RGBA2Hex(color),
        Palladium::ThemeActive()->AutoText(PDColor_FrameBg));
    // Red
    {
      ui7_ctx->cm->front->AddRectangle(npos + NVec2(2, cbs.y * 3 + 4),
                                       NVec2(50, cbs.y),
                                       PDColor_FrameBgHovered);
      ui7_ctx->cm->front->AddRectangle(
          npos + NVec2(2, cbs.y * 3 + 4),
          NVec2(50 * ((float)clr.m_r / 255.f), cbs.y), 0xff0000ff);
      ui7_ctx->cm->front->AddText(npos + NVec2(2, cbs.y * 3 + 4),
                                  "R: " + std::to_string(clr.m_r), PDColor_Text,
                                  PDTextFlags_AlignMid, NVec2(50, 0));
    }
    // Green
    {
      ui7_ctx->cm->front->AddRectangle(npos + NVec2(54, cbs.y * 3 + 4),
                                       NVec2(50, cbs.y),
                                       PDColor_FrameBgHovered);
      ui7_ctx->cm->front->AddRectangle(
          npos + NVec2(54, cbs.y * 3 + 4),
          NVec2(50 * ((float)clr.m_g / 255.f), cbs.y), 0xff00ff00);
      ui7_ctx->cm->front->AddText(npos + NVec2(54, cbs.y * 3 + 4),
                                  "G: " + std::to_string(clr.m_g), PDColor_Text,
                                  PDTextFlags_AlignMid, NVec2(50, 0));
    }
    // Blue
    {
      ui7_ctx->cm->front->AddRectangle(npos + NVec2(2, cbs.y * 4 + 4),
                                       NVec2(50, cbs.y),
                                       PDColor_FrameBgHovered);
      ui7_ctx->cm->front->AddRectangle(
          npos + NVec2(2, cbs.y * 4 + 4),
          NVec2(50 * ((float)clr.m_b / 255.f), cbs.y), 0xffff0000);
      ui7_ctx->cm->front->AddText(npos + NVec2(2, cbs.y * 4 + 4),
                                  "B: " + std::to_string(clr.m_b), PDColor_Text,
                                  PDTextFlags_AlignMid, NVec2(50, 0));
    }
    // Alpha
    {
      ui7_ctx->cm->front->AddRectangle(npos + NVec2(54, cbs.y * 4 + 4),
                                       NVec2(50, cbs.y),
                                       PDColor_FrameBgHovered);
      ui7_ctx->cm->front->AddRectangle(
          npos + NVec2(54, cbs.y * 4 + 4),
          NVec2(50 * ((float)clr.m_a / 255.f), cbs.y), 0xffffffff);
      ui7_ctx->cm->front->AddText(npos + NVec2(54, cbs.y * 4 + 4),
                                  "A: " + std::to_string(clr.m_a), PDColor_Text,
                                  PDTextFlags_AlignMid, NVec2(50, 0));
    }
  }

  ui7_ctx->cm->main->AddRectangle(pos, cbs, outline);
  ui7_ctx->cm->main->AddRectangle(pos + NVec2(2, 2), cbs - NVec2(4, 4), color);
  ui7_ctx->cm->main->AddText(
      pos + NVec2(cbs.x + 5, 1), label,
      Palladium::ThemeActive()->AutoText(PDColor_Background));
}

bool BeginTree(const std::string &text) {
  // TODO
  return false;
}

void EndTree() {
  // TODO
}

NVec2 GetCursorPos() {
  if (!UI7CtxValidate()) return NVec2();
  if (!UI7CtxInMenu()) return NVec2();
  return ui7_ctx->cm->cursor;
}

void SetCursorPos(NVec2 cp) {
  if (!UI7CtxValidate()) return;
  if (!UI7CtxInMenu()) return;
  ui7_ctx->cm->cb = ui7_ctx->cm->cursor;
  ui7_ctx->cm->cursor = cp;
}

void RestoreCursor() {
  if (!UI7CtxValidate()) return;
  if (!UI7CtxInMenu()) return;
  ui7_ctx->cm->cursor = ui7_ctx->cm->cb;
}

void SameLine() {
  if (!UI7CtxValidate()) return;
  if (!UI7CtxInMenu()) return;
  ui7_ctx->cm->ctrl->NewRow();
  ui7_ctx->cm->bslp = ui7_ctx->cm->lszs;
  ui7_ctx->cm->cursor = ui7_ctx->cm->slc;
}

void Debug() {
  if (!UI7CtxValidate()) return;
  if (ui7_ctx->debugging) {
    ui7_ctx->debug_calls->Process(false);
  }
  ui7_ctx->debug_calls->Clear();
}

float Menu::GetScrollingOffset() {
  if (!UI7CtxValidate()) return 0.f;
  if (!UI7CtxInMenu()) return 0.f;
  return ui7_ctx->cm->scrolling_offset;
}

void Menu::SetScrollingOffset(float off) {
  if (!UI7CtxValidate()) return;
  if (!UI7CtxInMenu()) return;
  ui7_ctx->cm->scrolling_offset = off;
  ui7_ctx->cm->scrolling_mod = 0.f;
}

bool Menu::IsScrolling() {
  if (!UI7CtxValidate()) return false;
  if (!UI7CtxInMenu()) return false;
  return ui7_ctx->cm->scrolling_mod != 0.f;
}

void MoveCursor(NVec2 size) {
  if (!UI7CtxValidate()) return;
  if (!UI7CtxInMenu()) return;
  ui7_ctx->cm->lszs = size;
  ui7_ctx->cm->slc = ui7_ctx->cm->cursor + NVec2(size.x + 5, 0);
  ui7_ctx->cm->cursor.x = 5;
  if (ui7_ctx->cm->bslp.y) {
    ui7_ctx->cm->cursor += NVec2(0, ui7_ctx->cm->bslp.y + 5);
    ui7_ctx->cm->bslp = NVec2();
  } else {
    ui7_ctx->cm->cursor += NVec2(0, size.y + 5);
  }
  ui7_ctx->cm->ms = NVec2(ui7_ctx->cm->slc.x, ui7_ctx->cm->cursor.y);
}

bool HandleScrolling(NVec2 &pos, NVec2 size) {
  if (ui7_ctx->cm->enable_scrolling) {
    NVec2 pb = pos;
    pos -= NVec2(0, ui7_ctx->cm->scrolling_offset);
    if (pos.y > 240 ||
        (pos.y + size.y < ui7_ctx->cm->tbh - 5 && pb.y > ui7_ctx->cm->tbh))
      return true;
  }
  return false;
}

bool InMenu() {
  if (!UI7CtxValidate()) return false;
  return UI7CtxInMenu();
}

bool &IsDebugging() {
  if (!UI7CtxValidate()) {
    // Return a Default Val
    static bool t = false;
    return t;
  }
  return ui7_ctx->debugging;
}

bool &DebugMenu() {
  if (!UI7CtxValidate()) {
    // Return a Default Val
    static bool t = false;
    return t;
  }
  return ui7_ctx->debug_menu;
}

UI7DrawList::Ref GetForegroundList() {
  if (!UI7CtxValidate()) return nullptr;
  return ui7_ctx->fdl;
}

UI7DrawList::Ref GetBackgroundList() {
  if (!UI7CtxValidate()) return nullptr;
  return ui7_ctx->bdl;
}

UI7DrawList::Ref Menu::GetBackgroundList() {
  if (!UI7CtxValidate()) return nullptr;
  if (!UI7CtxInMenu()) return ui7_ctx->bdl;
  return ui7_ctx->cm->background;
}

UI7DrawList::Ref Menu::GetList() {
  if (!UI7CtxValidate()) return nullptr;
  if (!UI7CtxInMenu()) return ui7_ctx->bdl;
  return ui7_ctx->cm->main;
}

UI7DrawList::Ref Menu::GetForegroundList() {
  if (!UI7CtxValidate()) return nullptr;
  if (!UI7CtxInMenu()) return ui7_ctx->bdl;
  return ui7_ctx->cm->front;
}
}  // namespace UI7