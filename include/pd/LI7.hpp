#pragma once
#include <citro3d.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include <pd/Allocator.hpp>
#include <pd/NVec.hpp>
#include <pd/Texture.hpp>
#include <vector>

#define MAKEFLAG(x) (1 << x)

using PDTextFlags = unsigned int;

enum PDTextFlags_ {
  PDTextFlags_None = 0,  //< Align is Left and Other things are disabled
  PDTextFlags_AlignRight = MAKEFLAG(0),
  PDTextFlags_AlignMid = MAKEFLAG(1),
  PDTextFlags_Shaddow = MAKEFLAG(2),  // TextBuf Killer lol (doubled Text)
  PDTextFlags_Wrap = MAKEFLAG(3),
  PDTextFlags_Short = MAKEFLAG(4),
  PDTextFlags_Scroll = MAKEFLAG(5),
};

namespace Palladium {
class LIFont {
 public:
  struct CPI {
    unsigned char codepoint;
    NVec4 uv;
    Texture::Ref tex;
    NVec2 szs;
    float off;
  };
  LIFont() = default;
  ~LIFont() = default;
  PD_SMART_CTOR(LIFont)

  void LoadTFF(const std::string path, int px_size = 32);
  void LoadBitmapFont(const std::string& path);
  void LoadSystemFont();

  int GetPixelHeight();
  CPI GetCodepoint(char c);
  std::string GetName() { return name; };

  void Dump();

 private:
  std::string name;
  int pixel_height;
  std::vector<CPI> cpmap;
  std::vector<Texture::Ref> tex;
};
class LI7 {
 public:
  struct Vtx {
    Vtx() {}
    Vtx(NVec2 xy, float u, float v, unsigned int clr) {
      // Coords
      xyz[0] = xy.x;
      xyz[1] = xy.y;
      xyz[2] = 0.5f;  // Always 0
      // UV
      uv[0] = u;
      uv[1] = v;
      col = clr;
    }
    float xyz[3];
    float uv[2];
    unsigned int col;
  };
  /// CMD TYPES ///
  /// 0 = SKIP
  /// 1 = TRIANGLE
  /// 2 = RECT
  /// 3 = RECT2P (2 Positions instead of pos,szs)
  /////////////////
  struct Cmd {
    NVec2 ppos;
    NVec2 pszs;
    NVec2 apos;
    NVec4 top;
    NVec4 bot;
    NVec4 uv;
    int layer = 0;
    int cmd_type = 0;
    unsigned int clr = 0;
    Texture::Ref tex = nullptr;
  };
  LI7() = default;
  ~LI7() = default;

  static void Init();
  static void Exit();

  static void OnScreen(bool bottom);
  static void Render(C3D_RenderTarget* top, C3D_RenderTarget* bot);
  static void Scale(float i) { m_scale = i; }
  static float Scale() { return m_scale; }
  static void BindTexture(Texture::Ref tex);
  static NVec2 ScreenSize() { return NVec2(m_width, m_height); }
  static LIFont::Ref GetFont() { return m_font; }

  static void ColorRect(NVec2 pos, NVec2 szs, NVec4 uvs, unsigned int clr);
  static void Rect(NVec2 pos, NVec2 szs, NVec4 uvs);
  static void ColorRect(NVec2 pos, NVec2 szs, unsigned int clr);
  static void Rect(NVec2 pos, NVec2 szs);
  static void TexCutRect(NVec2 pos, NVec2 szs, NVec2 cb, NVec2 ce);
  static void Line(NVec2 a, NVec2 b, unsigned int clr, int t = 1);
  static void Triangle(NVec2 a, NVec2 b, NVec2 c, unsigned int clr);
  static void UseTexture(Texture::Ref tex = nullptr) {
    m_current_texture = tex ? tex : m_white;
  }

  static NVec2 GetTextDimensions(const std::string& text);
  static void DrawText(NVec2 pos, unsigned int color, const std::string& text,
                       PDTextFlags flags = 0, NVec2 ap = NVec2());

  static float GetTextScale() { return m_txt_scale; }
  static void SetTextScale(float scale) { m_txt_scale = scale; }
  static void DefaultTextScale() { m_txt_scale = m_dts; }

  static int Vertices() { return m_d_vertices; }
  static int Drawcalls() { return m_d_drawcalls; }
  static int DarwCommands() { return m_d_commands; }

 private:
  static bool CompareCommands(const Cmd& a, const Cmd& b);
  static void RenderFrame(bool bottom);

  // Default Font Size in (px)
  static const float m_dffs;
  static const float m_dts;
  static float m_txt_scale;

  static int m_uLoc_proj;
  static float m_scale;
  static int m_width, m_height;

  static int m_d_vertices;
  static int m_d_drawcalls;
  static int m_d_commands;

  static const int m_char_height;  // Constant

  // UI Stuff
  static std::vector<Cmd> m_top_draw_cmds;
  static std::vector<Cmd> m_bot_draw_cmds;
  static Texture::Ref m_current_texture;
  static Texture::Ref m_white;
  static std::vector<Vtx, LinearAllocator<Vtx>> m_vtx_list[2];
  // static Font* m_font;
  static LIFont::Ref m_font;
  static std::vector<char> m_text_buffer;

  // Ctx Stuff
  static bool m_bottom_active;

  // Shader
  static DVLB_s* li7_dvlb;
  static shaderProgram_s li7_prog;
  static C3D_AttrInfo li7_attr;
};
}  // namespace Palladium