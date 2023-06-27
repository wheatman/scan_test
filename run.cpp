#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <iostream>
#include <limits>
#include <random>
#include <string>
#include <sys/time.h>
#include <vector>

#include "ParallelTools/parallel.h"
#include "ParallelTools/reducer.h"

static inline uint64_t get_usecs() {
  struct timeval st {};
  gettimeofday(&st, nullptr);
  return st.tv_sec * 1000000 + st.tv_usec;
}

template <class T> std::vector<T> create_random_data(size_t n) {

  std::vector<T> v(n);
  ParallelTools::parallel_for(0, n, [&](size_t i) { v[i] = i; });

  return v;
}

template <class T>
T sum(const std::vector<T> &data, const std::vector<size_t> &order,
      size_t block_size) {
  ParallelTools::Reducer_sum<T> total = 0;
  ParallelTools::parallel_for(0, order.size(), [&](size_t j) {
    size_t el = order[j];
    T partial_total = 0;
    for (size_t i = el; i < el + block_size; i++) {
      partial_total += data[i];
    }
    total += partial_total;
  });

  return total;
}

std::vector<size_t> create_order(size_t total_size, size_t block_size) {
  size_t order_length = total_size / block_size;
  std::vector<size_t> order(order_length);
  ParallelTools::parallel_for(0, order_length,
                              [&](size_t i) { order[i] = i * block_size; });

  std::random_device rd;
  std::mt19937 g(rd());

  std::shuffle(order.begin(), order.end(), g);
  return order;
}

template <class T> void test_all(size_t total_byte_count) {
  size_t total_element_count = total_byte_count / sizeof(T);
  auto data = create_random_data<T>(total_element_count);
  for (size_t block_size = 1; block_size <= total_element_count;
       block_size *= 2) {
    auto order = create_order(total_element_count, block_size);
    uint64_t start = get_usecs();
    auto total = sum(data, order, block_size);
    uint64_t end = get_usecs();
    printf("%lu, %lu, %lu, %lu\n", total_byte_count, block_size * sizeof(T),
           end - start, uint64_t(total));
  }
}

int main(int argc, char *argv[]) {
  if (argc == 2) {
    size_t power_of_2 = std::strtol(argv[1], nullptr, 10);
    // printf("bytes\n");
    // test_all<uint8_t>(1UL << power_of_2);
    // printf("shorts\n");
    // test_all<uint16_t>(1UL << power_of_2);
    // printf("ints\n");
    // test_all<uint32_t>(1UL << power_of_2);
    printf("longs\n");
    test_all<uint64_t>(1UL << power_of_2);
  } else if (argc == 3) {
    size_t power_of_2 = std::strtol(argv[1], nullptr, 10);
    uint64_t total_element_count = (1UL << power_of_2) / sizeof(uint64_t);
    uint64_t block_size = std::strtol(argv[2], nullptr, 10) / sizeof(uint64_t);
    auto order = create_order(total_element_count, block_size);

    auto data = create_random_data<uint64_t>(total_element_count);
    uint64_t start = get_usecs();
    auto total = sum(data, order, block_size);
    uint64_t end = get_usecs();
    printf("%lu, %lu, %lu, %lu\n", total_element_count * sizeof(uint64_t),
           block_size * sizeof(uint64_t), end - start, uint64_t(total));
  } else {
    printf("call with the log_2 of the test value\n");
    printf("can optionally specify the block size\n");
  }
  return 0;
}