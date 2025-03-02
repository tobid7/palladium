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
#include <pd/lithium/flags.hpp>
#include <pd/lithium/texture.hpp>
#include <pd/lithium/vertex.hpp>

namespace PD {
namespace LI {
/**
 * Lithium Draw Command (containing a list of vertex and index data
 * only for this specific command itself)
 */
class Command : public SmartCtor<Command> {
 public:
  Command() = default;
  ~Command() = default;

  /**
   * Copy Constructor [to Copy the Data of a Command Reference]
   */
  Command(Command::Ref v) {
    this->index = v->index;
    this->index_buf = v->index_buf;
    this->layer = v->layer;
    this->mode = v->mode;
    this->tex = v->tex;
    this->vertex_buf = v->vertex_buf;
  }

  /**
   * Setter for the Commands layer
   * @param v layer value
   * @return command reference
   */
  Command& Layer(int v) {
    layer = v;
    return *this;
  }

  /**
   * Getter for the Layer
   * @return layer value
   */
  int Layer() const { return layer; }

  /**
   * Setter for the Commands Index [used in sorting]
   * @param v index value
   * @return command reference
   */
  Command& Index(int v) {
    index = v;
    return *this;
  }
  /**
   * Getter for the Index
   * @return index value
   */
  int Index() const { return index; }

  /**
   * Setter for the Commands Texture
   * @param v Texture reference
   * @return command reference
   */
  Command& Tex(Texture::Ref v) {
    tex = v;
    return *this;
  }
  /**
   * Getter for the Texture reference
   * @return Texture reference
   */
  Texture::Ref Tex() const { return tex; }

  /**
   * Function to Push a Vertex to the vertexbuffer
   * @param v Vertex to push
   * @return command reference
   */
  Command& PushVertex(const Vertex& v) {
    vertex_buf.push_back(v);
    return *this;
  }

  /**
   * Access to the Index list [used to write index data
   * to the real indexbuffer]
   * @return const reference to commands idx buffer
   */
  const std::vector<u16>& IndexList() const { return index_buf; }
  /**
   * Access to the Vertex list [used to write vertices
   * data to the real vertexbuffer]
   * @return const reference to commands vertex buffer
   */
  const std::vector<Vertex>& VertexList() const { return vertex_buf; }

  // ADVANCED

  /**
   * Advanced function to access index list
   *
   * - This function is UNSAFE to use cause it allows to modify index data
   * @return reference to index list
   */
  std::vector<u16>& IndexList() { return index_buf; }
  /**
   * Advanced function to access vertex list
   *
   * - This function is UNSAFE to use cause it allows to modify index data
   * - Using this in StaticText to change Position color and stuff after it is
   *   rendered into commands
   * @return reference to vertex list
   */
  std::vector<Vertex>& VertexList() { return vertex_buf; }

  /**
   * Function to Push an index value to indexbuffer
   * @param v Index value
   * @return command reference
   */
  Command& PushIndex(u16 v) {
    index_buf.push_back(vertex_buf.size() + v);
    return *this;
  }

  /**
   * Setter for the Commands RenderMode
   * @param v RenderMode
   * @return command reference
   */
  Command& Rendermode(const RenderMode& v) {
    mode = v;
    return *this;
  }

  /**
   * Getter for the Commands RenderMode
   * @return RenderMode
   */
  RenderMode Rendermode() const { return mode; }

 private:
  /**
   * Vertex Buffer
   *
   * - Using default vector here as its data will be copied later
   */
  std::vector<Vertex> vertex_buf;
  /**
   * Index Buffer
   *
   * - Using default vector here as its data will be copied later
   */
  std::vector<u16> index_buf;
  /** Layer */
  int layer;
  /** Texture Reference */
  Texture::Ref tex;
  /** Index */
  int index;
  /** RenderMode (Default to RenderMode_RGBA) */
  RenderMode mode = RenderMode_RGBA;
};
}  // namespace LI
}  // namespace PD