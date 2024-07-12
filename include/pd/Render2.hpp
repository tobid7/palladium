#pragma once

#include <map>
#include <pd/Color.hpp>
#include <pd/Font.hpp>
#include <pd/Image.hpp>
#include <pd/NVec.hpp>
#include <pd/Sprite.hpp>
#include <pd/smart_ctor.hpp>

#define MAKEFLAG(x) (1 << x)

typedef unsigned int PDTextFlags;

enum PDTextFlags_ {
  PDTextFlags_None = 0,  //< Align is Left and Other things are disabled
  PDTextFlags_AlignRight = MAKEFLAG(0),
  PDTextFlags_AlignMid = MAKEFLAG(1),
  PDTextFlags_Shaddow = MAKEFLAG(2),  // TextBuf Killer lol (doubled Text)
  PDTextFlags_Wrap = MAKEFLAG(3),
  PDTextFlags_Short = MAKEFLAG(4),
  PDTextFlags_Scroll = MAKEFLAG(5),
};

enum R2Screen {
  R2Screen_Bottom,
  R2Screen_Top,
  // TopRight,
};

namespace Palladium {
class R2 {
 public:
  struct R2Cmd {
    NVec2 pos;        //< Position
    NVec2 pszs;       //< Position or (TextBox) Size
    NVec2 ap;         //< Additional Pos
    unsigned int clr;  //< Color
    bool Screen;       //< TopScreen
    Image::Ref img;    //< Image Reference
    Sprite::Ref spr;   //< Sprite Reference
    // 0 = skip, 1 = rect, 2 = tri, 3 = text,
    // 4 = image, 5 = sprite, 6 = Line
    int type;            //< Command Type
    bool lined = false;  //< Draw Lined Rect/Tri
    // Text Specific
    PDTextFlags flags;  // Text Flags
    std::string text;    // Text
    PD_SMART_CTOR(R2Cmd)
  };
  R2() = default;
  ~R2() = default;

  static void Init();

  // Settings
  static void SetFont(Font::Ref fnt);
  static Font::Ref GetFont();
  static void DefaultFont();
  static void DrawNextLined();
  static void OnScreen(R2Screen screen);
  static R2Screen GetCurrentScreen();
  static void SetTextSize(float szs);
  static void DefaultTextSize();
  static float GetTextSize();
  static NVec2 GetCurrentScreenSize();
  // Processing
  static void Process();
  static NVec2 GetTextDimensions(const std::string& text);
  static std::string WrapText(const std ::string& in, int maxlen);
  static std::string ShortText(const std::string& in, int maxlen);
  // Draw Functions
  static void AddRect(NVec2 pos, NVec2 size, PDColor clr);
  static void AddRect(NVec2 pos, NVec2 size, unsigned int clr);
  static void AddTriangle(NVec2 pos0, NVec2 pos1, NVec2 pos2, PDColor clr);
  static void AddTriangle(NVec2 pos0, NVec2 pos1, NVec2 pos2,
                          unsigned int clr);
  static void AddText(NVec2 pos, const std::string& text, PDColor clr,
                      PDTextFlags flags = 0, NVec2 tmb = NVec2());
  static void AddText(NVec2 pos, const std::string& text, unsigned int clr,
                      PDTextFlags flags = 0, NVec2 tmb = NVec2());
  static void AddImage(NVec2 pos, Image::Ref img);
  static void AddSprite(Sprite::Ref spr);
  static void AddLine(NVec2 pos_a, NVec2 pos_b, PDColor clr, int t = 1);
  static void AddLine(NVec2 pos_a, NVec2 pos_b, unsigned int clr, int t = 1);

 private:
  static const float default_text_size;
  static float text_size;
  static Font::Ref font;
  static std::map<std::string, float> ts;
  static std::map<std::string, int> mln;
  static bool next_lined;
  static std::vector<R2Cmd::Ref> commands;
  static R2Screen current_screen;
};
}  // namespace Palladium