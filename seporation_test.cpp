#include "reducer.hpp"
#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <iostream>
#include <limits>
#include <random>
#include <string>
#include <sys/time.h>
#include <vector>

#if CILK == 1
#include <cilk/cilk.h>
#else
#define cilk_for for
#endif

static inline uint64_t get_usecs() {
  struct timeval st {};
  gettimeofday(&st, nullptr);
  return st.tv_sec * 1000000 + st.tv_usec;
}

template <class T> std::vector<T> create_random_data(size_t n) {

  std::vector<T> v(n);
  cilk_for(size_t i = 0; i < n; i++) { v[i] = i; }

  return v;
}

template <class T>
T sum(const std::vector<T> &data,
      const std::vector<std::pair<size_t, size_t>> &order, size_t block_size) {
  Reducer_sum<T> total = 0;
  cilk_for(size_t j = 0; j < order.size(); j++) {
    auto &[el_1, el_2] = order[j];
    T partial_total = 0;
    for (size_t i = el_1; i < el_1 + block_size; i++) {
      partial_total += data[i];
    }
    if (el_2 < data.size()) {
      for (size_t i = el_2; i < el_2 + block_size; i++) {
        partial_total += data[i];
      }
    }
    total += partial_total;
  }
  return total;
}

// total size and block size in elements
// seperation in blocks, 1 means the next block

std::vector<std::pair<size_t, size_t>>
create_order(size_t total_size, size_t block_size, size_t seperation_amount) {
  size_t order_length = total_size / block_size;
  std::vector<std::pair<size_t, size_t>> order(order_length);
  cilk_for(size_t i = 0; i < order_length; i++) {
    order[i] = {i * block_size,
                (i * block_size) + (block_size * seperation_amount)};
  }

  std::vector<bool> already_done(order_length);
  std::vector<std::pair<size_t, size_t>> output;
  for (const auto &[first, second] : order) {
    if (already_done[first / block_size]) {
      continue;
    }
    already_done[second / block_size] = true;
    output.emplace_back(first, second);
  }

  std::random_device rd;
  std::mt19937 g(rd());

  std::shuffle(output.begin(), output.end(), g);

  return output;
}

template <class T>
void test_all(size_t total_byte_count, const std::vector<size_t> &block_sizes,
              const std::vector<size_t> &seperation_amounts) {
  size_t total_element_count = total_byte_count / sizeof(T);
  auto data = create_random_data<T>(total_element_count);
  for (auto block_size : block_sizes) {
    for (auto seperation_amount : seperation_amounts) {
      auto order =
          create_order(total_element_count, block_size, seperation_amount);
      uint64_t start = get_usecs();
      auto total = sum(data, order, block_size);
      uint64_t end = get_usecs();
      printf("%lu, %lu, %lu, %lu, %lu, %lu\n", total_byte_count,
             block_size * sizeof(T), seperation_amount * block_size * sizeof(T),
             end - start, uint64_t(total), order.size());
    }
  }
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    printf("call with the log_2 of the test value\n");
  }
  size_t power_of_2 = std::strtol(argv[1], nullptr, 10);
  printf("longs\n");
  std::vector<size_t> block_size = {1, 2, 4, 8, 16, 32, 64, 128};
  std::vector<size_t> seperation_amount = {1, 2, 3, 4, 5, 6, 7, 8, 16, 32};
  test_all<uint64_t>(1UL << power_of_2, block_size, seperation_amount);
  return 0;
}