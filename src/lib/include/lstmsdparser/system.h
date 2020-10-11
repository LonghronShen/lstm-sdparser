#ifndef __LTP_LSTMSDPARSER_SYSTEM_H__
#define __LTP_LSTMSDPARSER_SYSTEM_H__

#include <algorithm> // for find
#include <iostream>
#include <map>
#include <random>
#include <string>
#include <vector>

//! The tree type.
typedef std::vector<std::vector<int>> tree_t;
//! The MPC calculate result type
typedef std::tuple<bool, int, int> mpc_result_t;
typedef std::map<int, std::vector<std::pair<int, std::string>>> graph_t;

struct TransitionSystem {
  virtual void get_actions(graph_t &graph,
                           std::vector<std::string> &gold_actions) = 0;
};

#if __cplusplus >= 201703L
namespace std {
template <class RandomIt> void random_shuffle(RandomIt first, RandomIt last) {
  std::random_device rng;
  std::mt19937 urng(rng());
  std::shuffle(first, last, urng);
}
} // namespace std
#endif

#endif //  end for __LTP_LSTMSDPARSER_SYSTEM_H__