#include <format>
#include <pd/Hid.hpp>
#include <pd/Lithium.hpp>
#include <pd/Overlays.hpp>
#include <pd/base/FunctionTrace.hpp>
#include <pd/internal_db.hpp>
#include <pd/palladium.hpp>

///////////////////////////////
struct Key {
  std::string disp;
  NVec2 pos;
  NVec2 size;
  // 0 = default key
  // 1 = Shift
  // 2 = Backsp
  // 3 = Enter
  // 4 = Cancel
  // 5 = Confirm
  // 6 = Tab
  // 7 = Caps
  // 8 = Space
  int action = 0;
};

std::vector<Key> keyboard_layout_num{
    // 1st row
    {"7", NVec2(5, 135), NVec2(36, 24), 0},
    {"8", NVec2(43, 135), NVec2(36, 24), 0},
    {"9", NVec2(81, 135), NVec2(36, 24), 0},
    // 2nd row
    {"4", NVec2(5, 161), NVec2(36, 24), 0},
    {"5", NVec2(43, 161), NVec2(36, 24), 0},
    {"6", NVec2(81, 161), NVec2(36, 24), 0},
    // 3rd row
    {"1", NVec2(5, 187), NVec2(36, 24), 0},
    {"2", NVec2(43, 187), NVec2(36, 24), 0},
    {"3", NVec2(81, 187), NVec2(36, 24), 0},

    // 4th row
    {"0", NVec2(5, 213), NVec2(74, 24), 0},
    {".", NVec2(81, 213), NVec2(36, 24), 0},
    // additional actions
    {"<---", NVec2(119, 135), NVec2(74, 24), 2},
    //{"", NVec2(119, 161), NVec2(74, 24), 0},
    {"Confirm", NVec2(119, 187), NVec2(74, 24), 5},
    {"Cancel", NVec2(119, 213), NVec2(74, 24), 4},
};

std::vector<Key> keyboard_layout = {
    // 1st row
    {"`", NVec2(5, 137), NVec2(18, 18), 0},
    {"1", NVec2(25, 137), NVec2(18, 18), 0},
    {"2", NVec2(45, 137), NVec2(18, 18), 0},
    {"3", NVec2(65, 137), NVec2(18, 18), 0},
    {"4", NVec2(85, 137), NVec2(18, 18), 0},
    {"5", NVec2(105, 137), NVec2(18, 18), 0},
    {"6", NVec2(125, 137), NVec2(18, 18), 0},
    {"7", NVec2(145, 137), NVec2(18, 18), 0},
    {"8", NVec2(165, 137), NVec2(18, 18), 0},
    {"9", NVec2(185, 137), NVec2(18, 18), 0},
    {"0", NVec2(205, 137), NVec2(18, 18), 0},
    {"-", NVec2(225, 137), NVec2(18, 18), 0},
    {"=", NVec2(245, 137), NVec2(18, 18), 0},
    {"<---", NVec2(265, 137), NVec2(50, 18), 2},
    // 2nd row
    {"Tab", NVec2(5, 157), NVec2(40, 18), 6},
    {"q", NVec2(47, 157), NVec2(18, 18), 0},
    {"w", NVec2(67, 157), NVec2(18, 18), 0},
    {"e", NVec2(87, 157), NVec2(18, 18), 0},
    {"r", NVec2(107, 157), NVec2(18, 18), 0},
    {"t", NVec2(127, 157), NVec2(18, 18), 0},
    {"y", NVec2(147, 157), NVec2(18, 18), 0},
    {"u", NVec2(167, 157), NVec2(18, 18), 0},
    {"i", NVec2(187, 157), NVec2(18, 18), 0},
    {"o", NVec2(207, 157), NVec2(18, 18), 0},
    {"p", NVec2(227, 157), NVec2(18, 18), 0},
    {"[", NVec2(247, 157), NVec2(18, 18), 0},
    {"]", NVec2(267, 157), NVec2(18, 18), 0},
    {"\\", NVec2(287, 157), NVec2(28, 18), 0},
    // 3rd row
    {"Caps", NVec2(5, 177), NVec2(50, 18), 7},
    {"a", NVec2(57, 177), NVec2(18, 18), 0},
    {"s", NVec2(77, 177), NVec2(18, 18), 0},
    {"d", NVec2(97, 177), NVec2(18, 18), 0},
    {"f", NVec2(117, 177), NVec2(18, 18), 0},
    {"g", NVec2(137, 177), NVec2(18, 18), 0},
    {"h", NVec2(157, 177), NVec2(18, 18), 0},
    {"j", NVec2(177, 177), NVec2(18, 18), 0},
    {"k", NVec2(197, 177), NVec2(18, 18), 0},
    {"l", NVec2(217, 177), NVec2(18, 18), 0},
    {";", NVec2(237, 177), NVec2(18, 18), 0},
    {"'", NVec2(257, 177), NVec2(18, 18), 0},
    {"Enter", NVec2(277, 177), NVec2(38, 18), 3},
    // 4th row
    {"Shift", NVec2(5, 197), NVec2(60, 18), 1},
    {"z", NVec2(67, 197), NVec2(18, 18), 0},
    {"x", NVec2(87, 197), NVec2(18, 18), 0},
    {"c", NVec2(107, 197), NVec2(18, 18), 0},
    {"v", NVec2(127, 197), NVec2(18, 18), 0},
    {"b", NVec2(147, 197), NVec2(18, 18), 0},
    {"n", NVec2(167, 197), NVec2(18, 18), 0},
    {"m", NVec2(187, 197), NVec2(18, 18), 0},
    {",", NVec2(207, 197), NVec2(18, 18), 0},
    {".", NVec2(227, 197), NVec2(18, 18), 0},
    {"/", NVec2(247, 197), NVec2(18, 18), 0},
    {"Shift", NVec2(267, 197), NVec2(48, 18), 1},
    // 5th row
    {"Cancel", NVec2(5, 217), NVec2(70, 18), 4},
    {"(X)", NVec2(77, 217), NVec2(23, 18), 10},
    {"Space", NVec2(102, 217), NVec2(108, 18), 8},
    {"(!)", NVec2(212, 217), NVec2(23, 18), 10},
    {"Confirm", NVec2(237, 217), NVec2(78, 18), 5},
    /*{"←", NVec2(237, 217), NVec2(18, 18)},
    {"→", NVec2(257, 217), NVec2(18, 18)},
    {"↓", NVec2(277, 217), NVec2(18, 18)},
    {"↑", NVec2(297, 217), NVec2(18, 18)},*/
};

std::vector<Key> keyboard_layout_caps = {
    // 1st row
    {"`", NVec2(5, 137), NVec2(18, 18), 0},
    {"1", NVec2(25, 137), NVec2(18, 18), 0},
    {"2", NVec2(45, 137), NVec2(18, 18), 0},
    {"3", NVec2(65, 137), NVec2(18, 18), 0},
    {"4", NVec2(85, 137), NVec2(18, 18), 0},
    {"5", NVec2(105, 137), NVec2(18, 18), 0},
    {"6", NVec2(125, 137), NVec2(18, 18), 0},
    {"7", NVec2(145, 137), NVec2(18, 18), 0},
    {"8", NVec2(165, 137), NVec2(18, 18), 0},
    {"9", NVec2(185, 137), NVec2(18, 18), 0},
    {"0", NVec2(205, 137), NVec2(18, 18), 0},
    {"-", NVec2(225, 137), NVec2(18, 18), 0},
    {"=", NVec2(245, 137), NVec2(18, 18), 0},
    {"<---", NVec2(265, 137), NVec2(50, 18), 2},
    // 2nd row
    {"Tab", NVec2(5, 157), NVec2(40, 18), 6},
    {"Q", NVec2(47, 157), NVec2(18, 18), 0},
    {"W", NVec2(67, 157), NVec2(18, 18), 0},
    {"E", NVec2(87, 157), NVec2(18, 18), 0},
    {"R", NVec2(107, 157), NVec2(18, 18), 0},
    {"T", NVec2(127, 157), NVec2(18, 18), 0},
    {"Y", NVec2(147, 157), NVec2(18, 18), 0},
    {"U", NVec2(167, 157), NVec2(18, 18), 0},
    {"I", NVec2(187, 157), NVec2(18, 18), 0},
    {"O", NVec2(207, 157), NVec2(18, 18), 0},
    {"P", NVec2(227, 157), NVec2(18, 18), 0},
    {"[", NVec2(247, 157), NVec2(18, 18), 0},
    {"]", NVec2(267, 157), NVec2(18, 18), 0},
    {"\\", NVec2(287, 157), NVec2(28, 18), 0},
    // 3rd row
    {"Caps", NVec2(5, 177), NVec2(50, 18), 7},
    {"A", NVec2(57, 177), NVec2(18, 18), 0},
    {"S", NVec2(77, 177), NVec2(18, 18), 0},
    {"D", NVec2(97, 177), NVec2(18, 18), 0},
    {"F", NVec2(117, 177), NVec2(18, 18), 0},
    {"G", NVec2(137, 177), NVec2(18, 18), 0},
    {"H", NVec2(157, 177), NVec2(18, 18), 0},
    {"J", NVec2(177, 177), NVec2(18, 18), 0},
    {"K", NVec2(197, 177), NVec2(18, 18), 0},
    {"L", NVec2(217, 177), NVec2(18, 18), 0},
    {";", NVec2(237, 177), NVec2(18, 18), 0},
    {"'", NVec2(257, 177), NVec2(18, 18), 0},
    {"Enter", NVec2(277, 177), NVec2(38, 18), 3},
    // 4th row
    {"Shift", NVec2(5, 197), NVec2(60, 18), 1},
    {"Z", NVec2(67, 197), NVec2(18, 18), 0},
    {"X", NVec2(87, 197), NVec2(18, 18), 0},
    {"C", NVec2(107, 197), NVec2(18, 18), 0},
    {"V", NVec2(127, 197), NVec2(18, 18), 0},
    {"B", NVec2(147, 197), NVec2(18, 18), 0},
    {"N", NVec2(167, 197), NVec2(18, 18), 0},
    {"M", NVec2(187, 197), NVec2(18, 18), 0},
    {",", NVec2(207, 197), NVec2(18, 18), 0},
    {".", NVec2(227, 197), NVec2(18, 18), 0},
    {"/", NVec2(247, 197), NVec2(18, 18), 0},
    {"Shift", NVec2(267, 197), NVec2(48, 18), 1},
    // 5th row
    {"Cancel", NVec2(5, 217), NVec2(70, 18), 4},
    {"(X)", NVec2(77, 217), NVec2(23, 18), 10},
    {"Space", NVec2(102, 217), NVec2(108, 18), 8},
    {"(!)", NVec2(212, 217), NVec2(23, 18), 10},
    {"Confirm", NVec2(237, 217), NVec2(78, 18), 5},
    /*{"←", NVec2(237, 217), NVec2(18, 18)},
    {"→", NVec2(257, 217), NVec2(18, 18)},
    {"↑", NVec2(277, 217), NVec2(18, 18)},
    {"↓", NVec2(297, 217), NVec2(18, 18)},*/
};

std::vector<Key> keyboard_layout_shift = {
    // 1st row
    {"~", NVec2(5, 137), NVec2(18, 18), 0},
    {"!", NVec2(25, 137), NVec2(18, 18), 0},
    {"@", NVec2(45, 137), NVec2(18, 18), 0},
    {"#", NVec2(65, 137), NVec2(18, 18), 0},
    {"$", NVec2(85, 137), NVec2(18, 18), 0},
    {"%", NVec2(105, 137), NVec2(18, 18), 0},
    {"^", NVec2(125, 137), NVec2(18, 18), 0},
    {"&", NVec2(145, 137), NVec2(18, 18), 0},
    {"*", NVec2(165, 137), NVec2(18, 18), 0},
    {"(", NVec2(185, 137), NVec2(18, 18), 0},
    {")", NVec2(205, 137), NVec2(18, 18), 0},
    {"_", NVec2(225, 137), NVec2(18, 18), 0},
    {"+", NVec2(245, 137), NVec2(18, 18), 0},
    {"<---", NVec2(265, 137), NVec2(50, 18), 2},
    // 2nd row
    {"Tab", NVec2(5, 157), NVec2(40, 18), 6},
    {"Q", NVec2(47, 157), NVec2(18, 18), 0},
    {"W", NVec2(67, 157), NVec2(18, 18), 0},
    {"E", NVec2(87, 157), NVec2(18, 18), 0},
    {"R", NVec2(107, 157), NVec2(18, 18), 0},
    {"T", NVec2(127, 157), NVec2(18, 18), 0},
    {"Y", NVec2(147, 157), NVec2(18, 18), 0},
    {"U", NVec2(167, 157), NVec2(18, 18), 0},
    {"I", NVec2(187, 157), NVec2(18, 18), 0},
    {"O", NVec2(207, 157), NVec2(18, 18), 0},
    {"P", NVec2(227, 157), NVec2(18, 18), 0},
    {"{", NVec2(247, 157), NVec2(18, 18), 0},
    {"}", NVec2(267, 157), NVec2(18, 18), 0},
    {"|", NVec2(287, 157), NVec2(28, 18), 0},
    // 3rd row
    {"Caps", NVec2(5, 177), NVec2(50, 18), 7},
    {"A", NVec2(57, 177), NVec2(18, 18), 0},
    {"S", NVec2(77, 177), NVec2(18, 18), 0},
    {"D", NVec2(97, 177), NVec2(18, 18), 0},
    {"F", NVec2(117, 177), NVec2(18, 18), 0},
    {"G", NVec2(137, 177), NVec2(18, 18), 0},
    {"H", NVec2(157, 177), NVec2(18, 18), 0},
    {"J", NVec2(177, 177), NVec2(18, 18), 0},
    {"K", NVec2(197, 177), NVec2(18, 18), 0},
    {"L", NVec2(217, 177), NVec2(18, 18), 0},
    {":", NVec2(237, 177), NVec2(18, 18), 0},
    {"\"", NVec2(257, 177), NVec2(18, 18), 0},
    {"Enter", NVec2(277, 177), NVec2(38, 18), 3},
    // 4th row
    {"Shift", NVec2(5, 197), NVec2(60, 18), 1},
    {"Z", NVec2(67, 197), NVec2(18, 18), 0},
    {"X", NVec2(87, 197), NVec2(18, 18), 0},
    {"C", NVec2(107, 197), NVec2(18, 18), 0},
    {"V", NVec2(127, 197), NVec2(18, 18), 0},
    {"B", NVec2(147, 197), NVec2(18, 18), 0},
    {"N", NVec2(167, 197), NVec2(18, 18), 0},
    {"M", NVec2(187, 197), NVec2(18, 18), 0},
    {"<", NVec2(207, 197), NVec2(18, 18), 0},
    {">", NVec2(227, 197), NVec2(18, 18), 0},
    {"?", NVec2(247, 197), NVec2(18, 18), 0},
    {"Shift", NVec2(267, 197), NVec2(48, 18), 1},
    // 5th row
    {"Cancel", NVec2(5, 217), NVec2(70, 18), 4},
    {"(X)", NVec2(77, 217), NVec2(23, 18), 10},
    {"Space", NVec2(102, 217), NVec2(108, 18), 8},
    {"(!)", NVec2(212, 217), NVec2(23, 18), 10},
    {"Confirm", NVec2(237, 217), NVec2(78, 18), 5},
    /*{"←", NVec2(237, 217), NVec2(18, 18)},
    {"→", NVec2(257, 217), NVec2(18, 18)},
    {"↑", NVec2(277, 217), NVec2(18, 18)},
    {"↓", NVec2(297, 217), NVec2(18, 18)},*/
};

// From UI7
bool UI7_InBox(NVec2 inpos, NVec2 boxpos, NVec2 boxsize) {
  if ((inpos.x() > boxpos.x()) && (inpos.y() > boxpos.y()) &&
      (inpos.x() < boxpos.x() + boxsize.x()) &&
      (inpos.y() < boxpos.y() + boxsize.y()))
    return true;
  return false;
}

namespace Palladium {
Ovl_Ftrace::Ovl_Ftrace(bool* is_enabled) { i_is_enabled = is_enabled; }

void Ovl_Ftrace::Draw(void) const {
  float tmp_txt = LI::GetTextScale();
  LI::DefaultTextScale();
  LI::OnScreen(false);
  if (pd_ftrace_ovl_flags & PDFTraceOverlayFlags_FillBg) {
    LI::NewLayer();
    Palladium::Color::RGBA bg(PDColor_Background);
    bg.changeA(150);
    LI::DrawRect(NVec2(0, 0), NVec2(400, 20), bg.toRGBA());
  }
  LI::NewLayer();
  int lrb = LI::Layer();
  std::string label = "FTrace Overlay";
  auto lbdim = LI::GetTextDimensions(label);
  LI::DrawRect(NVec2(), lbdim,
               Palladium::ThemeActive()->Get(PDColor_TextDisabled));
  LI::Layer(lrb + 1);
  LI::DrawText(NVec2(0, 0), Palladium::ThemeActive()->Get(PDColor_Text2),
               label);
  if (pd_ftrace_ovl_flags & PDFTraceOverlayFlags_DisplayHelp) {
    std::string hlp =
        (pd_ftrace_ovl_flags & PDFTraceOverlayFlags_DisplayName ? "Name" : "#");
    hlp += ": Current";
    if (pd_ftrace_ovl_flags & PDFTraceOverlayFlags_DisplayAverage ||
        pd_ftrace_ovl_flags & PDFTraceOverlayFlags_DisplayMin ||
        pd_ftrace_ovl_flags & PDFTraceOverlayFlags_DisplayMax) {
      hlp += " |";
      if (pd_ftrace_ovl_flags & PDFTraceOverlayFlags_DisplayAverage)
        hlp += " Avg";
      if (pd_ftrace_ovl_flags & PDFTraceOverlayFlags_DisplayMin) hlp += " Min";
      if (pd_ftrace_ovl_flags & PDFTraceOverlayFlags_DisplayMax) hlp += " Max";
    }
    auto hlpdim = LI::GetTextDimensions(hlp);
    LI::Layer(lrb);
    LI::DrawRect(NVec2(0, 20), hlpdim,
                 Palladium::ThemeActive()->Get(PDColor_TextDisabled));
    LI::Layer(lrb + 1);
    LI::DrawText(NVec2(0, 20), Palladium::ThemeActive()->Get(PDColor_Text2),
                 hlp);
  }

  std::vector<Palladium::Ftrace::FTRes> dt;
  for (auto const& it : Palladium::Ftrace::pd_traces)
    if (it.second.is_ovl && dt.size() < 10) dt.push_back(it.second);
  for (size_t i = 0; i < (dt.size() < 10 ? dt.size() : 10); i++) {
    std::string slot = (pd_ftrace_ovl_flags & PDFTraceOverlayFlags_DisplayName
                            ? dt[i].func_name
                            : std::to_string(i));
    slot += ": " + MsTimeFmt(dt[i].time_of);
    if (pd_ftrace_ovl_flags & PDFTraceOverlayFlags_DisplayAverage ||
        pd_ftrace_ovl_flags & PDFTraceOverlayFlags_DisplayMin ||
        pd_ftrace_ovl_flags & PDFTraceOverlayFlags_DisplayMax) {
      slot += " |";
      if (pd_ftrace_ovl_flags & PDFTraceOverlayFlags_DisplayAverage)
        slot += " " + MsTimeFmt(dt[i].ts.GetAverage());
      if (pd_ftrace_ovl_flags & PDFTraceOverlayFlags_DisplayMin)
        slot += " " + MsTimeFmt(dt[i].ts.GetMin());
      if (pd_ftrace_ovl_flags & PDFTraceOverlayFlags_DisplayMax)
        slot += " " + MsTimeFmt(dt[i].ts.GetMax());
    }
    auto dim = LI::GetTextDimensions(slot);
    LI::Layer(lrb);
    LI::DrawRect(NVec2(0, 37 + i * dim.y()), dim,
                 Palladium::ThemeActive()->Get(PDColor_TextDisabled));
    LI::Layer(lrb + 1);
    LI::DrawText(NVec2(0, 37 + i * dim.y()),
                 Palladium::ThemeActive()->Get(PDColor_Text2), slot);
  }
  LI::SetTextScale(tmp_txt);
}

void Ovl_Ftrace::Logic() {
  if (!i_is_enabled[0]) this->Kill();
}

Ovl_Metrik::Ovl_Metrik(bool* is_enabled, bool* screen, unsigned int* mt_color,
                       unsigned int* txt_color, float* txt_size)
    : cpu_stats(300), gpu_stats(300) {
  i_is_enabled = is_enabled;
  i_screen = screen;
  i_mt_color = mt_color;
  i_txt_color = txt_color;
  i_txt_size = txt_size;
  v_update.Reset();
}

int MetrikEntry(const std::string& text, NVec2 pos, unsigned int clr1,
                unsigned int clr2) {
  int dim_y = LI::GetTextDimensions(text).y();
  int lr = LI::Layer();
  LI::DrawRect(pos, LI::GetTextDimensions(text), clr1);
  LI::Layer(lr + 1);
  LI::DrawText(pos, clr2, text);
  LI::Layer(lr);
  return dim_y + !LI::GetFont()->IsSystemFont();
}

void Graph(Palladium::Ftrace::TimeStats& s, NVec2 pos, NVec2 size, NVec2 range,
           unsigned int clr, int lod = 1) {
  float xs = static_cast<float>(size.x() / s.GetLen());
  float ys = static_cast<float>(size.y() / (range.y() - range.x()));

  std::vector<NVec2> nodes;
  for (size_t i = 0; i < s.GetNumValues(); i += lod) {
    nodes.push_back(
        NVec2(pos.x() + i * xs, pos.y() + size.y() - (s[i] - range.x()) * ys));
  }
  for (size_t i = 1; i < nodes.size(); i++)
    LI::DrawLine(nodes[i - 1], nodes[i], clr);
}

void Ovl_Metrik::Draw(void) const {
  float tmp_txt = LI::GetTextScale();
  LI::SetTextScale(*i_txt_size);
  LI::OnScreen(i_screen[0]);
  LI::NewLayer();
  std::string info = "Palladium " + std::string(PDVSTRING) + " Debug Overlay";
  float dim_y = LI::GetTextDimensions(info).y();
  mt_fps = std::format("{:.2f}ms/f -> {:.1f} FPS", Palladium::GetDeltaTime(),
                       1000.f / Palladium::GetDeltaTime());
  if (pdi_idb_running) mt_fps += " IDB -> ON";
  float cpu_time = C3D_GetProcessingTime();
  cpu_stats.Add(cpu_time);
  float gpu_time = C3D_GetDrawingTime();
  gpu_stats.Add(gpu_time);
  v_update.Tick();
  if (v_update.Get() > 500.f) {
    float fps_lim = C3D_FrameRate(0.f) / 10.f;
    mt_cpu = std::format("CPU: {:.1f}% | {:.2f}ms | {:.2f}ms",
                         cpu_time * fps_lim, cpu_time, cpu_stats.GetAverage());
    mt_gpu = std::format("GPU: {:.1f}% | {:.2f}ms | {:.2f}ms",
                         gpu_time * fps_lim, gpu_time, gpu_stats.GetAverage());
    v_update.Reset();
  }
  mt_cmd = std::format("CMD: {:.2f}%", C3D_GetCmdBufUsage() * 100.f);
  mt_lfr = "Linear: " + Palladium::FormatBytes(linearSpaceFree());
  if (pd_flags & PDFlags_MemTrack)
    mt_mem = "Mem: " + Palladium::FormatBytes(Palladium::Memory::GetCurrent()) +
             " | " +
             Palladium::FormatBytes(Palladium::Memory::GetTotalAllocated()) +
             " | " + Palladium::FormatBytes(Palladium::Memory::GetTotalFreed());
  mt_vtx = "Vertices: " + std::to_string(LI::Vertices());
  mt_idx = "Indices: " + std::to_string(LI::Indices());
  mt_dmc = "DrawCmds: " + std::to_string(LI::DarwCommands());
  mt_drc = "DrawCalls: " + std::to_string(LI::Drawcalls());
  // Rendering
  int posy = 0;
  if (pd_ovl_flags & PDMetrikOverlayFlags_FPS)
    posy += MetrikEntry(mt_fps, NVec2(0, posy), i_mt_color[0], i_txt_color[0]);
  // Mod PosY to 50
  posy = 50;
  if (pd_ovl_flags & PDMetrikOverlayFlags_CPU)
    posy += MetrikEntry(mt_cpu, NVec2(0, posy), i_mt_color[0], i_txt_color[0]);
  if (pd_ovl_flags & PDMetrikOverlayFlags_GPU)
    posy += MetrikEntry(mt_gpu, NVec2(0, posy), i_mt_color[0], i_txt_color[0]);
  if (pd_ovl_flags & PDMetrikOverlayFlags_CMD)
    posy += MetrikEntry(mt_cmd, NVec2(0, posy), i_mt_color[0], i_txt_color[0]);
  if (pd_ovl_flags & PDMetrikOverlayFlags_LMM)
    posy += MetrikEntry(mt_lfr, NVec2(0, posy), i_mt_color[0], i_txt_color[0]);
  if (pd_ovl_flags & PDMetrikOverlayFlags_LVT)
    posy += MetrikEntry(mt_vtx, NVec2(0, posy), i_mt_color[0], i_txt_color[0]);
  if (pd_ovl_flags & PDMetrikOverlayFlags_LID)
    posy += MetrikEntry(mt_idx, NVec2(0, posy), i_mt_color[0], i_txt_color[0]);
  if (pd_ovl_flags & PDMetrikOverlayFlags_LDM)
    posy += MetrikEntry(mt_dmc, NVec2(0, posy), i_mt_color[0], i_txt_color[0]);
  if (pd_ovl_flags & PDMetrikOverlayFlags_LDC)
    posy += MetrikEntry(mt_drc, NVec2(0, posy), i_mt_color[0], i_txt_color[0]);
  if (pd_flags & PDFlags_MemTrack && pd_ovl_flags & PDMetrikOverlayFlags_MTD)
    posy += MetrikEntry(mt_mem, NVec2(0, posy), i_mt_color[0], i_txt_color[0]);
  posy = 240 - dim_y;
  if (pd_ovl_flags & PDMetrikOverlayFlags_PDO)
    posy += MetrikEntry(info, NVec2(0, posy), i_mt_color[0], i_txt_color[0]);
  if (pd_ovl_flags & PDMetrikOverlayFlags_CGR ||
      pd_ovl_flags & PDMetrikOverlayFlags_GGR) {
    LI::NewLayer();
    float tl = 1000.f / GetFps();
    std::string tlt = std::format("{:.2f}ms", tl);
    auto tldim = LI::GetTextDimensions(tlt);
    LI::DrawRect(NVec2(0, 17), NVec2(150 + tldim.x(), 33), i_mt_color[0]);
    LI::NewLayer();
    LI::DrawText(NVec2(150, 17), i_txt_color[0], tlt);
    if (pd_ovl_flags & PDMetrikOverlayFlags_CGR)
      Graph(cpu_stats, NVec2(0, 17), NVec2(150, 33), NVec2(0, tl), 0xff0000ff,
            2);
    if (pd_ovl_flags & PDMetrikOverlayFlags_GGR)
      Graph(gpu_stats, NVec2(0, 17), NVec2(150, 33), NVec2(0, tl), 0xffff0000,
            2);
  }
  // Force Bottom (Debug Touchpos)
  LI::OnScreen(true);
  if (Hid::IsEvent("touch", Hid::Held)) {
    LI::DrawLine(NVec2((int)Hid::GetTouchPosition().x(), 0),
                 NVec2((int)Hid::GetTouchPosition().x(), 240),
                 Palladium::Color::Hex("#ff0000"));
    LI::DrawLine(NVec2(0, (int)Hid::GetTouchPosition().y()),
                 NVec2(320, (int)Hid::GetTouchPosition().y()),
                 Palladium::Color::Hex("#ff0000"));
  }
  LI::SetTextScale(tmp_txt);
}

void Ovl_Metrik::Logic() {
  if (!i_is_enabled[0]) this->Kill();
}

Ovl_Keyboard::Ovl_Keyboard(std::string& ref, PDKeyboardState& state,
                           const std::string& hint, PDKeyboard type,
                           PDKeyboardFlags flags) {
  // Blocks All Input outside of Keyboard
  // Doesnt work for Hidkeys down etc
  if (flags & PDKeyboardFlags_LockControls) Palladium::Hid::Lock();
  typed_text = &ref;
  this->state = &state;
  this->type = type;
  this->flags = flags;
  *this->state = PDKeyboardState_None;
  str_bak = ref;
  ft3 = 0;
}

Ovl_Keyboard::~Ovl_Keyboard() {
  // And Unlock when closing Keyboard lol
  if (flags & PDKeyboardFlags_LockControls) Palladium::Hid::Unlock();
}

void Ovl_Keyboard::Draw(void) const {
  float tmp_txt = LI::GetTextScale();
  LI::DefaultTextScale();
  if (ft3 > 5) {
    if (flags & PDKeyboardFlags_LockControls) Palladium::Hid::Unlock();
  }
  auto key_table =
      (type == PDKeyboard_Numpad) ? keyboard_layout_num : keyboard_layout;
  if (mode == 1)
    key_table = keyboard_layout_caps;
  else if (mode == 2)
    key_table = keyboard_layout_shift;
  if (flags & PDKeyboardFlags_BlendTop) {
    LI::OnScreen(false);
    LI::NewLayer();
    LI::DrawRect(NVec2(0, 0), NVec2(400, 240),
                 Palladium::Color::RGBA(PDColor_FrameBg).changeA(150).toRGBA());
  }
  LI::OnScreen(true);
  LI::NewLayer();
  if (flags & PDKeyboardFlags_BlendBottom) {
    LI::DrawRect(NVec2(0, 0), NVec2(320, 112),
                 Palladium::Color::RGBA(PDColor_FrameBg).changeA(150).toRGBA());
  }
  LI::DrawRect(NVec2(0, 112), NVec2(320, 128),
               ThemeActive()->Get(PDColor_FrameBg));
  LI::DrawRect(NVec2(0, 112), NVec2(320, 20),
               ThemeActive()->Get(PDColor_Header));
  LI::NewLayer();
  LI::DrawText(
      NVec2(5, 114),
      ThemeActive()->Get(Palladium::ThemeActive()->AutoText(PDColor_Header)),
      "> " + *typed_text);
  int lr = LI::Layer();
  for (auto const& it : key_table) {
    NVec2 szs = it.size;
    NVec2 pos = it.pos;
    NVec2 txtdim = LI::GetTextDimensions(it.disp);
    PDColor btn = PDColor_Button;
    if (Palladium::Hid::IsEvent("cancel", Palladium::Hid::Up)) {
      Palladium::Hid::Clear();
      shared_data[0x05] = 1;
    }
    if (Palladium::Hid::IsEvent("touch", Palladium::Hid::Up) &&
        UI7_InBox(Palladium::Hid::GetLastTouchPosition(), pos, szs)) {
      if (mode == 2)  // Request Disable Shift
        shared_data[0x02] = 1;

      if (it.action == 0)
        shared_data[0x01] = it.disp[0];
      else if (it.action == 1)
        shared_data[0x02] = 1;
      else if (it.action == 2)
        shared_data[0x03] = 1;
      else if (it.action == 3)
        shared_data[0x04] = 1;
      else if (it.action == 4)
        shared_data[0x05] = 1;
      else if (it.action == 5)
        shared_data[0x06] = 1;
      else if (it.action == 6)
        shared_data[0x07] = 1;
      else if (it.action == 7)
        shared_data[0x08] = 1;
      else if (it.action == 8)
        shared_data[0x09] = 1;
    } else if (Palladium::Hid::IsEvent("touch", Palladium::Hid::Held) &&
               UI7_InBox(Palladium::Hid::GetTouchPosition(), it.pos, it.size)) {
      btn = PDColor_ButtonHovered;
      pos -= NVec2(1, 1);
      szs += NVec2(2, 2);
    }
    NVec2 txtpos = NVec2(pos.x() + szs.x() * 0.5 - txtdim.x() * 0.5,
                         pos.y() + szs.y() * 0.5 - txtdim.y() * 0.5);
    LI::Layer(lr);
    LI::DrawRect(pos, szs, ThemeActive()->Get(btn));
    LI::Layer(lr + 1);
    LI::DrawText(txtpos,
                 ThemeActive()->Get(Palladium::ThemeActive()->AutoText(btn)),
                 it.disp);
  }
  if (ft3 > 5) {
    if (flags & PDKeyboardFlags_LockControls) Palladium::Hid::Lock();
  }
  LI::SetTextScale(tmp_txt);
}

void Ovl_Keyboard::Logic() {
  ft3++;
  for (const auto& it : shared_data) {
    if (it.first == 0x01) {
      typed_text->push_back(it.second);
    } else if (it.first == 0x02) {
      // Shift
      mode = (mode == 2) ? 0 : 2;
    } else if (it.first == 0x03) {
      if (typed_text->length() >= 1)
        typed_text->erase(typed_text->begin() + typed_text->length() - 1);
    } else if (it.first == 0x04) {
      // Enter
    } else if (it.first == 0x05) {
      *typed_text = str_bak;
      *state = PDKeyboardState_Cancel;
      this->Kill();
    } else if (it.first == 0x06) {
      *state = PDKeyboardState_Confirm;
      this->Kill();
    } else if (it.first == 0x07) {
      // this->typed_text += '\t';  // Tab
    } else if (it.first == 0x08) {
      // Caps
      mode = (mode == 1) ? 0 : 1;
    } else if (it.first == 0x09) {
      typed_text->append(" ");  // Space
    }
  }

  shared_data.clear();
}
}  // namespace Palladium