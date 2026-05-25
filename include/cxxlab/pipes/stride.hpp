/**
 * @copyright Copyright (c) 2026, Devin DeLong. All rights reserved.
 *
 * @license This code is distributed under the BSD 3-Clause License.
 *          See the LICENSE file for the full text of the license.
 *
 * @author Devin DeLong
 */

#pragma once

#include "cxxlab/pipes/adaptor.hpp"
#include "cxxlab/pipes/iterator_interface.hpp"

#include <concepts>
#include <iterator>
#include <utility>

namespace cxxlab::pipes
{

// -----------------------------------------------------------------------------
// stride_iterator
// -----------------------------------------------------------------------------

template <std::weakly_incrementable Iterator, std::integral Integral>
class stride_iterator : public iterator_interface<Iterator>
{
 public:
   constexpr stride_iterator() = default;

   constexpr stride_iterator(Iterator iter, Integral stride)
       : iterator_interface<Iterator>{std::move(iter)}, stride_{stride}
   {
   }

   template <typename Elem>
   constexpr auto operator()(Elem&& elem) -> void
   {
      if (count_ == 0)
      {
         this->write(std::forward<Elem>(elem));
      }
      count_ = (++count_ == stride_) ? 0 : count_;
   }

   constexpr auto stride() const noexcept -> auto { return stride_; }

 private:
   Integral stride_{0};
   Integral count_{0};
};

// -----------------------------------------------------------------------------
// stride_adaptor
// -----------------------------------------------------------------------------

template <std::integral Integral>
using stride_adaptor = generic_adaptor<stride_iterator, Integral>;

// -----------------------------------------------------------------------------
// stride
// -----------------------------------------------------------------------------

namespace detail
{
struct stride_fn
{
   template <std::integral Integral>
   constexpr auto operator()(Integral stride) const
   {
      return stride_adaptor<Integral>{stride};
   }

   template <std::weakly_incrementable Iterator, std::integral Integral>
   constexpr auto operator()(Iterator&& iter, Integral stride) const
   {
      return stride_iterator{std::forward<Iterator>(iter), stride};
   }
};
} // namespace detail

inline constexpr auto stride = detail::stride_fn{};
} // namespace cxxlab::pipes
