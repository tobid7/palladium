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
#include <pd/core/vec.hpp>
#include <pd/lib3ds/memory.hpp>
#include <pd/lithium/command.hpp>
#include <pd/lithium/flags.hpp>
#include <pd/lithium/font.hpp>
#include <pd/lithium/objects.hpp>
#include <pd/lithium/screen.hpp>
#include <pd/lithium/texture.hpp>
#include <pd/lithium/vertex.hpp>

namespace PD {
namespace LI {
/**
 * Lithium base renderer Class.
 */
class Renderer : public SmartCtor<Renderer> {
 public:
  /**
   * Constructor setting up the 2d Rendering Engine
   * @param flags Flags to use [can be changed at runtime].
   */
  Renderer(LIRenderFlags flags = LIRenderFlags_Default);
  /**
   * Deconstructor that unloads all the renderer data
   */
  ~Renderer();

  /**
   * Prepare render stage for rendering.
   */
  void PrepareRender();
  /**
   * Render a screens Command list.
   * @param s Screen to Draw its list on.
   */
  void Render(Screen::Ref s);
  /**
   * Finalize rendering stage.
   */
  void FinalizeRender();

  /**
   * Register Screens (For UI7, probably move this).
   * @param bottom set if you register bottom ot top screen.
   * @param s Screen to register.
   */
  void RegisterScreen(bool bottom, Screen::Ref s) { screens[bottom] = s; }
  /**
   * Set the Screen next commands will be add to
   * @param s Screen of choice
   */
  void OnScreen(Screen::Ref s) {
    if (!s) {
      return;
    }
    this->screen = s;
    area_size = screen->GetSize();
  }

  /**
   * Get cobst reference to active screen.
   * @return current screen.
   */
  Screen::Ref CurrentScreen() const { return screen; }
  /**
   * Get Screen of screen regestry.
   * @param bottom bottom ot top screen.
   * @return screen reference
   */
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

  /**
   * Use a specific Texture for Next Rectangle.
   * @param v texture reference to use.
   */
  void UseTex(Texture::Ref v = nullptr) {
    if (v == nullptr) {
      current_tex = white;
      return;
    }
    current_tex = v;
  }

  /**
   * Draws a Rect based on current Texture
   * @param pos Position of the Rect
   * @param size Size of the Rect
   * @param color Color of the Rect [0xffffffff]
   * for full visible texture
   * @param uv UV Map (if texture needs a custom uv)
   * @note This function is part of Simple Draw API.
   */
  void DrawRect(const vec2& pos, const vec2& size, u32 color,
                const Rect& uv = vec4(0.f, 1.f, 1.f, 0.f));
  /**
   *  Draw a Solid Rect (uses white tex)
   * @note
   * - acts as a simplified Draw rect Wrapper
   * - This function is part of Simple Draw API.
   * @param pos Position of the rect
   * @param size Size of the rect
   * @param color Color of the rect
   */
  void DrawRectSolid(const vec2& pos, const vec2& size, u32 color);
  /**
   *  Render a Triangle
   * @param a Position A
   * @param b Position B
   * @param c Position C
   * @param color Color of the triangle
   * @note
   * - Uses Solid color [white tex]
   * - This function is part of Simple Draw API.
   */
  void DrawTriangle(const vec2& a, const vec2& b, const vec2& c, u32 color);
  /**
   *  Draw a Circle (Supports Textures)
   * @param center_pos Center Position
   * @param r Radius of the circle
   * @param color Color of the circle
   * @param segments Segments to use
   * @note
   * - Textures could look a bit janky due to uv mapping
   * - This function is part of Simple Draw API.
   */
  void DrawCircle(const vec2& center_pos, float r, u32 color, int segments);
  /**
   * Draw a Line between to Positions
   * @param a Position A
   * @param b Position B
   * @param color Color of the line
   * @param t Thickness
   * @note This function is part of Simple Draw API.
   */
  void DrawLine(const vec2& a, const vec2& b, u32 color, int t);
  /**
   * Render a Text
   * @param pos Position of the text
   * @param color Color of the Text
   * @param text Text to Render
   * @param flags flags to use
   * @param ap optional size for specific flags
   * @note This function is part of Simple Draw API.
   */
  void DrawText(const vec2& pos, u32 color, const std::string& text,
                u32 flags = 0, const vec2& ap = vec2());
  /**
   * Draw a Texture as 2D Image
   * @param pos Position
   * @param tex Texture reference
   * @param scale Scale (cause maybe wants to be resized)
   * @note
   * - Acts as a Simplified wrapper to DrawRect
   * - This function is part of Simple Draw API.
   */
  void DrawImage(const vec2& pos, Texture::Ref tex,
                 const vec2& scale = vec2(1.f));

  // Debug STUFF

  /** DEBUG Get the Number of Vertices of last Frame */
  u32 Vertices() const { return vertices; }
  /** DEBUG Get the Number of Indices of last frame */
  u32 Indices() const { return indices; }
  /** DEBUG Get the Number of Commands of last frame */
  u32 Commands() const { return commands; }
  /** DEBUG Get the Number of Drawcalls of last frame */
  u32 DrawCalls() const { return drawcalls; }
  /** Auto Static Text Number of Texts */
  u32 AstUsage() const { return ast.size(); }
  /** Text Map System number of texts */
  u32 TmsUsage() const { return tms.size(); }

  // TOOLS

  /** Rotate a rect corner position by sine and cosine value */
  static void RotateCorner(vec2& v, float s, float c);
  /** Create a Rect by Position, Size and rotation angle */
  static Rect CreateRect(const vec2& pos, const vec2& size, float angle);
  /** Create a Line Rect by 2 Position and a thickness value */
  static Rect CreateLine(const vec2& a, const vec2& b, int t);
  /** Check if a pos and size are inside a vec4 rect */
  static bool InBox(const vec2& pos, const vec2& size, const vec4& rect);
  /** Check if a pos is inside a vec4 rect */
  static bool InBox(const vec2& pos, const vec4& rect);
  /** Check if one or all of three positions are somehow inside a vec4 rect */
  static bool InBox(const vec2& alpha, const vec2& bravo, const vec2& charlie,
                    const vec4& rect);
  /**
   * Get The Address of a Screen
   * @note **IMPORTANT** THIS IS FOR 32Bit System
   *
   * Should find a better way to do this for porting this lib
   */
  static u32 Screen32(Screen::Ref s) { return (u32)s.get(); }
  /** Function to optimize command order for rendering */
  static void OptiCommandList(std::vector<Command::Ref>& list);
  /** Returns Viewport with xy */
  vec4 GetViewport();
  /** Push a Self Created command */
  void PushCommand(Command::Ref cmd) {
    cmd->Index(cmd_idx++);  // Indexing
    draw_list[Screen32(screen)].push_back(cmd);
  }
  /** Automatically sets up a command */
  void SetupCommand(Command::Ref cmd);
  /** Creates a default Quad Render Command */
  void QuadCommand(Command::Ref cmd, const Rect& quad, const Rect& uv, u32 col);
  /** Create a Default Triangle */
  void TriangleCommand(Command::Ref cmd, const vec2& a, const vec2& b,
                       const vec2& c, u32 col);
  /**
   * Create List of a Text Commands
   * @param cmds Link to a command List
   * @param pos Position
   * @param color Color
   * @param text Text
   * @param flags Flags
   * @param box (Size for wrapping / Offset for Centered Text)
   */
  void TextCommand(std::vector<Command::Ref>& cmds, const vec2& pos, u32 color,
                   const std::string& text, LITextFlags flags, const vec2& box);
  vec2 GetTextDimensions(const std::string& text);
  /**
   * Function to short a text and replace the rest by ...
   * @param text Text to short
   * @param maxlen Maximum width
   * @param newsize New Textbox size
   * @return shorted text
   */
  std::string ShortText(const std::string& text, int maxlen, vec2& newsize);
  /**
   * Function to Wrap Text by specific width
   *
   * **NOT IMPLEMENTED YET**
   * @param text text to wrap
   * @param maxlen maximum width per line
   * @param newsize new textbox size
   * @return wrapped text
   */
  std::string WrapText(const std::string& text, int maxlen, vec2& newsize);

 private:
  // Helper Funcitons

  /**
   * Update the 3DS citro3d texenv for specific RenderMode
   * @param mode RenderMode to use.
   */
  void UpdateRenderMode(const RenderMode& mode);

  /** Current Screen */
  Screen::Ref screen;
  /** Screen Regestry */
  Screen::Ref screens[2];

  // Context Related

  /** Renderer flags */
  LIRenderFlags flags = LIRenderFlags_Default;
  /** Area Size */
  vec2 area_size;
  /** Current Layer */
  int current_layer = 0;
  /** Current Texture */
  Texture::Ref current_tex = nullptr;
  /** Single Color Texture (for solid color objs) */
  Texture::Ref white = nullptr;
  /** Current Font Reference */
  Font::Ref font = nullptr;
  /** Font updated */
  bool font_update;
  /** Current Rendermode */
  RenderMode mode = RenderMode_RGBA;
  /** Text Map System */
  std::map<std::string, TextBox> tms;
  /** (Auto) Static Text */
  std::unordered_map<u32, StaticText::Ref> ast;

  // Text Rendering

  /** Default FOnt height */
  const float default_font_h = 24.f;
  /** Default Text Scale */
  const float default_text_size = 0.7f;
  /** Current Text Scale */
  float text_size = 0.7f;
  // Special

  /** Current Rotation (RECTS) */
  float rot = 0.f;
  // Rendering

  /** Map of drawlist by 32Bit memory Address of their Screen */
  std::unordered_map<u32, std::vector<Command::Ref>> draw_list;
  /** Vertex Buffer in linear Ram */
  std::vector<Vertex, LinearAllocator<Vertex>> vertex_buf;
  /** 16Bit index buffer in linear Ram */
  std::vector<u16, LinearAllocator<u16>> index_buf;
  /** vertex index (render stage) */
  u32 vertex_idx = 0;
  /** index buf index (render stage) */
  u32 index_idx = 0;
  /** command index (used for debug count) */
  u32 cmd_idx = 0;

  // Debug

  /** Num of Vertices */
  u32 vertices = 0;
  /** Num of indices */
  u32 indices = 0;
  /** Num of Commands */
  u32 commands = 0;
  /** Num of Drawcalls */
  u32 drawcalls = 0;

  // Shader

  /** Shader code */
  DVLB_s* dvlb = nullptr;
  /** Shader program */
  shaderProgram_s shader;
  /** Shader Attribute info */
  C3D_AttrInfo attr;
  /** projection matrix location */
  int uLoc_projection = 0;

  // Matrix

  /** Precalculated Projectrion Matrix for top screen */
  C3D_Mtx top_proj;
  /** Precalculated Projectrion Matrix for bottom screen */
  C3D_Mtx bot_proj;
};
}  // namespace LI
}  // namespace PD