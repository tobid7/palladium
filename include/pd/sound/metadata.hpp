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
namespace Music {
/**
 * Music Metadata Data Holder
 */
class MetaData {
 public:
  MetaData() = default;
  ~MetaData() = default;

  /** Getter for name */
  std::string Name() const { return name; }
  /** Getter for album */
  std::string Album() const { return album; }
  /** Getter for year */
  std::string Year() const { return year; }
  /** Getter for Title */
  std::string Title() const { return title; }
  /** Getter for Artist */
  std::string Artist() const { return artist; }
  /** Getter for [what is this] */
  std::string Mdt() const { return mdt; }
  /** Gettr for file path */
  std::string Path() const { return path; }

  /** Setter for Name */
  void Name(const std::string &v) { name = v; }
  /** Setter for Album */
  void Album(const std::string &v) { album = v; }
  /** Settr for Year */
  void Year(const std::string &v) { year = v; }
  /** Settr for Title */
  void Title(const std::string &v) { title = v; }
  /** Settr for Artist */
  void Artist(const std::string &v) { artist = v; }
  /** Settr for [what is this] */
  void Mdt(const std::string &v) { mdt = v; }
  /** Settr for Path */
  void Path(const std::string &v) { path = v; }

 private:
  const std::string unk = "Unknown";
  std::string title = unk;
  std::string album = unk;
  std::string year = unk;
  std::string name = unk;
  std::string path = unk;
  std::string artist = unk;
  std::string mdt = unk;
};
}  // namespace Music
}  // namespace PD