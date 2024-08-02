#include <pd/LI7.hpp>
#include <pd/Render2.hpp>
#include <pd/internal_db.hpp>

namespace Palladium {
const float R2::default_text_size = 0.5f;
float R2::text_size = 0.5;
Font::Ref R2::font;
std::map<std::string, float> R2::ts;
std::map<std::string, int> R2::mln;
bool R2::next_lined = false;
std::vector<R2::R2Cmd::Ref> R2::commands;
R2Screen R2::current_screen = R2Screen_Bottom;

void R2::Init() { R2::font = Font::New(); }

void R2::SetFont(Font::Ref fnt) {
  if (!fnt) return;
  R2::font = fnt;
}

Font::Ref R2::GetFont() { return R2::font; }

void R2::DefaultFont() { R2::font->Unload(); }

void R2::DrawNextLined() { R2::next_lined = true; }

void R2::OnScreen(R2Screen screen) {
  if (screen < 0 || screen > R2Screen_Top) return;
  R2::current_screen = screen;
}

void R2::SetTextSize(float szs) { text_size = szs; }

void R2::DefaultTextSize() { text_size = R2::default_text_size; }

float R2::GetTextSize() { return text_size; }

R2Screen R2::GetCurrentScreen() { return current_screen; }

NVec2 R2::GetTextDimensions(const std::string& text) {
  return LI7::GetTextDimensions(text);
}

std::string R2::WrapText(const std::string& in, int maxlen) {
  std::string out;
  std::string line;
  int line_x = 0;
  std::istringstream istream(in);
  std::string temp;

  while (istream >> temp) {
    NVec2 dim = R2::GetTextDimensions(line + temp);
    if (line_x + dim.x <= maxlen) {
      line += temp + ' ';
      line_x += dim.x;
    } else {
      out += line + '\n';
      line = temp + ' ';
      line_x = R2::GetTextDimensions(line).x;
    }
  }
  out += line;
  return out;
}

std::string R2::ShortText(const std::string& in, int maxlen) {
  auto textdim = R2::GetTextDimensions(in);
  if (textdim.x < (float)maxlen) return in;
  std::string ext = "";
  std::string ph = "(...)"; // placeholder
  std::string worker = in;
  std::string out;
  size_t ext_pos = in.find_last_of('.');
  if (ext_pos != in.npos) {
    ext = in.substr(ext_pos);
    worker = in.substr(0, ext_pos);
  }

  maxlen -= R2::GetTextDimensions(ext).x;
  maxlen -= R2::GetTextDimensions(ph).x;

  for (auto& it : worker) {
    if (R2::GetTextDimensions(out).x > (float)maxlen) {
      out += ph;
      out += ext;
      return out;
    }
    out += it;
  }
  return out;  // Impossible to reach
}

NVec2 R2::GetCurrentScreenSize() {
  return NVec2(R2::current_screen == R2Screen_Bottom ? 320 : 400, 240);
}

// Main Processing of Draw Calls
void R2::Process() {
  Palladium::Ftrace::ScopedTrace st("Render2", "ProcessList");
  for (auto& it : R2::commands) {
    if (it->type <= 0 || it->type > 6) {
      // Skip
      continue;
    }
    LI7::OnScreen(!it->Screen);
    if (it->type == 1) {
      LI7::UseTexture();
      // Rect
      if (it->lined) {
        LI7::Line(it->pos, NVec2(it->pos.x + it->pszs.x, it->pos.y), it->clr,
                  1.f);
        LI7::Line(it->pos, NVec2(it->pos.x, it->pos.y + it->pszs.y), it->clr,
                  1.f);
        LI7::Line(NVec2(it->pos.x + it->pszs.x, it->pos.y),
                  NVec2(it->pos.x + it->pszs.x, it->pos.y + it->pszs.y),
                  it->clr, 1.f);
        LI7::Line(NVec2(it->pos.x, it->pos.y + it->pszs.y),
                  NVec2(it->pos.x + it->pszs.x, it->pos.y + it->pszs.y),
                  it->clr, 1.f);
      } else {
        LI7::ColorRect(it->pos, it->pszs, it->clr);
      }
    } else if (it->type == 2) {
      LI7::UseTexture();
      // Triangle
      if (it->lined) {
        LI7::Line(it->pos, it->pszs, it->clr, 1);
        LI7::Line(it->pos, it->ap, it->clr, 1);
        LI7::Line(it->pszs, it->ap, it->clr, 1);
      } else {
        LI7::Triangle(it->pos, it->pszs, it->ap, it->clr);
      }
    } else if (it->type == 3) {
      std::string txt = it->text;
      if (it->flags & PDTextFlags_Short) {
        txt = ShortText(txt, it->pszs.x - it->pos.x);
      } else if(it->flags & PDTextFlags_Wrap) {
        txt = WrapText(it->text, it->pszs.x-it->pos.x);
      }
      LI7::DrawText(it->pos, it->clr, txt, it->flags, it->pszs);
    } else if (it->type == 4) {
      if (it->img->Loadet()) {
        LI7::UseTexture(it->img->Get());
        LI7::Rect(it->pos, it->img->Get()->GetSize(), it->img->GetUV());
      }
    } else if (it->type == 5) {
      // TODO: Move the Draw Func into this API
      // it->spr->Draw();
    } else if (it->type == 6) {
      LI7::UseTexture();
      LI7::Line(it->pos, it->pszs, it->clr, it->ap.x);
    }
  }
  R2::commands.clear();
}

void R2::AddRect(NVec2 pos, NVec2 size, PDColor clr) {
  auto cmd = R2Cmd::New();
  cmd->pos = pos;
  cmd->pszs = size;
  cmd->clr = Palladium::ThemeActive()->Get(clr);
  cmd->type = 1;  // Rect
  // Just assign current screen as bottom is 0 (false)
  // and Top and TopRight are !0 (true)
  cmd->Screen = current_screen;
  if (R2::next_lined) {
    cmd->lined = true;
    R2::next_lined = false;
  }
  R2::commands.push_back(cmd);
}

void R2::AddRect(NVec2 pos, NVec2 size, unsigned int clr) {
  auto cmd = R2Cmd::New();
  cmd->pos = pos;
  cmd->pszs = size;
  cmd->clr = clr;
  cmd->type = 1;  // Rect
  // Just assign current screen as bottom is 0 (false)
  // and Top and TopRight are !0 (true)
  cmd->Screen = current_screen;
  if (R2::next_lined) {
    cmd->lined = true;
    R2::next_lined = false;
  }
  R2::commands.push_back(cmd);
}

void R2::AddLine(NVec2 pos_a, NVec2 pos_b, PDColor clr, int t) {
  auto cmd = R2Cmd::New();
  cmd->pos = pos_a;
  cmd->pszs = pos_b;
  cmd->ap.x = t;
  cmd->clr = Palladium::ThemeActive()->Get(clr);
  cmd->type = 6;  // Line
  // Just assign current screen as bottom is 0 (false)
  // and Top and TopRight are !0 (true)
  cmd->Screen = current_screen;
  if (R2::next_lined) {
    cmd->lined = true;
    R2::next_lined = false;
  }
  R2::commands.push_back(cmd);
}

void R2::AddLine(NVec2 pos_a, NVec2 pos_b, unsigned int clr, int t) {
  auto cmd = R2Cmd::New();
  cmd->pos = pos_a;
  cmd->pszs = pos_b;
  cmd->ap.x = t;
  cmd->clr = clr;
  cmd->type = 6;  // Line
  // Just assign current screen as bottom is 0 (false)
  // and Top and TopRight are !0 (true)
  cmd->Screen = current_screen;
  if (R2::next_lined) {
    cmd->lined = true;
    R2::next_lined = false;
  }
  R2::commands.push_back(cmd);
}

void R2::AddTriangle(NVec2 pos0, NVec2 pos1, NVec2 pos2, PDColor clr) {
  auto cmd = R2Cmd::New();
  cmd->pos = pos0;
  cmd->pszs = pos1;
  cmd->ap = pos2;
  cmd->clr = Palladium::ThemeActive()->Get(clr);
  cmd->type = 2;  // Triangle
  // Just assign current screen as bottom is 0 (false)
  // and Top and TopRight are !0 (true)
  cmd->Screen = current_screen;
  if (R2::next_lined) {
    cmd->lined = true;
    R2::next_lined = false;
  }
  R2::commands.push_back(cmd);
}

void R2::AddTriangle(NVec2 pos0, NVec2 pos1, NVec2 pos2, unsigned int clr) {
  auto cmd = R2Cmd::New();
  cmd->pos = pos0;
  cmd->pszs = pos1;
  cmd->ap = pos2;
  cmd->clr = clr;
  cmd->type = 2;  // Triangle
  // Just assign current screen as bottom is 0 (false)
  // and Top and TopRight are !0 (true)
  cmd->Screen = current_screen;
  if (R2::next_lined) {
    cmd->lined = true;
    R2::next_lined = false;
  }
  R2::commands.push_back(cmd);
}

void R2::AddText(NVec2 pos, const std::string& text, PDColor clr,
                 PDTextFlags flags, NVec2 tmb) {
  auto cmd = R2Cmd::New();
  cmd->pos = pos;
  cmd->pszs = tmb;
  cmd->clr = Palladium::ThemeActive()->Get(clr);
  cmd->flags = flags;
  cmd->text = text;
  cmd->type = 3;  // Text
  // Just assign current screen as bottom is 0 (false)
  // and Top and TopRight are !0 (true)
  cmd->Screen = current_screen;
  if (R2::next_lined) {
    cmd->lined = true;
    R2::next_lined = false;
  }
  R2::commands.push_back(cmd);
}

void R2::AddText(NVec2 pos, const std::string& text, unsigned int clr,
                 PDTextFlags flags, NVec2 tmb) {
  auto cmd = R2Cmd::New();
  cmd->pos = pos;
  cmd->pszs = tmb;
  cmd->clr = clr;
  cmd->flags = flags;
  cmd->text = text;
  cmd->type = 3;  // Text
  // Just assign current screen as bottom is 0 (false)
  // and Top and TopRight are !0 (true)
  cmd->Screen = current_screen;
  if (R2::next_lined) {
    cmd->lined = true;
    R2::next_lined = false;
  }
  R2::commands.push_back(cmd);
}

void R2::AddImage(NVec2 pos, Image::Ref img) {
  auto cmd = R2Cmd::New();
  cmd->pos = pos;
  cmd->img = img;
  cmd->type = 4;  // Image
  // Just assign current screen as bottom is 0 (false)
  // and Top and TopRight are !0 (true)
  cmd->Screen = current_screen;
  if (R2::next_lined) {
    cmd->lined = true;
    R2::next_lined = false;
  }
  R2::commands.push_back(cmd);
}

void R2::AddSprite(Sprite::Ref spr) {
  auto cmd = R2Cmd::New();
  cmd->spr = spr;
  cmd->type = 5;  // Sprite
  // Just assign current screen as bottom is 0 (false)
  // and Top and TopRight are !0 (true)
  cmd->Screen = current_screen;
  if (R2::next_lined) {
    cmd->lined = true;
    R2::next_lined = false;
  }
  R2::commands.push_back(cmd);
}

}  // namespace Palladium