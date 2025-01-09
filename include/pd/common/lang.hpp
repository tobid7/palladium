#pragma once

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

#include <pd/common/common.hpp>

namespace PD {
/// @brief Lang System
///< Translations are saved into json files
///< Path should point to a directory the fils
///< for example the files ar named [en.json, de.json, fr.json]
class Lang : public SmartCtor<Lang> {
 public:
  Lang() = default;
  ~Lang() = default;

  void SetBasePath(const std::string &path) { langs_path = path; }
  void Load(const std::string &lang_key) {
    LoadFile(langs_path + "/" + lang_key + ".json");
  }
  void LoadFile(const std::string &path);
  const std::string &Get(const std::string &k);
  const std::string &GetName() { return lang_name; }
  const std::string &GetID() { return lang_id; }
  const std::string &GetAuthor() { return lang_author; }
  const std::string &GetPath() { return langs_path; }

 private:
  const int ver = 0;
  std::string langs_path = "romfs:/lang";
  std::string lang_name;
  std::string lang_id;
  std::string lang_author;
  std::map<std::string, std::string> ltable;
};
}  // namespace PD