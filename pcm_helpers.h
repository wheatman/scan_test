#pragma once
#include "pcm/src/cpucounters.h"
#include "pcm/src/utils.h"
#include <bitset>
namespace pcm {
#define MAX_CORES 4096

template <class State>
void print_basic_metrics(const PCM *m, const State &state1, const State &state2,
                         bool csv = false) {
  // std::cout << "\n " << getExecUsage(state1, state2) << "   "
  //           << getIPC(state1, state2) << "   "
  //           << getRelativeFrequency(state1, state2) << "\n";
  // if (m->isActiveRelativeFrequencyAvailable())
  //   std::cout << "    " << getActiveRelativeFrequency(state1, state2);
  if (!csv) {
    std::cout << "L3CacheMisses = "
              << unit_format(getL3CacheMisses(state1, state2)) << "\n";
    std::cout << "L2CacheMisses = "
              << unit_format(getL2CacheMisses(state1, state2)) << "\n";
    std::cout << "L3CacheHitRatio = " << getL3CacheHitRatio(state1, state2)
              << "\n";
    std::cout << "L2CacheHitRatio = " << getL2CacheHitRatio(state1, state2)
              << "\n";
    std::cout << "IPC = " << getIPC(state1, state2) << "\n";
    std::cout << "L3CacheOccupancy = "
              << unit_format(getL3CacheOccupancy(state2)) << "\n";
    std::cout << "BadSpeculation = " << getBadSpeculation(state1, state2)
              << "\n";
    std::cout << "getBytesReadFromMC = "
              << unit_format(getBytesReadFromMC(state1, state2)) << "\n";
    std::cout << "getBytesWrittenToMC = "
              << unit_format(getBytesWrittenToMC(state1, state2)) << "\n";
    std::cout << "getLLCReadMissLatency = "
              << getLLCReadMissLatency(state1, state2) << "\n";

  } else {
    std::cout << getL3CacheMisses(state1, state2) << ", "
              << getL2CacheMisses(state1, state2) << ", "
              << getL3CacheHitRatio(state1, state2) << ", "
              << getL2CacheHitRatio(state1, state2) << ", "
              << getIPC(state1, state2) << ", "
              << getBadSpeculation(state1, state2) << ", "
              << getBytesReadFromMC(state1, state2) << ", "
              << getBytesWrittenToMC(state1, state2) << ", "
              << getLLCReadMissLatency(state1, state2) << "\n";
  }
  // if (m->isL3CacheMissesAvailable())
  //   std::cout << "  "
  //             << double(getL3CacheMisses(state1, state2)) /
  //                    getInstructionsRetired(state1, state2);
  // if (m->isL2CacheMissesAvailable())
  //   std::cout << "  "
  //             << double(getL2CacheMisses(state1, state2)) /
  //                    getInstructionsRetired(state1, state2);
  // std::cout.precision(2);
}

} // namespace pcm