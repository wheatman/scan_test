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
T sum(const std::vector<T> &data, const std::vector<size_t> order,
      size_t block_size) {
  Reducer_sum<T> total = 0;
  if (order.size() > 1UL << 10) {
    cilk_for(size_t j = 0; j < order.size(); j++) {
      size_t el = order[j];
      T partial_total = 0;
      for (size_t i = el; i < el + block_size; i++) {
        partial_total += data[i];
      }
      total += partial_total;
    }
  } else {
    if (block_size >= 2048 && block_size % 2048 == 0) {
      cilk_for(size_t j = 0; j < order.size(); j++) {
        size_t el = order[j];
        cilk_for(size_t i = el; i < el + block_size; i += 256) {
          T partial_total = 0;
          for (size_t k = 0; k < 256; k++) {
            partial_total += data[i + k];
          }
          total += partial_total;
        }
      }
    } else {
      cilk_for(size_t j = 0; j < order.size(); j++) {
        size_t el = order[j];
        cilk_for(size_t i = el; i < el + block_size; i++) { total += data[i]; }
      }
    }
  }
  return total;
}

std::vector<size_t> create_order(size_t total_size, size_t block_size) {
  size_t order_length = total_size / block_size;
  std::vector<size_t> order(order_length);
  cilk_for(size_t i = 0; i < order_length; i++) { order[i] = i * block_size; }

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
  if (argc != 2) {
    printf("call with the log_2 of the test value\n");
  }
  size_t power_of_2 = std::strtol(argv[1], nullptr, 10);
  printf("bytes\n");
  test_all<uint8_t>(1UL << power_of_2);
  printf("shorts\n");
  test_all<uint16_t>(1UL << power_of_2);
  printf("ints\n");
  test_all<uint32_t>(1UL << power_of_2);
  printf("longs\n");
  test_all<uint64_t>(1UL << power_of_2);
  return 0;
}