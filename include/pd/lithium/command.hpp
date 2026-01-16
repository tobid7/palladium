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
  int Layer;
  int Index;
  Texture::Ref Tex;
};

class CmdPool {
 public:
  CmdPool() {}
  ~CmdPool() {}

  Command::Ref NewCmd() {
    if (pPoolIdx >= pPool.size()) {
      Resize(pPool.size() + 128);
    }
    Command::Ref nu = pPool[pPoolIdx++];
    nu->Layer = Layer;
    nu->Index = pPoolIdx - 1;
    return nu;
  }

  void Init(size_t initial_size) { Resize(initial_size); }

  void Deinit() {
    for (auto it : pPool) {
      Command::Delete(it);
    }
    pPool.clear();
  }

  void Resize(size_t nulen) {
    if (nulen <= pPool.size()) {
      return;  // no idea yet
    }
    size_t oldlen = pPool.size();
    pPool.resize(nulen);
    for (size_t i = oldlen; i < pPool.size(); i++) {
      pPool[i] = Command::New();
    }
  }

  void Reset() {
    for (u32 i = 0; i < pPoolIdx; i++) {
      pPool[i]->Clear();
    }
    pPoolIdx = 0;
  }

  Command::Ref GetCmd(size_t idx) const { return pPool[idx]; }
  Command::Ref GetCmd(size_t idx) { return pPool[idx]; }

  size_t Size() const { return pPoolIdx; }
  size_t Cap() const { return pPool.size(); }

  void Merge(CmdPool& p) {
    if (pPoolIdx + p.Size() > pPool.size()) {
      Resize(pPoolIdx + p.Size());
    }
    for (size_t i = 0; i < p.Size(); i++) {
      size_t idx = pPoolIdx++;
      *pPool[idx] = *p.GetCmd(i);
      pPool[idx]->Index = idx;
    }
    p.Reset();
  }

 private:
  std::vector<Command::Ref> pPool;
  u32 pPoolIdx = 0;
  int Layer = 0;
};
}  // namespace Li
}  // namespace PD