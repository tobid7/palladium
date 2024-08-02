#include <pd/FunctionTrace.hpp>
#include <pd/Hid.hpp>
#include <pd/Overlays.hpp>
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
  if ((inpos.x > boxpos.x) && (inpos.y > boxpos.y) &&
      (inpos.x < boxpos.x + boxsize.x) && (inpos.y < boxpos.y + boxsize.y))
    return true;
  return false;
}

namespace Palladium {
Ovl_Ftrace::Ovl_Ftrace(bool* is_enabled) { i_is_enabled = is_enabled; }

void Ovl_Ftrace::Draw(void) const {
  float tmp_txt = R2::GetTextSize();
  R2::DefaultTextSize();
  R2::OnScreen(R2Screen_Top);
  Palladium::Color::RGBA bg(PDColor_Background);
  bg.changeA(150);
  R2::AddRect(NVec2(0, 0), NVec2(400, 20), bg.toRGBA());

  std::vector<Palladium::Ftrace::FTRes> dt;
  for (auto const& it : Palladium::Ftrace::pd_traces)
    if (it.second.is_ovl && dt.size() < 10) dt.push_back(it.second);
  for (size_t i = 0; i < (dt.size() < 10 ? dt.size() : 10); i++) {
    std::string text = dt[i].func_name + ": " + Palladium::MsTimeFmt(dt[i].time_of);
    auto dim = R2::GetTextDimensions(text);
    R2::AddRect(NVec2(5, 30+i*dim.y), dim, PDColor_TextDisabled);
    R2::AddText(NVec2(5, 30 + i * dim.y), text, PDColor_Text2);
  }
  R2::SetTextSize(tmp_txt);
}

void Ovl_Ftrace::Logic() {
  if (!i_is_enabled[0]) this->Kill();
}

Ovl_Metrik::Ovl_Metrik(bool* is_enabled, bool* screen, uint32_t* mt_color,
                       uint32_t* txt_color, float* txt_size) {
  i_is_enabled = is_enabled;
  i_screen = screen;
  i_mt_color = mt_color;
  i_txt_color = txt_color;
  i_txt_size = txt_size;
}

void Ovl_Metrik::Draw(void) const {
  float tmp_txt = R2::GetTextSize();
  R2::DefaultTextSize();
  R2::OnScreen(i_screen[0] ? R2Screen_Bottom : R2Screen_Top);
  std::string info = "Palladium " + std::string(PDVSTRING) + " Debug Overlay";
  float dim_y = R2::GetTextDimensions(info).y;
  float infoy = 240 - dim_y;
  mt_fps = "FPS: " + Palladium::GetFramerate();
  if (pdi_idb_running) mt_fps += " IDB -> ON";
  mt_cpu = "CPU: " +
           std::to_string(C3D_GetProcessingTime() * (Palladium::GetFps() / 10))
               .substr(0, 4) +
           "%/" + std::to_string(C3D_GetProcessingTime()).substr(0, 4) + "ms";
  mt_gpu = "GPU: " +
           std::to_string(C3D_GetDrawingTime() * (Palladium::GetFps() / 10))
               .substr(0, 4) +
           "%/" + std::to_string(C3D_GetDrawingTime()).substr(0, 4) + "ms";
  mt_cmd =
      "CMD: " + std::to_string(C3D_GetCmdBufUsage() * 100.0f).substr(0, 4) +
      "%";
  mt_lfr = "Linear: " + Palladium::FormatBytes(linearSpaceFree());
  if (pdi_enable_memtrack)
    mt_mem = "Mem: " + Palladium::FormatBytes(Palladium::Memory::GetCurrent()) +
             " | " +
             Palladium::FormatBytes(Palladium::Memory::GetTotalAllocated()) +
             " | " + Palladium::FormatBytes(Palladium::Memory::GetTotalFreed());
  mt_vtx = "Vertices: " + std::to_string(LI7::Vertices());
  mt_dmc = "DrawCmds: " + std::to_string(LI7::DarwCommands());
  mt_drc = "DrawCalls: " + std::to_string(LI7::Drawcalls());
  R2::AddRect(NVec2(0, 0), R2::GetTextDimensions(mt_fps),
              (unsigned int)i_mt_color[0]);
  R2::AddRect(NVec2(0, 50), R2::GetTextDimensions(mt_cpu),
              (unsigned int)i_mt_color[0]);
  R2::AddRect(NVec2(0, 50 + dim_y * 1), R2::GetTextDimensions(mt_gpu),
              (unsigned int)i_mt_color[0]);
  R2::AddRect(NVec2(0, 50 + dim_y * 2), R2::GetTextDimensions(mt_cmd),
              (unsigned int)i_mt_color[0]);
  R2::AddRect(NVec2(0, 50 + dim_y * 3), R2::GetTextDimensions(mt_lfr),
              (unsigned int)i_mt_color[0]);
  R2::AddRect(NVec2(0, 50 + dim_y * 4), R2::GetTextDimensions(mt_vtx),
              (unsigned int)i_mt_color[0]);
  R2::AddRect(NVec2(0, 50 + dim_y * 5), R2::GetTextDimensions(mt_dmc),
              (unsigned int)i_mt_color[0]);
  R2::AddRect(NVec2(0, 50 + dim_y * 6), R2::GetTextDimensions(mt_drc),
              (unsigned int)i_mt_color[0]);
  if (pdi_enable_memtrack)
    R2::AddRect(NVec2(0, 50 + dim_y * 7), R2::GetTextDimensions(mt_mem),
                (unsigned int)i_mt_color[0]);
  R2::AddRect(NVec2(0, infoy), R2::GetTextDimensions(info),
              (unsigned int)i_mt_color[0]);
  R2::AddText(NVec2(0, 0), mt_fps, (unsigned int)i_txt_color[0]);
  R2::AddText(NVec2(0, 50), mt_cpu, (unsigned int)i_txt_color[0]);
  R2::AddText(NVec2(0, 50 + dim_y * 1), mt_gpu, (unsigned int)i_txt_color[0]);
  R2::AddText(NVec2(0, 50 + dim_y * 2), mt_cmd, (unsigned int)i_txt_color[0]);
  R2::AddText(NVec2(0, 50 + dim_y * 3), mt_lfr, (unsigned int)i_txt_color[0]);
  R2::AddText(NVec2(0, 50 + dim_y * 4), mt_vtx, (unsigned int)i_txt_color[0]);
  R2::AddText(NVec2(0, 50 + dim_y * 5), mt_dmc, (unsigned int)i_txt_color[0]);
  R2::AddText(NVec2(0, 50 + dim_y * 6), mt_drc, (unsigned int)i_txt_color[0]);
  if (pdi_enable_memtrack)
    R2::AddText(NVec2(0, 50 + dim_y * 7), mt_mem, (unsigned int)i_txt_color[0]);
  R2::AddText(NVec2(0, infoy), info, (unsigned int)i_txt_color[0]);

  // Force Bottom (Debug Touchpos)
  R2::OnScreen(R2Screen_Bottom);
  if (Hid::IsEvent("touch", Hid::Held)) {
    R2::AddLine(NVec2(Hid::GetTouchPosition().x, 0),
                NVec2(Hid::GetTouchPosition().x, 240),
                Palladium::Color::Hex("#ff0000"));
    R2::AddLine(NVec2(0, Hid::GetTouchPosition().y),
                NVec2(320, Hid::GetTouchPosition().y),
                Palladium::Color::Hex("#ff0000"));
  }
  R2::SetTextSize(tmp_txt);
}

void Ovl_Metrik::Logic() {
  if (!i_is_enabled[0]) this->Kill();
}

Ovl_Keyboard::Ovl_Keyboard(std::string& ref, PDKeyboardState& state,
                           const std::string& hint, PDKeyboard type) {
  // Blocks All Input outside of Keyboard
  // Doesnt work for Hidkeys down etc
  Palladium::Hid::Lock();
  typed_text = &ref;
  this->state = &state;
  this->type = type;
  *this->state = PDKeyboardState_None;
  str_bak = ref;
  ft3 = 0;
}

Ovl_Keyboard::~Ovl_Keyboard() {
  // And Unlock when closing Keyboard lol
  Palladium::Hid::Unlock();
}

void Ovl_Keyboard::Draw(void) const {
  float tmp_txt = R2::GetTextSize();
  R2::DefaultTextSize();
  if (ft3 > 5) Palladium::Hid::Unlock();
  auto key_table =
      (type == PDKeyboard_Numpad) ? keyboard_layout_num : keyboard_layout;
  if (mode == 1)
    key_table = keyboard_layout_caps;
  else if (mode == 2)
    key_table = keyboard_layout_shift;
  R2::OnScreen(R2Screen_Top);
  R2::AddRect(NVec2(0, 0), NVec2(400, 240),
              Palladium::Color::RGBA(PDColor_FrameBg).changeA(150).toRGBA());
  R2::OnScreen(R2Screen_Bottom);
  R2::AddRect(NVec2(0, 0), NVec2(320, 112),
              Palladium::Color::RGBA(PDColor_FrameBg).changeA(150).toRGBA());
  R2::AddRect(NVec2(0, 112), NVec2(320, 128), PDColor_FrameBg);
  R2::AddRect(NVec2(0, 112), NVec2(320, 20), PDColor_Header);
  R2::AddText(NVec2(5, 114), "> " + *typed_text,
              Palladium::ThemeActive()->AutoText(PDColor_Header));
  for (auto const& it : key_table) {
    NVec2 szs = it.size;
    NVec2 pos = it.pos;
    NVec2 txtdim = R2::GetTextDimensions(it.disp);
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
    NVec2 txtpos = NVec2(pos.x + szs.x * 0.5 - txtdim.x * 0.5,
                         pos.y + szs.y * 0.5 - txtdim.y * 0.5);
    R2::AddRect(pos, szs, btn);
    R2::AddText(txtpos, it.disp, Palladium::ThemeActive()->AutoText(btn));
  }
  if (ft3 > 5) Palladium::Hid::Lock();
  R2::SetTextSize(tmp_txt);
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