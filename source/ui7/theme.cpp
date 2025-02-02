#include <pd/maths/color.hpp>
#include <pd/ui7/theme.hpp>

namespace PD {
namespace UI7 {
void Theme::Default(Theme& theme) {
  theme.Set(UI7Color_Text, Color("#FFFFFFFF"));
  theme.Set(UI7Color_TextDead, Color("#AAAAAAFF"));
  theme.Set(UI7Color_Background, Color("#222222aa"));
  theme.Set(UI7Color_Button, Color("#111111FF"));
  theme.Set(UI7Color_ButtonDead, Color("#080808FF"));
  theme.Set(UI7Color_ButtonActive, Color("#2A2A2AFF"));
  theme.Set(UI7Color_ButtonHovered, Color("#222222FF"));
  theme.Set(UI7Color_Header, Color("#111111FF"));
  theme.Set(UI7Color_Selector, Color("#222222FF"));
  theme.Set(UI7Color_Checkmark, Color("#2A2A2AFF"));
  theme.Set(UI7Color_FrameBackground, Color("#555555FF"));
  theme.Set(UI7Color_FrameBackgroundHovered, Color("#777777FF"));
  theme.Set(UI7Color_Progressbar, Color("#00FF00FF"));
  theme.Set(UI7Color_ListEven, Color("#CCCCCCFF"));
  theme.Set(UI7Color_ListOdd, Color("#BBBBBBFF"));
}
}  // namespace UI7
}  // namespace PD