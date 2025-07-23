#pragma once

/** Header for all source file functions */

#include <format>  // yes this tool requires at least c++ 20
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

namespace LVec {
const std::vector<std::string> elems = {"x", "y", "z", "w"};
std::string GenerateHeader(int n);
std::string MakeOperationFor(char op, int n);
std::string GenericOperations(int n);
std::string MakeFunctions(int n);
std::string MakeSwap(int n);
std::string MakeConstructors(int n);
}  // namespace LVec