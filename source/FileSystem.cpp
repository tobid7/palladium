#include <3ds.h>
#include <stdarg.h>
#include <stdio.h>
#include <unistd.h>

#include <cstring>
#include <pd/FileSystem.hpp>
// Debugging
#include <algorithm>
#include <filesystem>
#include <pd/stringtool.hpp>

bool ___dir__predicate__(const Palladium::FileSystem::Entry &lhs,
                         const Palladium::FileSystem::Entry &rhs) {
  if (!lhs.dir && rhs.dir) return false;
  if (lhs.dir && !rhs.dir) return true;
  std::string a = lhs.name;
  std::string b = rhs.name;
  std::transform(a.begin(), a.end(), a.begin(),
                 [](int i) { return std::tolower(i); });
  std::transform(b.begin(), b.end(), b.begin(),
                 [](int i) { return std::tolower(i); });
  return a.compare(b) < 0;
}

std::string Palladium::FileSystem::GetParentPath(std::string path,
                                                std::string mount_point) {
  std::string tcl = path;
  if (path.substr(path.length() - 1, 1) != "/") {
    tcl += "/";
  }
  std::string res =
      std::filesystem::path(tcl).parent_path().parent_path().string();
  if (res.length() > mount_point.length()) {
    return res;
  }

  return mount_point;
}

std::vector<Palladium::FileSystem::Entry> Palladium::FileSystem::GetDirContent(
    std::string path) {
  std::vector<Palladium::FileSystem::Entry> res;
  for (const auto &entry :
       std::filesystem::directory_iterator(std::filesystem::path(path))) {
    res.push_back({entry.path().string(), GetFileName(entry.path().string()),
                   entry.is_directory()});
  }
  return res;
}

std::vector<Palladium::FileSystem::Entry>
Palladium::FileSystem::GetDirContentsExt(
    std::string &path, const std::vector<std::string> &extensions) {
  std::vector<Palladium::FileSystem::Entry> res;
  for (auto const &it :
       std::filesystem::directory_iterator(std::filesystem::path(path))) {
    Palladium::FileSystem::Entry temp;
    std::string fn = it.path().string();
    temp.name = GetFileName(fn);
    temp.path = it.path().string().c_str();
    temp.dir = it.is_directory();
    if (NameIsEndingWith(GetFileName(it.path().string()), extensions) ||
        it.is_directory()) {
      res.push_back(temp);
    }
  }
  std::sort(res.begin(), res.end(), ___dir__predicate__);
  return res;
}
