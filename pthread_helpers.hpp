

#include <cstddef>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <pthread.h>

struct ThreadArgs {
  std::function<void(int, int)> func;
  int start;
  int end;
};

inline void *threadFunction(void *arg) {
  ThreadArgs *args = static_cast<ThreadArgs *>(arg);
  args->func(args->start, args->end);
  pthread_exit(NULL);
}

template <typename F> inline void parallel_for(size_t start, size_t end, F f) {
  const int numThreads = PTHREAD_NUM_THREADS;
  pthread_t threads[numThreads];
  ThreadArgs threadArgs[numThreads];
  int per_thread = (end - start) / numThreads;

  // Create the threads and start executing the lambda function
  for (int i = 0; i < numThreads; i++) {
    threadArgs[i].func = [&f](int arg1, int arg2) {
      for (int k = arg1; k < arg2; k++) {
        f(k);
      }
    };

    threadArgs[i].start = start + (i * per_thread);
    if (i == numThreads - 1) {
      threadArgs[i].end = end;
    } else {
      threadArgs[i].end = start + ((i + 1) * per_thread);
    }
    int result =
        pthread_create(&threads[i], NULL, threadFunction, &threadArgs[i]);

    if (result != 0) {
      std::cerr << "Failed to create thread " << i << std::endl;
      exit(-1);
    }
  }

  // Wait for the threads to finish
  for (int i = 0; i < numThreads; i++) {
    pthread_join(threads[i], NULL);
  }
}

template <typename F>
inline void parallel_for_with_id(size_t start, size_t end, F f) {
  const int numThreads = 48;
  pthread_t threads[numThreads];
  ThreadArgs threadArgs[numThreads];
  int per_thread = (end - start) / numThreads;

  // Create the threads and start executing the lambda function
  for (int i = 0; i < numThreads; i++) {
    threadArgs[i].func = [&f, i](int arg1, int arg2) {
      for (int k = arg1; k < arg2; k++) {
        f(i, k);
      }
    };

    threadArgs[i].start = start + (i * per_thread);
    if (i == numThreads - 1) {
      threadArgs[i].end = end;
    } else {
      threadArgs[i].end = start + ((i + 1) * per_thread);
    }
    int result =
        pthread_create(&threads[i], NULL, threadFunction, &threadArgs[i]);

    if (result != 0) {
      std::cerr << "Failed to create thread " << i << std::endl;
      exit(-1);
    }
  }

  // Wait for the threads to finish
  for (int i = 0; i < numThreads; i++) {
    pthread_join(threads[i], NULL);
  }
}