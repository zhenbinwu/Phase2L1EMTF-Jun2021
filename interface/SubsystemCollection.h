#ifndef L1Trigger_Phase2L1EMTF_SubsystemCollection_h
#define L1Trigger_Phase2L1EMTF_SubsystemCollection_h

#include <tuple>
#include <variant>
#include <vector>

#include "L1Trigger/Phase2L1EMTF/interface/Common.h"
#include "L1Trigger/Phase2L1EMTF/interface/SubsystemTags.h"

namespace emtf {

  namespace phase2 {

    class SubsystemCollection {
    public:
      using poly_subsystem_type =
          std::variant<csc_subsystem_tag, rpc_subsystem_tag, gem_subsystem_tag, me0_subsystem_tag>;
      using poly_detid_type = std::variant<csc_subsystem_tag::detid_type,
                                           rpc_subsystem_tag::detid_type,
                                           gem_subsystem_tag::detid_type,
                                           me0_subsystem_tag::detid_type>;
      using poly_digi_type = std::variant<csc_subsystem_tag::digi_type,
                                          rpc_subsystem_tag::digi_type,
                                          gem_subsystem_tag::digi_type,
                                          me0_subsystem_tag::digi_type>;

      using first_container_type = std::vector<poly_subsystem_type>;
      using second_container_type = std::vector<poly_detid_type>;
      using third_container_type = std::vector<poly_digi_type>;

      template <typename T1, typename T2, typename T3>
      void push_back(T1 subsystem, const T2& detid, const T3& digi) {
        storage_subsystem_.push_back(subsystem);
        storage_detid_.push_back(detid);
        storage_digi_.push_back(digi);
        assert((storage_subsystem_.size() == storage_detid_.size()) and
               (storage_subsystem_.size() == storage_digi_.size()));
      }

      struct reference {
        using first_ref_type = first_container_type::const_iterator::reference;
        using second_ref_type = second_container_type::const_iterator::reference;
        using third_ref_type = third_container_type::const_iterator::reference;

        first_ref_type first_ref;
        second_ref_type second_ref;
        third_ref_type third_ref;
      };

      struct iterator {
        using first_iter_type = first_container_type::const_iterator;
        using second_iter_type = second_container_type::const_iterator;
        using third_iter_type = third_container_type::const_iterator;

        reference operator*() const { return reference{*first_iter, *second_iter, *third_iter}; }

        iterator& operator++() {
          ++first_iter;
          ++second_iter;
          ++third_iter;
          return *this;
        }

        friend bool operator==(const iterator& lhs, const iterator& rhs) {
          return std::tie(lhs.first_iter, lhs.second_iter, lhs.third_iter) ==
                 std::tie(rhs.first_iter, rhs.second_iter, rhs.third_iter);
        }

        friend bool operator!=(const iterator& lhs, const iterator& rhs) {
          return std::tie(lhs.first_iter, lhs.second_iter, lhs.third_iter) !=
                 std::tie(rhs.first_iter, rhs.second_iter, rhs.third_iter);
        }

        first_iter_type first_iter;
        second_iter_type second_iter;
        third_iter_type third_iter;
      };

      iterator begin() const {
        return iterator{storage_subsystem_.begin(), storage_detid_.begin(), storage_digi_.begin()};
      }

      iterator end() const { return iterator{storage_subsystem_.end(), storage_detid_.end(), storage_digi_.end()}; }

    private:
      first_container_type storage_subsystem_;
      second_container_type storage_detid_;
      third_container_type storage_digi_;
    };

  }  // namespace phase2

}  // namespace emtf

#endif  // L1Trigger_Phase2L1EMTF_SubsystemCollection_h not defined
