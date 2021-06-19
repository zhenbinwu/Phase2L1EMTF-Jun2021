#include "L1Trigger/Phase2L1EMTF/interface/GeometryHelper.h"

using namespace emtf::phase2;

GeometryHelper::GeometryHelper(edm::ConsumesCollector&& iConsumes)
    : magFieldToken_(iConsumes.esConsumes<MagneticField, IdealMagneticFieldRecord>()),
      dtGeomToken_(iConsumes.esConsumes<DTGeometry, MuonGeometryRecord>()),
      cscGeomToken_(iConsumes.esConsumes<CSCGeometry, MuonGeometryRecord>()),
      rpcGeomToken_(iConsumes.esConsumes<RPCGeometry, MuonGeometryRecord>()),
      gemGeomToken_(iConsumes.esConsumes<GEMGeometry, MuonGeometryRecord>()),
      me0GeomToken_(iConsumes.esConsumes<ME0Geometry, MuonGeometryRecord>()),
      magFieldWatcher_(this, &GeometryHelper::watch_mag_field),
      dtGeomWatcher_(this, &GeometryHelper::watch_dt_geom),
      cscGeomWatcher_(this, &GeometryHelper::watch_csc_geom),
      rpcGeomWatcher_(this, &GeometryHelper::watch_rpc_geom),
      gemGeomWatcher_(this, &GeometryHelper::watch_gem_geom),
      me0GeomWatcher_(this, &GeometryHelper::watch_me0_geom),
      magField_(nullptr),
      dtGeom_(nullptr),
      cscGeom_(nullptr),
      rpcGeom_(nullptr),
      gemGeom_(nullptr),
      me0Geom_(nullptr) {}

GeometryHelper::GeometryHelper(edm::ConsumesCollector& iConsumes)
    : magFieldToken_(iConsumes.esConsumes<MagneticField, IdealMagneticFieldRecord>()),
      dtGeomToken_(iConsumes.esConsumes<DTGeometry, MuonGeometryRecord>()),
      cscGeomToken_(iConsumes.esConsumes<CSCGeometry, MuonGeometryRecord>()),
      rpcGeomToken_(iConsumes.esConsumes<RPCGeometry, MuonGeometryRecord>()),
      gemGeomToken_(iConsumes.esConsumes<GEMGeometry, MuonGeometryRecord>()),
      me0GeomToken_(iConsumes.esConsumes<ME0Geometry, MuonGeometryRecord>()),
      magFieldWatcher_(this, &GeometryHelper::watch_mag_field),
      dtGeomWatcher_(this, &GeometryHelper::watch_dt_geom),
      cscGeomWatcher_(this, &GeometryHelper::watch_csc_geom),
      rpcGeomWatcher_(this, &GeometryHelper::watch_rpc_geom),
      gemGeomWatcher_(this, &GeometryHelper::watch_gem_geom),
      me0GeomWatcher_(this, &GeometryHelper::watch_me0_geom),
      magField_(nullptr),
      dtGeom_(nullptr),
      cscGeom_(nullptr),
      rpcGeom_(nullptr),
      gemGeom_(nullptr),
      me0Geom_(nullptr) {}

GeometryHelper::~GeometryHelper() {}

bool GeometryHelper::check(const edm::EventSetup& iSetup) {
  bool changed = false;
  bool changed_i = false;
  changed_i = magFieldWatcher_.check(iSetup);
  changed |= changed_i;
  changed_i = dtGeomWatcher_.check(iSetup);
  changed |= changed_i;
  changed_i = cscGeomWatcher_.check(iSetup);
  changed |= changed_i;
  changed_i = rpcGeomWatcher_.check(iSetup);
  changed |= changed_i;
  changed_i = gemGeomWatcher_.check(iSetup);
  changed |= changed_i;
  changed_i = me0GeomWatcher_.check(iSetup);
  changed |= changed_i;
  return changed;
}
