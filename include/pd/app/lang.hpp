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

#include <pd/core/common.hpp>

namespace PD {
/**
 * Language System
 *
 * - Translations are saved into json files
 * - path should point to a directory containing the json files
 * - example for filenames: `en.json`, `de.json`, `fr.json`
 */
class Lang : public SmartCtor<Lang> {
 public:
  Lang() = default;
  ~Lang() = default;

  /**
   * Function to set the path to search for Language files
   * @param path Path to search the files
   */
  void SetBasePath(const std::string &path) { langs_path = path; }
  /**
   * Load a language file by the language key
   * @param lang_key Language key for example `de`
   */
  void Load(const std::string &lang_key) {
    LoadFile(langs_path + "/" + lang_key + ".json");
  }
  /**
   * Directly load a Language file from a specific path
   * @param path Path to load the file from
   */
  void LoadFile(const std::string &path);
  /**
   * Get a String by a `Keyword`
   * @param k Keyword to search for
   * @return Returns the string or if none found it returns the Keyword
   */
  const std::string &Get(const std::string &k);
  /**
   * Get the Language Name
   * @return Returns the Language Name
   */
  const std::string &GetName() { return lang_name; }
  /**
   * Get the Language ID / Key
   * @return Returns the Language ID
   */
  const std::string &GetID() { return lang_id; }
  /**
   * Get the Language Author(s)
   * @return Returns the Author(s) of the Language file
   */
  const std::string &GetAuthor() { return lang_author; }
  /**
   * Get the Language File Search Path
   * @return Returns Path where the Files are searched for
   */
  const std::string &GetPath() { return langs_path; }

 private:
  const int ver = 0;
  /** Language Files Root path */
  std::string langs_path = "romfs:/lang";
  /** Language Name */
  std::string lang_name;
  /** Language ID / Key */
  std::string lang_id;
  /** Language Author */
  std::string lang_author;
  /** KEY - STRING Table for faster Key access */
  std::map<std::string, std::string> ltable;
};
}  // namespace PD