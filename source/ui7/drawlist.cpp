/*
MIT License
Copyright (c) 2024 René Amthor (tobid7)

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

#include <pd/ui7/drawlist.hpp>

namespace PD {
namespace UI7 {
void DrawList::AddRectangle(vec2 pos, vec2 szs, const UI7Color& clr) {
  if (!ren->InBox(pos, szs, ren->GetViewport())) {
    return;
  }
  auto rect = ren->CreateRect(pos, szs, 0.f);
  auto cmd = LI::Command::New();
  ren->SetupCommand(cmd);
  ren->UseTex();
  cmd->Layer(layer);
  ren->QuadCommand(cmd, rect, vec4(0.f, 1.f, 1.f, 0.f), clr);
  commands.push_back(cmd);
}

void DrawList::AddTriangle(vec2 pos0, vec2 pos1, vec2 pos2,
                           const UI7Color& clr) {
  if (!ren->InBox(pos0, pos1, pos2, ren->GetViewport())) {
    return;
  }
  auto cmd = LI::Command::New();
  ren->SetupCommand(cmd);
  ren->UseTex();
  cmd->Layer(layer);
  ren->TriangleCommand(cmd, pos0, pos1, pos2, clr);
  commands.push_back(cmd);
}

void DrawList::AddText(vec2 pos, const std::string& text, const UI7Color& clr,
                       LITextFlags flags, vec2 box) {
  // Dont create a Command here as TextCommand has autosetup
  // cause it needs to generate multiple commands if
  // Font uses multiple textures
  ren->TextCommand(commands, pos, clr, text, flags, box);
}

void DrawList::AddImage(vec2 pos, Texture::Ref img) {
  if (!ren->InBox(pos, img->GetSize(), ren->GetViewport())) {
    return;
  }
  auto rect = ren->CreateRect(pos, img->GetSize(), 0.f);
  auto cmd = LI::Command::New();
  ren->SetupCommand(cmd);
  ren->UseTex(img);
  cmd->Layer(layer);
  ren->QuadCommand(cmd, rect, vec4(0.f, 1.f, 1.f, 0.f), 0xffffffff);
  commands.push_back(cmd);
}

void DrawList::Clear() { commands.clear(); }

void DrawList::Process() {
  // UI7 Commands Use LI7 as default feature
  ren->OptiCommandList(commands);
  for (auto command : commands) {
    ren->PushCommand(command);
  }
}
}  // namespace UI7
}  // namespace PD