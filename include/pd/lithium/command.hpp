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

#include <pd/core/core.hpp>
// #include <pd/lithium/flags.hpp>
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

  Command& AppendIndex(u16 idx) {
    IndexBuffer.Add(VertexBuffer.Size() + idx);
    return *this;
  }

  Command& AppendVertex(const Vertex& v) {
    VertexBuffer.Add(v);
    return *this;
  }

  Vec<Vertex> VertexBuffer;
  Vec<u16> IndexBuffer;
  ivec4 ScissorRect;
  bool ScissorEnabled = false;
  int Layer;
  int Index;
  Texture::Ref Tex;
};
}  // namespace LI
}  // namespace PD