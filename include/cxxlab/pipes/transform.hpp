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
// transform_iterator
// -----------------------------------------------------------------------------

/**
 * @brief And output iterator that transforms its output.
 */
template <std::weakly_incrementable Iterator, typename Transform>
class transform_iterator : public iterator_interface<Iterator>
{
 public:
   /**
    * @brief Defaulted constructor..
    * @detail This constructor leaves the iterator in an invalid an unusable state.
    */
   constexpr transform_iterator()
      requires std::default_initializable<Transform>
   = default;

   /**
    * @brief Constructs from an ourput iterator an a transform function.
    * @param iter An output iterator,
    * @param transform Function to transform the values pointed to by the iterator.
    */
   constexpr transform_iterator(Iterator iter, Transform transform)
       : iterator_interface<Iterator>{std::move(iter)}, transform_{std::move(transform)}
   {
   }

   /**
    * @brief Constructs from an ourput iterator an a transform function.
    * @param elem The element to transform and write to the iterator contained iterator.
    * @return *this
    */
   template <typename Elem>
   constexpr auto operator()(Elem&& elem) -> void
      requires std::invocable<Transform&, Elem>
   {
      this->write(std::invoke(transform_, std::forward<Elem>(elem)));
   }

   /**
    * @brief Gets the transform function.
    * @return Returns a forward-constructed copy of the transform function.
    */
   template <typename Self>
   constexpr auto transform(this Self&& self) -> auto
   {
      return std::forward_like<Self>(self.transform_);
   }

 private:
   [[no_unique_address]] Transform transform_;
};

// -----------------------------------------------------------------------------
// transform_adapter
// -----------------------------------------------------------------------------

template <typename Transform>
using transform_adaptor = generic_adaptor<transform_iterator, Transform>;

// -----------------------------------------------------------------------------
// transform
// -----------------------------------------------------------------------------

namespace detail
{
struct transform_fn
{
   template <typename Transform>
   constexpr auto operator()(Transform&& transform) const
   {
      return transform_adaptor<std::remove_cvref_t<Transform>>{std::forward<Transform>(transform)};
   }

   template <std::weakly_incrementable Iterator, typename Transform>
   constexpr auto operator()(Iterator&& iter, Transform&& transform) const
   {
      return transform_iterator{std::forward<Iterator>(iter), std::forward<Transform>(transform)};
   }
};
} // namespace detail

inline constexpr auto transform = detail::transform_fn{};

} // namespace cxxlab::pipes
