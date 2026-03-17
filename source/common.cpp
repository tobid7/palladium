#include <iostream>
#include <pd/common.hpp>

PD_API void PD::Log(const std::string& txt) {
  std::cout << "[PD] " << txt << std::endl;
}