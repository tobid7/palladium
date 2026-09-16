#include <iostream>
#include <palladium>

bool ColorTests() {
  PD::Color color = PD::Color(65, 32, 45, 240);
  std::cout << "PD Color Tests:" << std::endl;
  std::cout << "Input: Color(65, 32, 45, 240)\n";
  std::cout << std::format("  RGB: {:rgb}", color) << std::endl;
  std::cout << std::format("  RGBF: {:rgbf}", color) << std::endl;
  std::cout << std::format("  RGBA: {:rgba}", color) << std::endl;
  std::cout << std::format("  RGBAF: {:rgbaf}", color) << std::endl;
  std::cout << std::format("  Hex: {:hex}", color) << std::endl;
  std::cout << "Input: Color(0.1f, 0.3f, 0.4f, 0.9f)\n";
  color = PD::Color(0.1f, 0.3f, 0.4f, 0.9f);
  std::cout << std::format("  RGB: {:rgb}", color) << std::endl;
  std::cout << std::format("  RGBF: {:rgbf}", color) << std::endl;
  std::cout << std::format("  RGBA: {:rgba}", color) << std::endl;
  std::cout << std::format("  RGBAF: {:rgbaf}", color) << std::endl;
  std::cout << std::format("  Hex: {:hex}", color) << std::endl;
  return true;
}

bool IDTests() {
  std::cout << std::format("HashID32(\"1234\") -> {:016X}",
                           PD::HashID32("1234").Get())
            << std::endl;
  std::cout << std::format("HashID32(\"1235\") -> {:016X}",
                           PD::HashID32("1235").Get())
            << std::endl;
  std::cout << std::format("HashID64(\"1234\") -> {:016X}",
                           PD::HashID64("1234").Get())
            << std::endl;
  std::cout << std::format("HashID64(\"1235\") -> {:016X}",
                           PD::HashID64("1235").Get())
            << std::endl;
  return true;
}

int main() {
  PD::Os::UseDriver<PD::OsDriver>();
  ColorTests();
  IDTests();
  std::cout << PD::Os::GetTime() << std::endl;
  PD::Pool<int> pool;
  pool.Init(90);
  int* elems = pool.Allocate(5);
  std::cout << std::format("Pool ({}/{})", pool.size(), pool.capacity())
            << std::endl;
  return 0;
}