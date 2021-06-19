#ifndef L1Trigger_Phase2L1EMTF_GeometryHelper_h
#define L1Trigger_Phase2L1EMTF_GeometryHelper_h

#include <type_traits>

#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ConsumesCollector.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/ESWatcher.h"

#include "MagneticField/Engine/interface/MagneticField.h"
#include "MagneticField/Records/interface/IdealMagneticFieldRecord.h"
#include "Geometry/DTGeometry/interface/DTGeometry.h"
#include "Geometry/CSCGeometry/interface/CSCGeometry.h"
#include "Geometry/RPCGeometry/interface/RPCGeometry.h"
#include "Geometry/GEMGeometry/interface/GEMGeometry.h"
#include "Geometry/GEMGeometry/interface/ME0Geometry.h"
#include "Geometry/Records/interface/MuonGeometryRecord.h"

namespace emtf {

  namespace phase2 {

    class GeometryHelper {
    public:
      explicit GeometryHelper(edm::ConsumesCollector&& iConsumes);
      explicit GeometryHelper(edm::ConsumesCollector& iConsumes);
      ~GeometryHelper();

      bool check(const edm::EventSetup& iSetup);

      constexpr const MagneticField& getMagneticField() const { return *magField_; }
      constexpr const DTGeometry& getDTGeometry() const { return *dtGeom_; }
      constexpr const CSCGeometry& getCSCGeometry() const { return *cscGeom_; }
      constexpr const RPCGeometry& getRPCGeometry() const { return *rpcGeom_; }
      constexpr const GEMGeometry& getGEMGeometry() const { return *gemGeom_; }
      constexpr const ME0Geometry& getME0Geometry() const { return *me0Geom_; }

      // Type-dependent get()
      template <typename T>
      using add_const_reference_t = typename std::add_lvalue_reference<typename std::add_const<T>::type>::type;

      template <typename T, typename U>
      using enable_if_same_t = typename std::enable_if<std::is_same<T, U>::value, int>::type;

      template <typename T, enable_if_same_t<T, MagneticField> = 0>
      constexpr add_const_reference_t<T> get() const {
        return getMagneticField();
      }
      template <typename T, enable_if_same_t<T, DTGeometry> = 0>
      constexpr add_const_reference_t<T> get() const {
        return getDTGeometry();
      }
      template <typename T, enable_if_same_t<T, CSCGeometry> = 0>
      constexpr add_const_reference_t<T> get() const {
        return getCSCGeometry();
      }
      template <typename T, enable_if_same_t<T, RPCGeometry> = 0>
      constexpr add_const_reference_t<T> get() const {
        return getRPCGeometry();
      }
      template <typename T, enable_if_same_t<T, GEMGeometry> = 0>
      constexpr add_const_reference_t<T> get() const {
        return getGEMGeometry();
      }
      template <typename T, enable_if_same_t<T, ME0Geometry> = 0>
      constexpr add_const_reference_t<T> get() const {
        return getME0Geometry();
      }

    private:
      // ESWatcher functions
      void watch_mag_field(const IdealMagneticFieldRecord& record) { magField_ = &(record.get(magFieldToken_)); }
      void watch_dt_geom(const MuonGeometryRecord& record) { dtGeom_ = &(record.get(dtGeomToken_)); }
      void watch_csc_geom(const MuonGeometryRecord& record) { cscGeom_ = &(record.get(cscGeomToken_)); }
      void watch_rpc_geom(const MuonGeometryRecord& record) { rpcGeom_ = &(record.get(rpcGeomToken_)); }
      void watch_gem_geom(const MuonGeometryRecord& record) { gemGeom_ = &(record.get(gemGeomToken_)); }
      void watch_me0_geom(const MuonGeometryRecord& record) { me0Geom_ = &(record.get(me0GeomToken_)); }

      // ESGetToken
      edm::ESGetToken<MagneticField, IdealMagneticFieldRecord> magFieldToken_;
      edm::ESGetToken<DTGeometry, MuonGeometryRecord> dtGeomToken_;
      edm::ESGetToken<CSCGeometry, MuonGeometryRecord> cscGeomToken_;
      edm::ESGetToken<RPCGeometry, MuonGeometryRecord> rpcGeomToken_;
      edm::ESGetToken<GEMGeometry, MuonGeometryRecord> gemGeomToken_;
      edm::ESGetToken<ME0Geometry, MuonGeometryRecord> me0GeomToken_;

      // ESWatcher
      edm::ESWatcher<IdealMagneticFieldRecord> magFieldWatcher_;
      edm::ESWatcher<MuonGeometryRecord> dtGeomWatcher_;
      edm::ESWatcher<MuonGeometryRecord> cscGeomWatcher_;
      edm::ESWatcher<MuonGeometryRecord> rpcGeomWatcher_;
      edm::ESWatcher<MuonGeometryRecord> gemGeomWatcher_;
      edm::ESWatcher<MuonGeometryRecord> me0GeomWatcher_;

      // ESHandle products
      // The products are returned as raw pointer. Please use with care.
      const MagneticField* magField_;
      const DTGeometry* dtGeom_;
      const CSCGeometry* cscGeom_;
      const RPCGeometry* rpcGeom_;
      const GEMGeometry* gemGeom_;
      const ME0Geometry* me0Geom_;
    };

  }  // namespace phase2

}  // namespace emtf

#endif  // L1Trigger_Phase2L1EMTF_GeometryHelper_h not defined
