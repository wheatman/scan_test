
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <iostream>

uint64_t read_test(uint64_t size) {
  uint64_t *data = (uint64_t *)malloc(size * sizeof(uint64_t));
  uint64_t total = 0;
  for (uint64_t i = 0; i < size; i++) {
    total += data[i];
  }
  return total;
}

uint64_t write_read_test(uint64_t size) {
  uint64_t *data = (uint64_t *)malloc(size * sizeof(uint64_t));
  for (uint64_t i = 0; i < size; i++) {
    data[i] = i;
  }
  uint64_t total = 0;
  for (uint64_t i = 0; i < size; i++) {
    total += data[i];
  }
  return total;
}

int main(int argc, char *argv[]) {
  if (argc != 3) {
    printf("call with the log_2 of the test value and if read or read/write "
           "mode\n");
  }
  size_t power_of_2 = std::strtol(argv[1], nullptr, 10);
  size_t mode = std::strtol(argv[2], nullptr, 10);
  if (mode == 1) {
    uint64_t total = read_test(1UL << power_of_2);
    std::cout << total << "\n";
  } else {
    uint64_t total = write_read_test(1UL << power_of_2);
    std::cout << total << "\n";
  }
  return 0;
}