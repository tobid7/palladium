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
#include <pd/core/strings.hpp>

namespace PD {
namespace UI7 {
/**
 * ID Class (Generating an ID by String)
 */
class ID {
 public:
  /**
   * Constructor to Generate ID by input string
   * @param text Input String
   */
  ID(const std::string& text) {
    id = PD::Strings::FastHash(text);
    name = text;
  }
  /**
   * Constructor used for const char* which is automatically
   * used when directly placing a string istead of using ID("")
   * @param text Input String
   */
  ID(const char* text) {
    id = PD::Strings::FastHash(text);
    name = text;
  }
  /**
   * Use an ID as Input
   */
  ID(u32 id) { this->id = id; }
  ~ID() = default;

  /** Get The ID Initial Name */
  const std::string& GetName() const { return name; }

  /** Getter for the raw 32bit int id */
  const u32& RawID() const { return id; }

  /** Return the ID when casting to u32 */
  operator u32() const { return id; }

 private:
  u32 id;            ///< Hash of the name
  std::string name;  ///< Name
};
}  // namespace UI7
}  // namespace PD