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

#include <pd/core/strings.hpp>
#include <pd/ui7/drawlist.hpp>

namespace PD {
namespace UI7 {
void DrawList::AddRectangle(vec2 pos, vec2 szs, const UI7Color& clr) {
  if (!ren->InBox(pos, szs, ren->GetViewport())) {
    return;
  }
  auto rect = ren->CreateRect(pos, szs, 0.f);
  auto cmd = LI::Command::New();
  ren->UseTex();
  ren->SetupCommand(cmd);
  cmd->Layer(base + layer);
  ren->QuadCommand(cmd, rect, vec4(0.f, 1.f, 1.f, 0.f), clr);
  commands.push_back(std::make_pair(
      ren->CurrentScreen()->ScreenType() == Screen::Bottom, cmd));
}

void DrawList::AddTriangle(vec2 pos0, vec2 pos1, vec2 pos2,
                           const UI7Color& clr) {
  if (!ren->InBox(pos0, pos1, pos2, ren->GetViewport())) {
    return;
  }
  auto cmd = LI::Command::New();
  ren->UseTex();
  ren->SetupCommand(cmd);
  cmd->Layer(base + layer);
  ren->TriangleCommand(cmd, pos0, pos1, pos2, clr);
  commands.push_back(std::make_pair(
      ren->CurrentScreen()->ScreenType() == Screen::Bottom, cmd));
}

void DrawList::AddText(vec2 pos, const std::string& text, const UI7Color& clr,
                       LITextFlags flags, vec2 box) {
  if (!ren->Font()) {
    return;
  }
  u32 id = Strings::FastHash(text);
  auto e = static_text.find(id);
  if (e == static_text.end()) {
    static_text[id] = LI::StaticText::New();
    e = static_text.find(id);
  }
  if (!e->second->IsSetup() || e->second->Font() != ren->Font()) {
    int l = ren->Layer();
    ren->Layer(base);
    /// Probably a simple ren.get() would handle the job too
    e->second->Setup(&(*ren), pos, clr, text, flags, box);
    e->second->Font(ren->Font());
    ren->Layer(l);
  }
  e->second->SetPos(pos);
  e->second->SetColor(clr);
  e->second->SetLayer(layer);
  e->second->Draw();

  ////// STILL LEAVING THE OLD CODE BELOW AS IT IS MAYBE NEEDED //////
  //////   IF STATIC TEXT SYSTEM SHOULD HAVE AN DISABLE OPTION  //////

  // Dont create a Command here as TextCommand has autosetup
  // cause it needs to generate multiple commands if
  // Font uses multiple textures
  // Oh and Handle Layer management here as well
  //  int l = ren->Layer();
  //  ren->Layer(base + layer);
  //  std::vector<LI::Command::Ref> cmds;
  //  ren->TextCommand(cmds, pos, clr, text, flags, box);
  //  ren->Layer(l);
  //  for (auto c : cmds) {
  //    commands.push_back(
  //        std::make_pair(ren->CurrentScreen()->ScreenType() == Screen::Bottom,
  //        c));
  //  }
}

void DrawList::AddImage(vec2 pos, Texture::Ref img, vec2 size) {
  size = size == 0.f ? img->GetSize() : size;
  if (!ren->InBox(pos, size, ren->GetViewport())) {
    return;
  }
  auto rect = ren->CreateRect(pos, size, 0.f);
  auto cmd = LI::Command::New();
  ren->UseTex(img);
  ren->SetupCommand(cmd);
  cmd->Layer(base + layer);
  ren->QuadCommand(cmd, rect, img->GetUV(), 0xffffffff);
  commands.push_back(std::make_pair(
      ren->CurrentScreen()->ScreenType() == Screen::Bottom, cmd));
}

void DrawList::Clear() { commands.clear(); }

void DrawList::Process() {
  for (auto command : commands) {
    ren->OnScreen(ren->GetScreen(command.first));
    ren->PushCommand(command.second);
  }
  commands.clear();
  layer = 0;
  std::vector<u32> rem;
  for (auto it : static_text) {
    if (!it.second->Used()) {
      rem.push_back(it.first);
    }
    it.second->SetUnused();
  }
  for (auto& it : rem) {
    static_text.erase(it);
  }
}
}  // namespace UI7
}  // namespace PD