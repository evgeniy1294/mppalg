#include <iostream>
#include <chrono>
#include <array>
#include <unistd.h>
#include <mpp/pipeblock.hpp>
#include <mpp/pid.hpp>
#include <mpp/threshold.hpp>




int main()
{
  mpp::pid::Regulator<int> Reg1(1, 0, 0);
  mpp::pid::Regulator<int> Reg2(1, 0, 0);
  mpp::ThresholdBlock<float>    Thr{-12, 12};
  auto out = 7 >> Reg1[4] >> Reg2[6] >> Thr;

  std::cout << out << std::endl;

  return 0; 
}
