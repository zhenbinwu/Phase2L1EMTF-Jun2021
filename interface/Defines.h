#ifndef L1Trigger_Phase2L1EMTF_Defines_h
#define L1Trigger_Phase2L1EMTF_Defines_h

#include <cassert>

// Uncomment the following line to use emtf_assert
#define EMTF_ALLOW_ASSERT

#ifdef EMTF_ALLOW_ASSERT
#define emtf_assert(expr) assert(expr)
#else
#define emtf_assert(expr) ((void)0)
#endif  // EMTF_ALLOW_ASSERT is defined

// Mark a variable to avoid compiler error about unused variables
#define emtf_maybe_unused(param) ((void)(param))

// _____________________________________________________________________________
// The following macro is used in SectorProcessor

// Uncomment the following line to dump debugging info
//#define EMTF_DUMP_INFO

// _____________________________________________________________________________
// The following macros are used in SegmentFormatter

// getQuartStripBit() is not yet available in 11_1_7
// getEighthStripBit() is not yet available in 11_1_7
#define EMTF_USE_CSC_HALFSTRIP

// getSlope() is not yet available in 11_1_7
#define EMTF_USE_CSC_RUN2_BEND

// Does not yet support BX=[-1,0] window
#define EMTF_USE_CSC_BX0_ONLY

// GE2/1 NumberPartition is 8 in 11_1_7
#define EMTF_USE_GEM_NPARTSGE21_8

#endif  // L1Trigger_Phase2L1EMTF_Defines_h not defined
