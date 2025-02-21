#pragma once

/*
MIT License

Copyright (c) 2024 - 2025 tobid7

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

#include <pd/core/common.hpp>
#include <pd/lib3ds/memory.hpp>
#include <pd/lithium/command.hpp>
#include <pd/lithium/flags.hpp>
#include <pd/lithium/font.hpp>
#include <pd/lithium/objects.hpp>
#include <pd/lithium/screen.hpp>
#include <pd/lithium/texture.hpp>
#include <pd/lithium/vertex.hpp>
#include <pd/maths/vec.hpp>

namespace PD {
namespace LI {
class Renderer : public SmartCtor<Renderer> {
 public:
  Renderer(LIRenderFlags flags = LIRenderFlags_Default);
  ~Renderer();

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
  LIRenderFlags& GetFlags() { return flags; }
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
  std::string ShortText(const std::string& text, int maxlen, vec2& newsize);
  std::string WrapText(const std::string& text, int maxlen, vec2& newsize);

 private:
  /// Helper Funcitons ///
  void UpdateRenderMode(const RenderMode& mode);

  /// One Screen Only... ///
  Screen::Ref screen;
  /// Reference Screens ///
  Screen::Ref screens[2];

  /// Context Related ///
  LIRenderFlags flags = LIRenderFlags_Default;
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