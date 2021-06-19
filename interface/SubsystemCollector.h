#ifndef L1Trigger_Phase2L1EMTF_SubsystemCollector_h
#define L1Trigger_Phase2L1EMTF_SubsystemCollector_h

#include <type_traits>

#include "FWCore/Framework/interface/Event.h"
#include "DataFormats/Common/interface/Handle.h"

#include "L1Trigger/Phase2L1EMTF/interface/Common.h"
#include "L1Trigger/Phase2L1EMTF/interface/SubsystemTags.h"
#include "L1Trigger/Phase2L1EMTF/interface/SubsystemCollection.h"

namespace emtf {

  namespace phase2 {

    class SubsystemCollector {
    public:
      // Case 1: assume MuonDigiCollection type, e.g. CSC, GEM, ME0.
      // Case 2: assume edm::RangeMap type, e.g. RPC.
      template <typename T>
      void collect(const edm::Event& iEvent, const edm::EDGetToken& token, SubsystemCollection& muon_primitives) const {
        typedef typename T::collection_type collection_type;
        if constexpr (is_muon_digi_collection<collection_type>::value) {
          collect_impl_1<T>(iEvent, token, muon_primitives);
        } else if constexpr (is_edm_range_map<collection_type>::value) {
          collect_impl_2<T>(iEvent, token, muon_primitives);
        } else {
          static_assert(dependent_false<T>::value, "unreachable!");
        }
      }

    private:
      template <typename>
      struct dependent_false;

      template <typename>
      struct is_muon_digi_collection;

      template <typename>
      struct is_edm_range_map;

      template <typename>
      struct get_detid_from_digi;

      // Collect from a MuonDigiCollection
      template <typename T>
      void collect_impl_1(const edm::Event& iEvent,
                          const edm::EDGetToken& token,
                          SubsystemCollection& muon_primitives) const;

      // Collect from a edm::RangeMap
      template <typename T>
      void collect_impl_2(const edm::Event& iEvent,
                          const edm::EDGetToken& token,
                          SubsystemCollection& muon_primitives) const;
    };

    // Implementation of the templated classes and functions

    // A type-dependent expression that is always false
    template <typename>
    struct SubsystemCollector::dependent_false : std::false_type {};

    // Detect MuonDigiCollection type
    template <typename>
    struct SubsystemCollector::is_muon_digi_collection : std::false_type {};

    template <typename T1, typename T2>
    struct SubsystemCollector::is_muon_digi_collection<MuonDigiCollection<T1, T2> > : std::true_type {};

    // Detect edm::RangeMap type
    template <typename>
    struct SubsystemCollector::is_edm_range_map : std::false_type {};

    template <typename T1, typename T2, typename T3>
    struct SubsystemCollector::is_edm_range_map<edm::RangeMap<T1, T2, T3> > : std::true_type {};

    // Functor to get detid from digi
    // Specialized to get RPCDetId from RPCRecHit
    template <typename>
    struct SubsystemCollector::get_detid_from_digi {};

    template <>
    struct SubsystemCollector::get_detid_from_digi<rpc_subsystem_tag::digi_type> {
      inline auto operator()(const rpc_subsystem_tag::digi_type& digi) const -> decltype(digi.rpcId()) {
        return digi.rpcId();
      }
    };

    // Collect from a MuonDigiCollection
    template <typename T>
    void SubsystemCollector::collect_impl_1(const edm::Event& iEvent,
                                            const edm::EDGetToken& token,
                                            SubsystemCollection& muon_primitives) const {
      typedef typename T::digi_type digi_type;
      typedef typename T::collection_type collection_type;

      edm::Handle<collection_type> handle;
      iEvent.getByToken(token, handle);

      auto chamber = handle->begin();
      auto chend = handle->end();
      for (; chamber != chend; ++chamber) {
        auto&& detid = (*chamber).first;
        auto digi = (*chamber).second.first;
        auto dend = (*chamber).second.second;
        for (; digi != dend; ++digi) {
          muon_primitives.push_back(T{}, detid, *digi);
        }
      }
    }

    // Collect from a edm::RangeMap
    template <typename T>
    void SubsystemCollector::collect_impl_2(const edm::Event& iEvent,
                                            const edm::EDGetToken& token,
                                            SubsystemCollection& muon_primitives) const {
      typedef typename T::digi_type digi_type;
      typedef typename T::collection_type collection_type;

      edm::Handle<collection_type> handle;
      iEvent.getByToken(token, handle);

      auto detid_getter = get_detid_from_digi<digi_type>{};
      auto digi = handle->begin();
      auto dend = handle->end();
      for (; digi != dend; ++digi) {
        auto&& detid = detid_getter(*digi);
        muon_primitives.push_back(T{}, detid, *digi);
      }
    }

  }  // namespace phase2

}  // namespace emtf

#endif  // L1Trigger_Phase2L1EMTF_SubsystemCollector_h not defined
