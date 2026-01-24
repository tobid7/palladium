#pragma once

/*
MIT License
Copyright (c) 2024 - 2025 René Amthor (tobid7)

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
#include <pd/lithium/pd_p_api.hpp>
#include <pd/lithium/texture.hpp>
#include <pd/lithium/vertex.hpp>

namespace PD {
namespace Li {
class Command {
 public:
  Command() = default;
  ~Command() = default;

  PD_RAW(Command);

  Command& AddIdx(const u16& idx) {
    IndexBuffer.push_back(VertexBuffer.size() + idx);
    return *this;
  }
  Command& AddIdxs(const u16& a, const u16& b, const u16& c) {
    IndexBuffer.push_back(VertexBuffer.size() + a);
    IndexBuffer.push_back(VertexBuffer.size() + b);
    IndexBuffer.push_back(VertexBuffer.size() + c);
    return *this;
  }

  Command& AddVtx(const Vertex& v) {
    VertexBuffer.push_back(std::move(v));
    return *this;
  }

  void Clear() {
    VertexBuffer.clear();
    IndexBuffer.clear();
    Index = 0;
    Layer = 0;
    Tex = 0;
    ScissorOn = false;
    ScissorRect = ivec4();
  }

  std::vector<Vertex> VertexBuffer;
  std::vector<u16> IndexBuffer;
  ivec4 ScissorRect;
  bool ScissorOn = false;
  int Layer = 0;
  int Index = 0;
  TexAddress Tex;
};

class PD_LITHIUM_API CmdPool {
 public:
  CmdPool() {}
  ~CmdPool() {}

  Command::Ref NewCmd();
  void Init(size_t initial_size);
  void Deinit();
  void Resize(size_t nulen);
  void Reset();
  Command::Ref GetCmd(size_t idx) const;
  Command::Ref GetCmd(size_t idx);
  size_t Size() const;
  size_t Cap() const;
  void Merge(CmdPool& p);
  void Copy(CmdPool& p);
  void Sort();

 private:
  static bool pTheOrder(const Command::Ref& a, const Command::Ref& b);
  friend class DrawList;
  Command::Ref* begin() { return &pPool[0]; }
  Command::Ref* end() { return &pPool[pPoolIdx - 1]; }
  std::vector<Command::Ref> pPool;
  u32 pPoolIdx = 0;
  int Layer = 0;
};
}  // namespace Li
}  // namespace PD