#pragma once

#if CILK == 1
#include <cilk/cilk_api.h>
#include <cilk/cilksan.h>
#endif

#include <algorithm>
#include <cstring>
#include <functional>
#include <vector>

template <class F> class Reducer {

#ifdef __cpp_lib_hardware_interference_size
  using std::hardware_constructive_interference_size;
  using std::hardware_destructive_interference_size;
#else
  // 64 bytes on x86-64 │ L1_CACHE_BYTES │ L1_CACHE_SHIFT │ __cacheline_aligned
  // │
  // ...
  static constexpr std::size_t hardware_constructive_interference_size = 64;
  static constexpr std::size_t hardware_destructive_interference_size = 64;
#endif

  struct aligned_f {
    alignas(hardware_destructive_interference_size) F f;
  };
  std::vector<aligned_f> data;

#if CILK == 1
  // so cilksan doesn't report races on accesses to the vector which I make sure
  // are fine by using getWorkerNum()
  Cilksan_fake_mutex fake_lock;
#endif

public:
  Reducer() {
#if CILK == 1
    data.resize(__cilkrts_get_nworkers());
#else
    data.resize(1);
#endif
  }
  void update(F new_values) {
#if CILK == 1
    int worker_num = __cilkrts_get_worker_number();

    Cilksan_fake_lock_guard guad(&fake_lock);
#else
    int worker_num = 0;
#endif
    data[worker_num].f.update(new_values);
  }
  F get() const {
    F output;
    for (const auto &d : data) {
      output.update(d.f);
    }
    return output;
  }
};

template <class T> class Reducer_sum {
  static_assert(std::is_integral<T>::value, "Integral required.");
  struct F {
    T value;
    void update(const F new_value) { value += new_value.value; }
    F(T t) : value(t) {}
    F() : value(0) {}
  };
  Reducer<F> reducer;

public:
  Reducer_sum(T initial_value = {}) { add(initial_value); }
  void add(T new_value) { reducer.update(new_value); }
  void inc() { reducer.update(1); }
  T get() const { return reducer.get().value; }
  Reducer_sum &operator++() {
    inc();
    return *this;
  }
  Reducer_sum &operator--() {
    add(-1);
    return *this;
  }

  Reducer_sum &operator-=(T new_value) {
    add(-new_value);
    return *this;
  }
  Reducer_sum &operator+=(T new_value) {
    add(+new_value);
    return *this;
  }

  friend bool operator==(const Reducer_sum &lhs, const Reducer_sum &rhs) {
    return lhs.get() == rhs.get();
  }
  operator T() const { return get(); }
};
