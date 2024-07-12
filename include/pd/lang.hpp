#pragma once
// clang-format off
#include <string>
#include <pd/external/json.hpp>
// clang-format on

namespace Palladium {
namespace Lang {
/// @brief Get 3ds System lang! [en] by default
/// @return Sytemlang as string
std::string GetSys();
/// @brief Get The Translation String
/// @param key Key of Translation
/// @return The Translated String
std::string Get(const std::string &key);
/// @brief Load A Language json
/// @param lang The Language Key [en], [de], etc, or getSys()
void Load(const std::string &lang);
// New funcs
std::string GetName();
std::string GetAuthor();
std::string GetShortcut();
}  // namespace Lang
}  // namespace Palladium
