#ifndef L1Trigger_Phase2L1EMTF_SubsystemTags_h
#define L1Trigger_Phase2L1EMTF_SubsystemTags_h

#include "DataFormats/MuonDetId/interface/CSCDetId.h"
#include "DataFormats/MuonDetId/interface/RPCDetId.h"
#include "DataFormats/MuonDetId/interface/GEMDetId.h"
#include "DataFormats/MuonDetId/interface/ME0DetId.h"

#include "DataFormats/CSCDigi/interface/CSCCorrelatedLCTDigi.h"
#include "DataFormats/CSCDigi/interface/CSCCorrelatedLCTDigiCollection.h"
#include "DataFormats/RPCRecHit/interface/RPCRecHit.h"
#include "DataFormats/RPCRecHit/interface/RPCRecHitCollection.h"
#include "DataFormats/GEMDigi/interface/GEMPadDigiCluster.h"
#include "DataFormats/GEMDigi/interface/GEMPadDigiClusterCollection.h"
#include "DataFormats/GEMDigi/interface/ME0TriggerDigi.h"
#include "DataFormats/GEMDigi/interface/ME0TriggerDigiCollection.h"

// Forward declarations
class CSCGeometry;
class RPCGeometry;
class GEMGeometry;
class ME0Geometry;

namespace emtf {

  namespace phase2 {

    struct csc_subsystem_tag {
      typedef CSCDetId detid_type;
      typedef CSCCorrelatedLCTDigi digi_type;
      typedef CSCCorrelatedLCTDigiCollection collection_type;
      typedef CSCGeometry detgeom_type;
    };

    struct rpc_subsystem_tag {
      typedef RPCDetId detid_type;
      typedef RPCRecHit digi_type;
      typedef RPCRecHitCollection collection_type;
      typedef RPCGeometry detgeom_type;
    };

    struct gem_subsystem_tag {
      typedef GEMDetId detid_type;
      typedef GEMPadDigiCluster digi_type;
      typedef GEMPadDigiClusterCollection collection_type;
      typedef GEMGeometry detgeom_type;
    };

    struct me0_subsystem_tag {
      typedef ME0DetId detid_type;
      typedef ME0TriggerDigi digi_type;
      typedef ME0TriggerDigiCollection collection_type;
      typedef ME0Geometry detgeom_type;
    };

  }  // namespace phase2

}  // namespace emtf

#endif  // L1Trigger_Phase2L1EMTF_SubsystemTags_h not defined
