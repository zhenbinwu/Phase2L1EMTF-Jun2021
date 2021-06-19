#ifndef L1Trigger_Phase2L1EMTF_Common_h
#define L1Trigger_Phase2L1EMTF_Common_h

#include <cstdint>
#include <memory>

#include "DataFormats/L1TMuonPhase2/interface/Phase2L1EMTFHit.h"
#include "DataFormats/L1TMuonPhase2/interface/Phase2L1EMTFTrack.h"

#include "L1Trigger/Phase2L1EMTF/interface/Defines.h"

namespace emtf {

  namespace phase2 {

    // from DataFormats/MuonDetId/interface/CSCDetId.h
    constexpr int MIN_ENDCAP = 1;
    constexpr int MAX_ENDCAP = 2;

    // from DataFormats/MuonDetId/interface/CSCTriggerNumbering.h
    constexpr int MIN_TRIGSECTOR = 1;
    constexpr int MAX_TRIGSECTOR = 6;
    constexpr int NUM_TRIGSECTORS = 12;

    // Containers
    typedef l1t::Phase2L1EMTFHit EMTFHit;
    typedef l1t::Phase2L1EMTFHitCollection EMTFHitCollection;
    typedef l1t::Phase2L1EMTFTrack EMTFTrack;
    typedef l1t::Phase2L1EMTFTrackCollection EMTFTrackCollection;

  }  // namespace phase2

}  // namespace emtf

#endif  // L1Trigger_Phase2L1EMTF_Common_h not defined
