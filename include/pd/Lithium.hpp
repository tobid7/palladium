#pragma once
#include <citro3d.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include <map>
#include <pd/Texture.hpp>
#include <pd/base/Allocator.hpp>
#include <pd/maths/NVec.hpp>
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

using PDLithiumFlags = unsigned int;

enum PDLithiumFlags_ {
  PDLithiumFlags_None = 0,
  PDLithiumFlags_TMS = MAKEFLAG(0),  // Text Map System
  PDLithiumFlags_LRS = MAKEFLAG(1),  // Layer Render System
  PDLithiumFlags_FCS = MAKEFLAG(2),  // Floor Coords System
  PDLithiumFlags_Default = PDLithiumFlags_TMS,
};

namespace Palladium {
class LIFont {
 public:
  struct CPI {
    CPI()
        : codepoint(0),
          uv(NVec4()),
          tex(nullptr),
          szs(NVec2()),
          off(0.f),
          invalid(false) {}
    CPI(bool iv)
        : codepoint(0),
          uv(NVec4()),
          tex(nullptr),
          szs(NVec2()),
          off(0.f),
          invalid(iv) {}
    unsigned int codepoint;
    NVec4 uv;
    Texture::Ref tex;
    NVec2 szs;
    float off;
    bool invalid;
  };
  LIFont() = default;
  ~LIFont() = default;
  PD_SMART_CTOR(LIFont)

  void LoadTFF(const std::string path, int px_size = 32);
  void LoadBitmapFont(const std::string& path);
  void LoadSystemFont();

  int GetPixelHeight();
  CPI GetCodepoint(unsigned int c);
  std::string GetName() { return name; };

  bool IsSystemFont() { return sysfnt; }

  void Dump();

 private:
  bool sysfnt = false;
  std::string name;
  int pixel_height;
  std::map<unsigned int, CPI> cpmap;
  std::vector<Texture::Ref> tex;
};

class LI {
 public:
  struct Vtx {
    Vtx() {}
    Vtx(NVec2 xy, float u, float v, unsigned int clr) {
      // Coords
      pos[0] = xy[0];
      pos[1] = xy[1];
      pos[2] = 0.f;
      // UV
      uv[0] = u;
      uv[1] = v;
      col = clr;
    }
    NVec3 pos;
    NVec2 uv;
    unsigned int col;
  };
  /// CMD TYPES ///
  /// 0 = SKIP
  /// 2 = TRIANGLE
  /// 1 = RECT
  /// 3 = Circle
  /////////////////
  struct Cmd {
    NVec4 top;
    NVec4 bot;
    NVec4 uv;
    int layer = 0;
    int cmd_type = 0;
    bool fcs = false; // Floor Coords System
    unsigned int clr = 0;
    bool sfr = false;  // SysFontRender
    Texture::Ref tex = nullptr;
    int index = 0;
  };
  /// Text Box ///
  /// System to Make GetTextDiemnsions
  /// and Short/Wrap faster
  struct TextBox {
    NVec2 size;
    float time_created;
    // Optional
    bool optinal = false;
    std::string text;
  };
  LI() = default;
  ~LI() = default;

  static void Init();
  static void Exit();

  // Settings
  static void EnableFeature(PDLithiumFlags flag) { flags |= flag; }
  static void DisableFeature(PDLithiumFlags flag) { flags &= ~flag; }
  static PDLithiumFlags& GetFeatures() { return flags; }

  static void OnScreen(bool bottom);
  static void Render(C3D_RenderTarget* top, C3D_RenderTarget* bot);
  static NVec2 GetScreenSize() { return screen_size; }
  static LIFont::Ref GetFont() { return font; }
  static void SetFont(LIFont::Ref i) {
    font_update = true;
    font = i;
  }
  static void UseTexture(Texture::Ref tex = nullptr) {
    active_texture = tex ? tex : single_color;
  }
  static bool IsBottomScreen() { return bottom_screen; }
  static float GetTextScale() { return text_scale; }
  static void SetTextScale(float scale) {
    font_update = true;
    text_scale = scale;
  }
  static void DefaultTextScale() {
    font_update = true;
    text_scale = default_text_size;
  }
  static void Layer(int v) { layer = v; }
  static int Layer() { return layer; }
  static void NewLayer() { layer++; }
  static NVec2 GetTextDimensions(const std::string& text);
  static std::string ShortText(const std::string& in, int maxlen, NVec2& dim);
  static std::string WrapText(const std::string& in, int maxlen, NVec2& dim);

  // Drawing Functions
  static void DrawRect(NVec2 pos, NVec2 size, unsigned int color, NVec4 uvs);
  static void DrawLine(NVec2 a, NVec2 b, unsigned int clr, int t = 1);
  static void DrawCircle(NVec2 pos, float r, unsigned int color, int segments);
  static void DrawRect(NVec2 pos, NVec2 size, unsigned int color) {
    UseTexture();
    DrawRect(pos, size, color, NVec4(0, 1, 1, 0));
  }
  static void DrawTriangle(NVec2 a, NVec2 b, NVec2 c, unsigned int color);
  static void DrawImage(NVec2 pos, Texture::Ref tex, NVec2 size, NVec4 uvs) {
    UseTexture(tex);
    DrawRect(pos, size, 0xffffffff, uvs);
  }
  static void DrawText(NVec2 pos, unsigned int color, const std::string& text,
                       PDTextFlags flags = 0, NVec2 ap = NVec2());

  static int Vertices() { return num_vertices; }
  static int Indices() { return num_indices; }
  static int Drawcalls() { return num_drawcalls; }
  static int DarwCommands() { return num_commands; }
  static size_t GetMaxVerticesNum() { return vertex_buffer.size(); }

 private:
  static void RotateCorner(NVec2& v, float s, float c);
  static void MakeRect(NVec4& top, NVec4& bot, NVec2 pos, NVec2 szs,
                       float angle = 0.f);
  static bool CompareCommands(const Cmd& a, const Cmd& b);
  static void RenderFrame(bool bottom);

  /// CTX ///
  static PDLithiumFlags flags;
  // Font Stuff
  // Default Font Size in (px)
  static const float default_font_size;
  static const float default_text_size;
  static float text_scale;

  // Renderer Stuff
  static NVec2 screen_size;
  static bool bottom_screen;
  static int layer;
  static std::vector<Cmd> draw_lists[2];
  static Texture::Ref active_texture;
  static Texture::Ref single_color;
  static std::vector<Vtx, LinearAllocator<Vtx>> vertex_buffer;
  static std::vector<unsigned short, LinearAllocator<unsigned short>>
      idx_buffer;
  static size_t vertex_index;
  static size_t idx_index;
  static LIFont::Ref font;
  static bool font_update;
  static bool sysfont_render;
  static std::map<std::string, TextBox> text_sizes;
  static int cmd_index;

  // Debug
  static int num_vertices;
  static int num_drawcalls;
  static int num_commands;
  static int num_indices;

  // Shader
  static DVLB_s* li7_dvlb;
  static shaderProgram_s li7_prog;
  static C3D_AttrInfo li7_attr;
  static int uLoc_proj;
};
}  // namespace Palladium