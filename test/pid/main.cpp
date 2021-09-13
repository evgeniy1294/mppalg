#include <iostream>
#include <chrono>
#include <array>
#include <unistd.h>
#include <mpp/pipeblock.hpp>
#include <mpp/pid.hpp>
#include <mpp/threshold.hpp>
#include <mpp/ring.hpp>



int main()
{
  mpp::pid::Regulator<int> Reg1(1, 0, 0);
  mpp::pid::Regulator<int> Reg2(1, 0, 0);
  mpp::ThresholdBlock<float>    Thr{-12, 12};
  auto out = 7 >> Reg1[4] >> Reg2[6] >> Thr;

  std::cout << out << std::endl;

  // 1/10000 deg
  std::int32_t pos1 = 120000;
  std::int32_t pos2 = 125000;
  std::int32_t spd1 = (pos2 - pos1) / 500;
  std::int32_t spd2 = (pos1 - pos2) / 500;

  std::cout << spd1 << std::endl;
  std::cout << spd2 << std::endl;

  return 0; 
}
