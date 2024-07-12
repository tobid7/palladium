#pragma once
#include <functional>

namespace Palladium {
namespace Tasks {
/// @brief Push A Task
/// @param fun Function of Your Task
/// @return index
int Create(std::function<void()> fun);
/// @brief Destroy all Tasks
void DestroyAll();
}  // namespace Tasks
}  // namespace Palladium
