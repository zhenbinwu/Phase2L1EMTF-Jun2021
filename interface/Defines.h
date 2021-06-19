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

#endif  // L1Trigger_Phase2L1EMTF_Defines_h not defined
