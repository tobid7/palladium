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
#include <pd/lithium/flags.hpp>
#include <pd/lithium/texture.hpp>
#include <pd/lithium/vertex.hpp>
#include <pd/core/vec.hpp>

namespace PD {
namespace LI {
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
}  // namespace LI
}  // namespace PD