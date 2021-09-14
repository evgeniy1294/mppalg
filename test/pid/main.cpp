#include <iostream>
#include <chrono>
#include <array>
#include <algorithm>
#include <unistd.h>
#include <mpp/pipeblock.hpp>
#include <mpp/pid.hpp>
#include <mpp/threshold.hpp>
#include <mpp/ring.hpp>


std::array<int, 10> buffer;
mpp::ring<int> ring(buffer.data(), buffer.end());



int main()
{
  ring.push_back(0);
  ring.push_back(1);
  ring.push_back(2);
  ring.push_back(3);
  ring.push_back(4);
  ring.push_back(5);
  ring.push_back(6);
  ring.push_back(7);
  ring.push_back(8);
  ring.push_back(9);
  ring.push_back(9);
  ring.push_back(9);

  for (auto i: ring)
    std::cout << i << " ";
  std::cout << std::endl;

  auto is_actual = [](int i) { return i > 4; };
  auto res = std::find_if(ring.begin(), ring.end(), is_actual);
  std::cout << *res << std::endl;

  return 0; 
}
