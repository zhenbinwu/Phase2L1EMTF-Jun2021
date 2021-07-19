#ifndef L1Trigger_Phase2L1EMTF_NdArrayDesc_h
#define L1Trigger_Phase2L1EMTF_NdArrayDesc_h

#include <array>
#include <cassert>
#include <initializer_list>

namespace emtf {

  namespace phase2 {

    // Adapted from:
    // https://github.com/tensorflow/tensorflow/blob/master/tensorflow/lite/kernels/internal/common.h
    // Only works for N-dimensional arrays, N <= 4.
    class NdArrayDesc {
    public:
      typedef unsigned value_type;
      typedef std::array<value_type, 4> container_type;  // batch_size, height, width, depth

      NdArrayDesc();
      ~NdArrayDesc();

      template <typename T>
      constexpr NdArrayDesc(std::initializer_list<T> lst);

      constexpr value_type num_elements() const { return num_elements_; }

      constexpr value_type num_dimensions() const { return num_dimensions_; }

      constexpr bool is_valid() const { return num_elements_ >= 1; }

      // Compute 1-D index from n-D index
      constexpr value_type get_index(value_type i3) const {
        assert(i3 < extents_[3]);
        return (i3 * strides_[3]);
      }
      constexpr value_type get_index(value_type i2, value_type i3) const {
        assert(i2 < extents_[2]);
        assert(i3 < extents_[3]);
        return (i2 * strides_[2]) + (i3 * strides_[3]);
      }
      constexpr value_type get_index(value_type i1, value_type i2, value_type i3) const {
        assert(i1 < extents_[1]);
        assert(i2 < extents_[2]);
        assert(i3 < extents_[3]);
        return (i1 * strides_[1]) + (i2 * strides_[2]) + (i3 * strides_[3]);
      }
      constexpr value_type get_index(value_type i0, value_type i1, value_type i2, value_type i3) const {
        assert(i0 < extents_[0]);
        assert(i1 < extents_[1]);
        assert(i2 < extents_[2]);
        assert(i3 < extents_[3]);
        return (i0 * strides_[0]) + (i1 * strides_[1]) + (i2 * strides_[2]) + (i3 * strides_[3]);
      }

      // Compute 1-D index from n-D index provided as initializer_list
      template <typename T>
      constexpr value_type get_index(std::initializer_list<T> lst) const;

      // Retrieve element from a 1-D vector using the computed 1-D index
      template <typename T, typename U>
      constexpr auto get_vec_element(const T& vec, std::initializer_list<U> lst) const -> decltype(vec.at(0)) {
        return vec.at(get_index(lst));
      }

    private:
      container_type extents_;  // the extent of each dimension
      container_type strides_;  // the number of elements between consecutive indices of each dimension
      value_type num_elements_;
      value_type num_dimensions_;
    };

    // Implementation of the templated classes and functions

    template <typename T>
    constexpr NdArrayDesc::NdArrayDesc(std::initializer_list<T> lst) {
      // It is assumed that the input list does not contain any zeros.
      // If the list contains more than 4 entries, use only the first 4 entries.
      auto data = lst.begin();
      switch (lst.size()) {
        case 0:
          extents_[0] = 0;
          extents_[1] = 0;
          extents_[2] = 0;
          extents_[3] = 0;
          strides_[0] = 0;
          strides_[1] = 0;
          strides_[2] = 0;
          strides_[3] = 1;
          num_elements_ = 0;
          num_dimensions_ = 0;
          break;
        case 1:
          extents_[0] = 0;
          extents_[1] = 0;
          extents_[2] = 0;
          extents_[3] = data[0];
          strides_[0] = 0;
          strides_[1] = 0;
          strides_[2] = 0;
          strides_[3] = 1;
          num_elements_ = data[0];
          num_dimensions_ = 1;
          break;
        case 2:
          extents_[0] = 0;
          extents_[1] = 0;
          extents_[2] = data[0];
          extents_[3] = data[1];
          strides_[0] = 0;
          strides_[1] = 0;
          strides_[2] = data[1];
          strides_[3] = 1;
          num_elements_ = data[0] * data[1];
          num_dimensions_ = 2;
          break;
        case 3:
          extents_[0] = 0;
          extents_[1] = data[0];
          extents_[2] = data[1];
          extents_[3] = data[2];
          strides_[0] = 0;
          strides_[1] = data[1] * data[2];
          strides_[2] = data[2];
          strides_[3] = 1;
          num_elements_ = data[0] * data[1] * data[2];
          num_dimensions_ = 3;
          break;
        default:
          extents_[0] = data[0];
          extents_[1] = data[1];
          extents_[2] = data[2];
          extents_[3] = data[3];
          strides_[0] = data[1] * data[2] * data[3];
          strides_[1] = data[2] * data[3];
          strides_[2] = data[3];
          strides_[3] = 1;
          num_elements_ = data[0] * data[1] * data[2] * data[3];
          num_dimensions_ = 4;
          break;
      }  // end switch
    }

    template <typename T>
    constexpr NdArrayDesc::value_type NdArrayDesc::get_index(std::initializer_list<T> lst) const {
      // If the list contains more than 4 entries, use only the first 4 entries.
      decltype(lst.size()) size_four = 4;
      assert(std::min(lst.size(), size_four) <= num_dimensions_);
      auto data = lst.begin();
      switch (lst.size()) {
        case 0:
          return 0;
        case 1:
          return get_index(data[0]);
        case 2:
          return get_index(data[0], data[1]);
        case 3:
          return get_index(data[0], data[1], data[2]);
        default:
          return get_index(data[0], data[1], data[2], data[3]);
      }  // end switch
    }

  }  // namespace phase2

}  // namespace emtf

#endif  // L1Trigger_Phase2L1EMTF_NdArrayDesc_h not defined
