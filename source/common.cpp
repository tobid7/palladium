#include <iostream>
#include <pd/common.hpp>

void PD::Log(const std::string& txt) {
  std::cout << "[PD] " << txt << std::endl;
}