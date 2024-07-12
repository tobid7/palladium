#include <pd/NVec.hpp>
#include <pd/Allocator.hpp>
#include <pd/Texture.hpp>
#include <citro3d.h>

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include <vector>

namespace Palladium {
class LIFont {
    public:
    struct CPI {
        unsigned char codepoint;
        NVec4 uv;
        Texture::Ref tex;
    };
    LIFont() = default;
    ~LIFont() = default;
    PD_SMART_CTOR(LIFont)

    void LoadTFF(const std::string path, int px_size = 32);
    void LoadBitmapFont(const std::string& path);
    void LoadSystemFont();

    int GetPixelHeight();
    CPI GetCodepoint();

    private:
    int pixel_height;
    unsigned char fontw[256];
    int charsize;
    std::vector<Texture::Ref> tex;
};
class LI7 {
 public:
 struct Vtx
{
    float xyz[3];
    float uv[2];
    unsigned int col;
};
/// CMD TYPES ///
/// 0 = SKIP
/// 1 = TRIANGLE
/// 2 = RECT
/////////////////
struct Cmd {
    NVec2 ppos;
    NVec2 pszs;
    NVec2 apos;
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

  static void ColorRect(NVec2 pos, NVec2 szs, NVec4 uvs, unsigned int clr);
  static void Rect(NVec2 pos, NVec2 szs, NVec4 uvs);
  static void ColorRect(NVec2 pos, NVec2 szs, unsigned int clr);
  static void Rect(NVec2 pos, NVec2 szs);
  static void TexCutRect(NVec2 pos, NVec2 szs, NVec2 cb, NVec2 ce);

  static int DrawText(int x, int y, int z, unsigned int color, bool shadow,
                      int wrap, int* ySize, const char* fmt, ...);

  static int Vertices() { return m_d_vertices; }
  static int Drawcalls() { return m_d_drawcalls; }
  static int DarwCommandss() { return m_d_commands; }
 private:
  static bool CompareCommands(const Cmd& a,
                              const Cmd& b);
  static void RenderFrame(bool bottom);
  static int DrawTextVargs(int x, int y, int z, unsigned int color, bool shadow,
                           int wrap, int* ySize, const char* fmt, va_list arg);

  // Default Font Size in (px)
  static const float m_dffs;

  static int m_uLoc_proj;
  static float m_scale;
  static int m_width, m_height;

  static int m_d_vertices;
  static int m_d_drawcalls;
  static int m_d_commands;

  static const int m_char_height;  // Constant
  static float m_rot;

  // UI Stuff
  static std::vector<Cmd> m_top_draw_cmds;
  static std::vector<Cmd> m_bot_draw_cmds;
  static Texture::Ref m_current_texture;
  static std::vector<Vtx, LinearAllocator<Vtx>> m_vtx_list[2];
  //static Font* m_font;
  static std::vector<char> m_text_buffer;

  // Matrix
  static C3D_Mtx m_icon_model_matrix;

  // Ctx Stuff
  static bool m_bottom_active;

  // Shader
  static DVLB_s *li7_dvlb;
  static shaderProgram_s li7_shader;
  static C3D_AttrInfo li7_attr;
};
}