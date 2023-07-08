OPT?=3
VALGRIND?=0
SANITIZE?=0
CILK=0
PARLAY?=0
PTHREAD?=0
PARALLEL=0







PARALLEL=0

CFLAGS := -Wall -Wno-address-of-packed-member -Wextra -O$(OPT) -g  -std=c++20 -gdwarf-4 -IParallelTools/

LDFLAGS := -lrt -lm -lm -ldl 

ifeq ($(CILK),1)
PARLAY=0
PTHREAD=0
PARALLEL=1
CFLAGS += -fopencilk
endif

ifeq ($(PARLAY),1)
PTHREAD=0
PARALLEL=1
CFLAGS += -Iparlaylib/include/
LDFLAGS += -lpthread
endif

ifeq ($(PTHREAD),1)
PARALLEL=1
LDFLAGS += -lpthread
PTHREAD_NUM_THREADS?=48
endif


ifeq ($(SANITIZE),1)
ifeq ($(CILK),1)
CFLAGS += -fsanitize=cilk,undefined -fno-omit-frame-pointer
else
CFLAGS += -fsanitize=undefined,address -fno-omit-frame-pointer
endif
endif

ifeq ($(OPT),3)
CFLAGS += -fno-signed-zeros  -freciprocal-math -ffp-contract=fast -fno-trapping-math  -ffinite-math-only
endif

ifeq ($(VALGRIND),0)
CFLAGS += -march=native #-static
endif


DEFINES :=  -DCILK=$(CILK) -DPARLAY=$(PARLAY) -DPTHREAD=$(PTHREAD) -DPTHREAD_NUM_THREADS=$(PTHREAD_NUM_THREADS)
SRC := run.cpp


.PHONY: all clean tidy

all:  basic 
#build_profile profile opt


basic: $(SRC)
	$(CXX) $(CFLAGS) $(DEFINES) -DNDEBUG $(SRC) $(LDFLAGS) -o basic

pcm: run_with_pcm.cpp
	$(CXX) $(CFLAGS) $(DEFINES) -DNDEBUG run_with_pcm.cpp $(LDFLAGS) -L./pcm/build/lib -l:libpcm.so  -o basic_pcm

prefetch_test: prefetch_test.cpp
	$(CXX) $(CFLAGS) $(DEFINES) -DNDEBUG prefetch_test.cpp $(LDFLAGS) -o prefetch_test

seporation_test: seporation_test.cpp
	$(CXX) $(CFLAGS) $(DEFINES) -DNDEBUG seporation_test.cpp $(LDFLAGS) -o seporation_test


clean:
	rm -f run run_profile run.dump run_basic run.gcda run_basic.dump *.profdata *.profraw test_out/* test basic opt
	rm -f bfs.out bc.out pr.out cc.out bf.out
	rm -f coverage_* perf.data perf.data.old

