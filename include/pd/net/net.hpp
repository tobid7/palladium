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
#include <pd/external/json.hpp>

namespace PD {
class DownloadManager : public SmartCtor<DownloadManager> {
 public:
  using Error = u64;
  enum Error_ {
    Error_None,        // Function Executed Successfully
    Error_Memory,      // Memory  Allocation Error
    Error_Write,       // Unable to Write File
    Error_StatusCode,  // Error with Status Code
    Error_Git,         // Git Error
    Error_CtrStatus,   // 3ds Result Code
    Error_Curl,        // Curl Error
    Error_Busy,        // Another Download Taskl is already running
    Error_Invalid,     // Invalid Json struct
    Error_NoWifi,      // Console not connected to wifi
  };
  DownloadManager() {}
  ~DownloadManager() {}

  static Error_ GetErrorCode(Error err) {
    return (Error_)u32(err & 0xffffffff);
  }
  static int GetStatusCode(Error err) {
    Error_ c = GetErrorCode(err);
    if (c != Error_StatusCode && c != Error_CtrStatus && c != Error_Curl) {
      return 0;
    }
    return u32(err >> 32);
  }

  Error Get(const std::string& url, std::string& res);
  Error GetFile(const std::string& url, const std::string& res_path);
  Error GetGitRelease(const std::string& url, const std::string& asset,
                      const std::string& path, bool pre);
  Error GetAsJson(const std::string& url, nlohmann::json& res);

  u64 ProgressCurrent() const { return current; }
  u64 ProgressTotal() const { return total; }

 private:
  u64 current;
  u64 total;
};
}  // namespace PD