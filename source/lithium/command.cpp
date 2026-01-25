/*
MIT License
Copyright (c) 2024 - 2026 René Amthor (tobid7)

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

#include <pd/lithium/command.hpp>

PD_API PD::Li::Command::Ref PD::Li::CmdPool::NewCmd() {
  if (pPoolIdx >= pPool.size()) {
    Resize(pPool.size() + 128);
  }
  Command::Ref nu = pPool[pPoolIdx++];
  nu->Layer = Layer;
  nu->Index = pPoolIdx - 1;
  return nu;
}

PD_API void PD::Li::CmdPool::Init(size_t initial_size) { Resize(initial_size); }

PD_API void PD::Li::CmdPool::Deinit() {
  for (auto it : pPool) {
    Command::Delete(it);
  }
  pPool.clear();
}

PD_API void PD::Li::CmdPool::Resize(size_t nulen) {
  if (nulen <= pPool.size()) {
    return;  // no idea yet
  }
  size_t oldlen = pPool.size();
  pPool.resize(nulen);
  for (size_t i = oldlen; i < pPool.size(); i++) {
    pPool[i] = Command::New();
  }
}

PD_API void PD::Li::CmdPool::Reset() {
  for (u32 i = 0; i < pPoolIdx; i++) {
    pPool[i]->Clear();
  }
  pPoolIdx = 0;
}

PD::Li::Command::Ref PD::Li::CmdPool::GetCmd(size_t idx) const {
  return pPool[idx];
}
PD::Li::Command::Ref PD::Li::CmdPool::GetCmd(size_t idx) { return pPool[idx]; }

size_t PD::Li::CmdPool::Size() const { return pPoolIdx; }
size_t PD::Li::CmdPool::Cap() const { return pPool.size(); }

PD_API void PD::Li::CmdPool::Merge(CmdPool& p) {
  Copy(p);
  p.Reset();
}

PD_API void PD::Li::CmdPool::Copy(CmdPool& p) {
  if (pPoolIdx + p.Size() > pPool.size()) {
    Resize(pPoolIdx + p.Size());
  }
  for (size_t i = 0; i < p.Size(); i++) {
    size_t idx = pPoolIdx++;
    *pPool[idx] = *p.GetCmd(i);
    pPool[idx]->Index = idx;
    pPool[idx]->Layer += Layer;
  }
}

PD_API void PD::Li::CmdPool::Sort() {
  if (pPoolIdx < 2) return;
  std::sort(begin(), end(), pTheOrder);
}

PD_API bool PD::Li::CmdPool::pTheOrder(const Command::Ref& a,
                                       const Command::Ref& b) {
  if (a->Layer == b->Layer) {
    if (a->Tex == b->Tex) {
      return a->Index < b->Index;
    }
    return a->Tex < b->Tex;
  }
  return a->Layer < b->Layer;
}