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

#include <pd/common/lang.hpp>
#include <pd/external/json.hpp>

namespace PD {
void Lang::LoadFile(const std::string &path) {
  nlohmann::json js;
  std::fstream iff(path, std::ios::in);
  if (iff.is_open()) {
    js = nlohmann::json::parse(iff);
    iff.close();
    if (js.is_discarded()) {
      return;
    }
    if (!js.contains("ver") || !js.contains("id") || !js.contains("name") ||
        !js.contains("author") || !js.contains("keys")) {
      return;
    }
    if (js["ver"].get<int>() != ver) {
      return;
    }
    lang_id = js["id"];
    lang_name = js["name"];
    lang_author = js["author"];
    ltable.clear();
    for (auto k : js["keys"].items()) {
      ltable.insert(std::make_pair(k.key(), k.value().get<std::string>()));
    }
  }
}

const std::string &Lang::Get(const std::string &k) {
  auto e = ltable.find(k);
  if (e != ltable.end()) {
    return e->second;
  }
  return k;
}
}  // namespace PD