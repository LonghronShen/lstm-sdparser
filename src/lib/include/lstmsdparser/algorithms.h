#pragma once

#ifndef B253E114_C9D6_4FE7_BC05_CBC56955533F
#define B253E114_C9D6_4FE7_BC05_CBC56955533F

#include <algorithm>
#include <iterator>
#include <random>

namespace lstmsdparser {

namespace algorithms {

template <class RandomIt> void random_shuffle(RandomIt first, RandomIt last) {
  std::random_device rng;
  std::mt19937 urng(rng());
  std::shuffle(first, last, urng);
}

} // namespace algorithms

} // namespace lstmsdparser

#endif /* B253E114_C9D6_4FE7_BC05_CBC56955533F */
