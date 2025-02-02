#pragma once

/*
MIT License

Copyright (c) 2024 tobid7

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

#include <pd/common/common.hpp>
#include <pd/common/memory.hpp>
#include <pd/graphics/rect.hpp>
#include <pd/graphics/screen.hpp>
#include <pd/graphics/texture.hpp>
#include <pd/maths/vec.hpp>
#include <pd/tools/markdown.hpp>

using LITextFlags = u32;

enum LITextFlags_ {
  LITextFlags_None = 0,
  LITextFlags_AlignRight = 1 << 0,
  LITextFlags_AlignMid = 1 << 1,
  LITextFlags_Shaddow = 1 << 2,   // Draws the text twice
  LITextFlags_Wrap = 1 << 3,      // May be runs better with TMS
  LITextFlags_Short = 1 << 4,     // May be runs better with TMS
  LITextFlags_Scroll = 1 << 5,    // Not implemented
  LITextFlags_RenderOOS = 1 << 6  // Render Out of Screen
};

namespace PD {
namespace LI {
class Font : public SmartCtor<Font> {
 public:
  class Codepoint {
   public:
    Codepoint() {}
    ~Codepoint() {}

    u32 cp() const { return m_cp; }
    Codepoint& cp(u32 v) {
      m_cp = v;
      return *this;
    }
    vec4 uv() const { return m_uv; }
    Codepoint& uv(const vec4& v) {
      m_uv = v;
      return *this;
    }
    Texture::Ref tex() const { return m_tex; }
    Codepoint& tex(Texture::Ref v) {
      m_tex = v;
      return *this;
    }
    vec2 size() const { return m_size; }
    Codepoint& size(const vec2& v) {
      m_size = v;
      return *this;
    }
    float off() const { return m_off; }
    Codepoint& off(float v) {
      m_off = v;
      return *this;
    }
    bool invalid() const { return m_invalid; }
    Codepoint& invalid(bool v) {
      m_invalid = v;
      return *this;
    }

   private:
    u32 m_cp = 0;
    vec4 m_uv;
    Texture::Ref m_tex = nullptr;
    vec2 m_size;
    float m_off = 0;
    bool m_invalid = false;
  };
  Font() {}
  ~Font() {}
  void LoadTTF(const std::string& path, int px_height = 32);
  void LoadSystemFont();
  int PixelHeight() const { return pixel_height; }
  Codepoint& GetCodepoint(u32 c);
  bool SystemFont() const { return sysfont; }

 private:
  bool sysfont;
  int pixel_height;
  std::vector<Texture::Ref> textures;
  std::map<u32, Codepoint> cpmap;
};
class Vertex {
 public:
  Vertex() {}
  Vertex(const vec2& p, const vec2& u, u32 c) {
    pos[0] = p[0];
    pos[1] = p[1];
    uv = u;
    color = c;
  }
  ~Vertex() {}

  Vertex& Pos(const vec2& v) {
    pos = v;
    return *this;
  }
  Vertex& Uv(const vec2& v) {
    uv = v;
    return *this;
  }
  Vertex& Color(u32 v) {
    color = v;
    return *this;
  }

  // private:
  vec2 pos;
  vec2 uv;
  u32 color;
};
/// @brief Required to Set the TexENV
enum RenderMode {
  RenderMode_RGBA,
  RenderMode_Font,
};
/// @brief Reform the Drawcommand by generating the Vertexbuffer into it
class Command : public SmartCtor<Command> {
 public:
  Command() {}
  ~Command() {}

  Command(Command::Ref v) {
    this->index = v->index;
    this->index_buf = v->index_buf;
    this->layer = v->layer;
    this->mode = v->mode;
    this->tex = v->tex;
    this->vertex_buf = v->vertex_buf;
  }

  Command& Layer(int v) {
    layer = v;
    return *this;
  }

  int Layer() const { return layer; }

  Command& Index(int v) {
    index = v;
    return *this;
  }

  int Index() const { return index; }

  Command& Tex(Texture::Ref v) {
    tex = v;
    return *this;
  }

  Texture::Ref Tex() const { return tex; }

  Command& PushVertex(const Vertex& v) {
    vertex_buf.push_back(v);
    return *this;
  }

  const std::vector<u16>& IndexList() const { return index_buf; }
  const std::vector<Vertex>& VertexList() const { return vertex_buf; }

  /// ADVANCED ///
  std::vector<u16>& IndexList() { return index_buf; }
  std::vector<Vertex>& VertexList() { return vertex_buf; }

  Command& PushIndex(u16 v) {
    index_buf.push_back(vertex_buf.size() + v);
    return *this;
  }

  Command& Rendermode(const RenderMode& v) {
    mode = v;
    return *this;
  }

  RenderMode Rendermode() const { return mode; }

 private:
  /// Using Default std::vector here
  std::vector<Vertex> vertex_buf;
  std::vector<u16> index_buf;
  int layer;
  Texture::Ref tex;
  int index;
  RenderMode mode = RenderMode_RGBA;
};
class TextBox {
 public:
  TextBox() {}
  TextBox(const vec2& s, float time) {
    size = s;
    time_created = time;
    optional = false;
  }
  ~TextBox() {}

  void TimeCreated(float v) { time_created = v; }
  void Size(const vec2& v) { size = v; }
  void Optional(bool v) { optional = v; }
  void Text(const std::string& v) { text = v; }

  vec2 Size() const { return size; }
  float TimeCreated() const { return time_created; }
  bool Optional() const { return optional; }
  std::string Text() const { return text; }

 private:
  vec2 size;
  float time_created;
  bool optional;
  std::string text;  // TextWrap
};
class StaticObject : public SmartCtor<StaticObject> {
 public:
  StaticObject() {}
  ~StaticObject() {}

  void PushCommand(Command::Ref v) { cmds.push_back(v); }

  void ReCopy() {
    cpy.clear();
    for (auto it : cmds) {
      cpy.push_back(Command::New(it));
    }
  }

  void ReColorQuad(int idx, u32 col) {
    if (idx > (int)cpy.size()) {
      return;
    }
    for (auto& it : cpy[idx]->VertexList()) {
      it.Color(col);
    }
  }

  void MoveIt(vec2 off) {
    for (auto& it : cpy) {
      for (auto& jt : it->VertexList()) {
        jt.pos += off;
      }
    }
  }

  void ReColor(u32 col) {
    for (auto& it : cpy) {
      for (auto& jt : it->VertexList()) {
        jt.Color(col);
      }
    }
  }

  void ReLayer(int base_layer) {
    for (auto& it : cpy) {
      it->Layer(it->Layer() + base_layer);
    }
  }

  void ReIndex(int start) {
    for (int i = 0; i < (int)cpy.size(); i++) {
      cpy[i]->Index(start + i);
    }
  }

  std::vector<Command::Ref>& List() {
    if (cpy.size() != 0) {
      return cpy;
    }
    return cmds;
  }

 private:
  std::vector<Command::Ref> cpy;
  std::vector<Command::Ref> cmds;
};

using RenderFlags = u32;
enum RenderFlags_ {
  RenderFlags_None = 0,
  RenderFlags_TMS = 1 << 0,  ///< Text Map System
  RenderFlags_LRS = 1 << 1,  ///< Layer Render System
  RenderFlags_AST = 1 << 2,  ///< Auto Static Text
  RenderFlags_Default = RenderFlags_TMS | RenderFlags_LRS | RenderFlags_AST,
};
class Renderer : public SmartCtor<Renderer> {
 public:
  Renderer(RenderFlags flags = RenderFlags_Default);
  ~Renderer();

  class StaticText : public SmartCtor<StaticText> {
   public:
    StaticText() {}
    StaticText(Renderer* ren, const vec2& pos, u32 clr, const std::string& text,
               LITextFlags flags = 0, const vec2& box = 0) {
      Setup(ren, pos, clr, text, flags, box);
    }
    ~StaticText() {}

    void Setup(Renderer* ren, const vec2& pos, u32 clr, const std::string& text,
               LITextFlags flags = 0, const vec2& box = 0);

    vec2 GetDim() const { return tdim; }
    vec2 GetPos() const { return pos; }

    void SetColor(u32 col);
    void SetPos(const vec2& pos);
    void SetLayer(int l);

    void SetUnused() { used = false; }
    bool Used() const { return used; }

    bool IsSetup() { return text != nullptr; }

    void Draw();

   private:
    bool used;
    Renderer* ren;
    vec2 tdim;
    vec2 pos;
    StaticObject::Ref text;
  };

  void PrepareRender();
  void Render(Screen::Ref s);
  void FinalizeRender();

  void RegisterScreen(bool bottom, Screen::Ref s) { screens[bottom] = s; }
  void OnScreen(Screen::Ref s) {
    if (!s) {
      return;
    }
    this->screen = s;
    area_size = screen->GetSize();
  }

  Screen::Ref CurrentScreen() const { return screen; }
  Screen::Ref GetScreen(bool bottom) {
    auto res = screens[bottom];
    Assert(res.get(), "Screen is not registered!");
    return res;
  }

  void Rotation(float v) { rot = v; }
  float Rotation() const { return rot; }
  void TextScale(float v) { text_size = v; }
  void DefaultTextScale() { text_size = default_text_size; }
  float TextScale() const { return text_size; }
  void Layer(int v) { current_layer = v; }
  int Layer() const { return current_layer; }
  RenderFlags& GetFlags() { return flags; }
  void Font(Font::Ref v) {
    font = v;
    font_update = true;
  }
  Font::Ref Font() const { return font; }

  void UseTex(Texture::Ref v = nullptr) {
    if (v == nullptr) {
      current_tex = white;
      return;
    }
    current_tex = v;
  }

  /// @brief Draws a Rect based on current Texture
  /// @param pos Pos
  /// @param size Size
  /// @param color Color
  /// @param uv UV Map
  void DrawRect(const vec2& pos, const vec2& size, u32 color,
                const Rect& uv = vec4(0.f, 1.f, 1.f, 0.f));
  /// @brief Draw a Solid Rect (uses white tex)
  /// @note acts as a simplified Draw rect Wrapper
  /// @param pos Position
  /// @param size Size
  /// @param color Color
  void DrawRectSolid(const vec2& pos, const vec2& size, u32 color);
  /// @brief Render a Triangle
  /// @param a Position Alpha
  /// @param b Position Bravo
  /// @param c Position Delta
  /// @param color Color
  /// @note Defaults to Solif Color
  void DrawTriangle(const vec2& a, const vec2& b, const vec2& c, u32 color);
  /// @brief Draw a Circle (Supports Textures)
  /// @param center_pos Center Position
  /// @param r Radius
  /// @param color Color
  /// @param segments Segments to use
  /// @note Textures could look a bit janky due to uv mapping
  void DrawCircle(const vec2& center_pos, float r, u32 color, int segments);
  /// @brief Draw a Line between to Positions
  /// @param a Position Alpha
  /// @param b Position Bravo
  /// @param color Color
  /// @param t Thickness
  void DrawLine(const vec2& a, const vec2& b, u32 color, int t);
  void DrawText(const vec2& pos, u32 color, const std::string& text,
                u32 flags = 0, const vec2& ap = vec2());
  /// @brief Draw a Texture as 2D Image
  /// @param pos Position
  /// @param tex Texture reference
  /// @param scale Scale (cause maybe wants to be resized)
  /// @note Acts as a Simplified wrapper to DrawRect
  void DrawImage(const vec2& pos, Texture::Ref tex,
                 const vec2& scale = vec2(1.f));

  /// Debug STUFF
  u32 Vertices() const { return vertices; }
  u32 Indices() const { return indices; }
  u32 Commands() const { return commands; }
  u32 DrawCalls() const { return drawcalls; }
  u32 AstUsage() const { return ast.size(); }
  u32 TmsUsage() const { return tms.size(); }

  /// TOOLS ///
  static void RotateCorner(vec2& v, float s, float c);
  static Rect CreateRect(const vec2& pos, const vec2& size, float angle);
  static Rect CreateLine(const vec2& a, const vec2& b, int t);
  static bool InBox(const vec2& pos, const vec2& size, const vec4& rect);
  static bool InBox(const vec2& pos, const vec4& rect);
  static bool InBox(const vec2& alpha, const vec2& bravo, const vec2& charlie,
                    const vec4& rect);
  /// @brief Get The Address of a Screen
  /// @note IMPORTANT: THIS IS FOR 32Bit System
  /// Should find a better way to do this for porting this lib
  static u32 Screen32(Screen::Ref s) { return (u32)s.get(); }
  static void OptiCommandList(std::vector<Command::Ref>& list);
  /// @brief Returns Viewport with xy
  vec4 GetViewport();
  /// @brief Push a Self Created command
  void PushCommand(Command::Ref cmd) {
    cmd->Index(cmd_idx++);  // Indexing
    draw_list[Screen32(screen)].push_back(cmd);
  }
  /// @brief Automatically sets up a command
  void SetupCommand(Command::Ref cmd);
  /// @brief Creates a default Quad Render Command
  void QuadCommand(Command::Ref cmd, const Rect& quad, const Rect& uv, u32 col);
  /// @brief Create a Default Triangle
  void TriangleCommand(Command::Ref cmd, const vec2& a, const vec2& b,
                       const vec2& c, u32 col);
  /// @brief Create List of a Text Commands
  /// @param cmds Link to a command List
  /// @param pos Position
  /// @param color Color
  /// @param text Text
  /// @param flags Flags
  /// @param box (Size for wrapping / Offset for Centered Text)
  /// @note Funktion macht noch faxxen (Text nicht sichtbar)
  void TextCommand(std::vector<Command::Ref>& cmds, const vec2& pos, u32 color,
                   const std::string& text, LITextFlags flags, const vec2& box);
  vec2 GetTextDimensions(const std::string& text);

 private:
  /// Helper Funcitons ///
  void UpdateRenderMode(const RenderMode& mode);

  /// One Screen Only... ///
  Screen::Ref screen;
  /// Reference Screens ///
  Screen::Ref screens[2];

  /// Context Related ///
  RenderFlags flags = RenderFlags_Default;
  vec2 area_size;
  int current_layer = 0;
  Texture::Ref current_tex = nullptr;
  Texture::Ref white = nullptr;  // Single color
  Font::Ref font = nullptr;
  bool font_update;
  RenderMode mode = RenderMode_RGBA;
  // Text Map System
  std::map<std::string, TextBox> tms;
  // (Auto) Static Text
  std::unordered_map<u32, StaticText::Ref> ast;
  /// Text Rendering ///
  const float default_font_h = 24.f;
  const float default_text_size = 0.7f;
  float text_size = 0.7f;
  /// Special ///
  float rot = 0.f;
  /// Rendering ///
  // Use dual drawlist
  std::unordered_map<u32, std::vector<Command::Ref>> draw_list;
  std::vector<Vertex, LinearAllocator<Vertex>> vertex_buf;
  std::vector<u16, LinearAllocator<u16>> index_buf;
  u32 vertex_idx = 0;
  u32 index_idx = 0;
  u32 cmd_idx = 0;

  /// Debug ///
  u32 vertices = 0;
  u32 indices = 0;
  u32 commands = 0;
  u32 drawcalls = 0;

  /// Shader
  DVLB_s* dvlb = nullptr;
  shaderProgram_s shader;
  C3D_AttrInfo attr;
  int uLoc_projection = 0;

  /// Matrix
  C3D_Mtx top_proj;
  C3D_Mtx bot_proj;
};
}  // namespace LI
}  // namespace PD