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
/**
 * Download manager class
 */
class DownloadManager : public SmartCtor<DownloadManager> {
 public:
  /** Alias to contain Error Cdoe and for some Errors a Status Code */
  using Error = u64;
  /** Error Codes of DL Manager */
  enum Error_ {
    Error_None,        ///< Function Executed Successfully
    Error_Memory,      ///< Memory  Allocation Error
    Error_Write,       ///< Unable to Write File
    Error_StatusCode,  ///< Error with Status Code
    Error_Git,         ///< Git Error
    Error_CtrStatus,   ///< 3ds Result Code
    Error_Curl,        ///< Curl Error
    Error_Busy,        ///< Another Download Taskl is already running
    Error_Invalid,     ///< Invalid Json struct
    Error_NoWifi,      ///< Console not connected to wifi
  };
  DownloadManager() = default;
  ~DownloadManager() = default;

  /**
   * Extract the DL Manager Error code of a Error
   * @param err Error
   * @return Downloadmanager Error code
   */
  static Error_ GetErrorCode(Error err) {
    return (Error_)u32(err & 0xffffffff);
  }
  /**
   * Extract the DL Manager Status code of a Error
   * @param err Error
   * @return Status code
   */
  static int GetStatusCode(Error err) {
    Error_ c = GetErrorCode(err);
    if (c != Error_StatusCode && c != Error_CtrStatus && c != Error_Curl) {
      return 0;
    }
    return u32(err >> 32);
  }

  /**
   * Download from URL inro a String Buffer
   * @param url URL to download from
   * @param res result buffer
   * @return Error Code
   */
  Error Get(const std::string& url, std::string& res);
  /**
   * Download from URL into a File
   * @param url URL to download from
   * @param res_path Path to write file to
   * @return Error Code
   */
  Error GetFile(const std::string& url, const std::string& res_path);
  /**
   * Download a File from a Git Release
   * @param url URL of the repo
   * @param asset Asset to download
   * @param path Path to write file into
   * @param pre download from Prerelease
   * @return Error Code
   */
  Error GetGitRelease(const std::string& url, const std::string& asset,
                      const std::string& path, bool pre);
  /**
   * Get a json API request as nlohmann json object
   * @param url URL to request
   * @param res result json object
   * @return Error Code
   */
  Error GetAsJson(const std::string& url, nlohmann::json& res);

  /** Get Current Progress in Bytes */
  u64 ProgressCurrent() const { return current; }
  /** Get Total number in bytes */
  u64 ProgressTotal() const { return total; }

 private:
  u64 current;  ///< Current Progress
  u64 total;    ///< Total Bytes tp Download
};
}  // namespace PD