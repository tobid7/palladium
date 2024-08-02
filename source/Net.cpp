// TODO: Make Download2File faster on large files

#include <3ds.h>
#include <curl/curl.h>
#include <malloc.h>
#include <unistd.h>

#include <filesystem>
#include <fstream>
#include <pd/Net.hpp>
#include <pd/internal_db.hpp>
#include <regex>

static Palladium::Net::Error pdi_check_wifi() {
  // if (pdi_is_citra) return 0;
  int s = osGetWifiStrength();
  return (s == 0 ? Palladium::Net::Error_NoWifi : 0);
}

static size_t pdi_handle_data(char* ptr, size_t size, size_t nmemb,
                              void* userdata) {
  size_t ms = size * nmemb;
  ((std::string*)userdata)->append(ptr, ms);
  return ms;
}

static size_t pdi_handle_file(char* ptr, size_t size, size_t nmemb, void* out) {
  size_t ms = size * nmemb;
  ((std::ofstream*)out)->write(reinterpret_cast<const char*>(ptr), ms);
  return ms;
}

struct pdi_net_dl {
  unsigned long long current = 0;
  unsigned long long total = 1;
  void Reset() {
    current = 0;
    total = 1;
  }
};

static pdi_net_dl pdi_net_dl_spec;

static int pdi_handle_curl_progress(CURL* hnd, curl_off_t dltotal,
                                    curl_off_t dlnow, curl_off_t ultotal,
                                    curl_off_t ulnow) {
  pdi_net_dl_spec.total = dltotal;
  pdi_net_dl_spec.current = dlnow;
  return 0;
}

static void pdi_setup_curl_context(CURL* hnd, const std::string& url,
                                   void* userptr, bool mem) {
  std::string user_agent =
      pdi_app_name + "/Palladium (Version: " + std::string(PDVSTRING) + ")";

  if (!mem) {
    curl_easy_setopt(hnd, CURLOPT_FAILONERROR, 1L);
    curl_easy_setopt(hnd, CURLOPT_ACCEPT_ENCODING, "gzip");
    curl_easy_setopt(hnd, CURLOPT_XFERINFOFUNCTION, pdi_handle_curl_progress);
  }

  curl_easy_setopt(hnd, CURLOPT_URL, url.c_str());
  curl_easy_setopt(hnd, CURLOPT_NOPROGRESS, 0L);
  curl_easy_setopt(hnd, CURLOPT_USERAGENT, user_agent.c_str());
  curl_easy_setopt(hnd, CURLOPT_WRITEDATA, userptr);
  curl_easy_setopt(hnd, CURLOPT_FOLLOWLOCATION, 1L);
  curl_easy_setopt(hnd, CURLOPT_MAXREDIRS, 50L);
  curl_easy_setopt(hnd, CURLOPT_HTTP_VERSION, (long)CURL_HTTP_VERSION_2TLS);
  curl_easy_setopt(hnd, CURLOPT_WRITEFUNCTION,
                   mem ? pdi_handle_data : pdi_handle_file);
  curl_easy_setopt(hnd, CURLOPT_SSL_VERIFYPEER, 0L);
  curl_easy_setopt(hnd, CURLOPT_VERBOSE, 1L);
  curl_easy_setopt(hnd, CURLOPT_STDERR, stdout);
}

static bool pdi_curl_is_busy = false;
static bool pdi_apir_is_busy = false;

namespace Palladium {
namespace Net {
Error Download(const std::string& url, std::string& data) {
  if (pdi_curl_is_busy) return Error_Busy;
  Error ret = pdi_check_wifi();
  if (ret != 0) {
    return ret;
  }
  pdi_curl_is_busy = true;
  pdi_net_dl_spec.Reset();

  auto hnd = curl_easy_init();
  pdi_setup_curl_context(hnd, url, &data, true);

  CURLcode curl_res = curl_easy_perform(hnd);
  curl_easy_cleanup(hnd);

  if (curl_res != CURLE_OK) {
    data.clear();
    pdi_curl_is_busy = false;
    return ((static_cast<Error>(curl_res) << 32) |
            static_cast<Error>(Error_Curl));
  }

  pdi_curl_is_busy = false;
  return 0;
}

Error Download2File(const std::string& url, const std::string& path) {
  if (pdi_curl_is_busy) return Error_Busy;
  Error ret = pdi_check_wifi();
  if (ret != 0) {
    return ret;
  }
  pdi_curl_is_busy = true;
  pdi_net_dl_spec.Reset();
  // std::filesystem::create_directories(
  // std::filesystem::path(path).remove_filename());
  std::ofstream file(path, std::ios::binary);

  if (!file.is_open()) {
    pdi_curl_is_busy = false;
    return Error_Write;
  }

  auto hnd = curl_easy_init();
  pdi_setup_curl_context(hnd, url, &file, false);

  CURLcode curl_res = curl_easy_perform(hnd);
  curl_easy_cleanup(hnd);

  file.close();
  if (curl_res != CURLE_OK) {
    if (Palladium::FS::FileExist(path)) {
      std::filesystem::remove(path);
    }
    pdi_curl_is_busy = false;
    return ((static_cast<Error>(curl_res) << 32) |
            static_cast<Error>(Error_Curl));
  }

  pdi_curl_is_busy = false;
  return 0;
}

Error GitDownloadRelease(const std::string& url, const std::string& asset_name,
                         const std::string& path, bool prerelease) {
  if (pdi_apir_is_busy) return Error_Busy;
  Error ret = pdi_check_wifi();
  if (ret != 0) {
    return ret;
  }
  pdi_apir_is_busy = true;
  std::regex parse("github\\.com\\/(.+)\\/(.+)");
  std::smatch res;
  std::regex_search(url, res, parse);

  std::string user = res[1].str();
  std::string repo = res[2].str();

  std::stringstream req;
  req << "https://api.github.com/repos/" << user << "/" << repo
      << (prerelease ? "/releases" : "/releases/latest");

  std::string buf;
  ret = Download(req.str(), buf);
  if (ret) {
    pdi_apir_is_busy = false;
    return ret;
  }

  int pret = 0;
  std::string freq;
  if (nlohmann::json::accept(buf)) {
    nlohmann::json api = nlohmann::json::parse(buf);
    if (!api.size()) pret = -1;
    if (pret != -1) {
      if (prerelease) api = api[0];
      if (api["assets"].is_array()) {
        for (const auto& asset : api["assets"]) {
          if (asset.is_object() && asset["name"].is_string() &&
              asset["browser_download_url"].is_string()) {
            if (std::regex_match(std::string(asset["name"]),
                                 std::regex(asset_name))) {
              freq = asset["browser_download_url"];
              break;
            }
          }
        }
      }
    }
  } else {
    pret = -1;
  }

  if (pret != 0 || freq.empty()) {
    pdi_apir_is_busy = false;
    return Error_Git;
  }

  ret = Download2File(freq, path);
  pdi_apir_is_busy = false;
  return ret;
}

Error JsonApiRequest(const std::string& api_url, nlohmann::json& res) {
  if (pdi_apir_is_busy) return Error_Busy;
  Error ret = pdi_check_wifi();
  if (ret != 0) {
    return ret;
  }
  pdi_apir_is_busy = true;

  std::string buf;
  ret = Download(api_url, buf);
  if (ret) {
    pdi_apir_is_busy = false;
    return ret;
  }

  if (nlohmann::json::accept(buf)) {
    res = nlohmann::json::parse(buf);
    if (!res.size()) {
      pdi_apir_is_busy = false;
      return Error_Invalid;
    }
  } else {
    pdi_apir_is_busy = false;
    return Error_Invalid;
  }

  pdi_apir_is_busy = false;
  return 0;
}

unsigned long long GetProgressCurrent() { return pdi_net_dl_spec.current; }
unsigned long long GetProgressTotal() {
  // As curl sets total to 0 we need
  // to return a 1 as devide by zeroi will crash
  if (pdi_net_dl_spec.total <= 0) return 1;
  return pdi_net_dl_spec.total;
}
}  // namespace Net
}  // namespace Palladium