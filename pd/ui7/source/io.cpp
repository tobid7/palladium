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
#include <pd/ui7/io.hpp>

namespace PD {
PD_UI7_API void UI7::IO::Update() {
  /** Todo: find out if we even still use the Drawlist regestry */
  u64 current = OS::GetNanoTime();
  Delta = static_cast<float>(current - LastTime) / 1000000.f;
  LastTime = current;
  DeltaStats->Add(Delta * 1000);
  Time->Update();
  InputHandler->Update();
  Framerate = 1000.f / Delta;
  DrawListRegestry.Clear();
  DrawListRegestry.PushFront(
      Pair<UI7::ID, Li::DrawList::Ref>("CtxBackList", Back));
  // RegisterDrawList("CtxBackList", Back);
  NumIndices = FDL->pNumIndices;
  NumVertices = FDL->pNumVertices;
}
}  // namespace PD