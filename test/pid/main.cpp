#include <iostream>
#include <chrono>
#include <array>
#include <unistd.h>
#include <mpp/pipeblock.hpp>
#include <mpp/pid.hpp>

static std::array<std::uint8_t, 10> array;


int main()
{
  mpp::pid::Regulator<int> Reg1(1,0,0);
  auto out = Reg1[5] << 2;

  std::cout << out << std::endl;

  return 0; 
}
