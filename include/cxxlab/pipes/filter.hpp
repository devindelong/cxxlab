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
#include <functional>
#include <iterator>
#include <type_traits>
#include <utility>

namespace cxxlab::pipes
{

// -----------------------------------------------------------------------------
// filter_iterator
// -----------------------------------------------------------------------------

template <std::weakly_incrementable Iterator, typename Predicate>
class filter_iterator : public iterator_interface<Iterator>
{
 public:
   constexpr filter_iterator()
      requires std::default_initializable<Predicate>
   = default;

   constexpr filter_iterator(Iterator iter, Predicate pred)
       : iterator_interface<Iterator>{std::move(iter)}, predicate_{std::move(pred)}
   {
   }

   template <typename Elem>
   constexpr auto operator()(Elem&& elem) -> void
      requires std::predicate<Predicate&, Elem&>
   {
      if (std::invoke(predicate_, elem))
      {
         this->write(std::forward<Elem>(elem));
      }
   }

   template <typename Self>
   constexpr auto predicate(this Self&& self) -> auto
   {
      return std::forward_like<Self>(self.predicate_);
   }

 private:
   [[no_unique_address]] Predicate predicate_;
};

// -----------------------------------------------------------------------------
// filter_adaptor
// -----------------------------------------------------------------------------

template <typename Predicate>
using filter_adaptor = generic_adaptor<filter_iterator, Predicate>;

// -----------------------------------------------------------------------------
// filter
// -----------------------------------------------------------------------------

namespace detail
{
struct filter_fn
{
   template <typename Predicate>
   constexpr auto operator()(Predicate&& predicate) const
   {
      return filter_adaptor<std::remove_cvref_t<Predicate>>{std::forward<Predicate>(predicate)};
   }

   template <std::weakly_incrementable Iterator, typename Predicate>
   constexpr auto operator()(Iterator&& iter, Predicate&& predicate) const
   {
      return filter_iterator{std::forward<Iterator>(iter), std::forward<Predicate>(predicate)};
   }
};
} // namespace detail

inline constexpr auto filter = detail::filter_fn{};

} // namespace cxxlab::pipes
