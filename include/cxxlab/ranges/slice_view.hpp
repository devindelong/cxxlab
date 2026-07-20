/**
 * @copyright Copyright (c) 2025, Devin DeLong. All rights reserved.
 *
 * @license This code is distributed under the BSD 3-Clause License.
 *          See the LICENSE file for the full text of the license.
 *
 * @author Devin DeLong
 */

#pragma once

#include "cxxlab/ranges/cached_iterator.hpp"
#include "cxxlab/type_traits/like.hpp"

#include <algorithm>
#include <cassert>
#include <concepts>
#include <ranges>
#include <type_traits>

namespace cxxlab::ranges
{

namespace detail
{
template <typename T>
concept const_copyable_or_movable =
   (std::is_const_v<std::remove_reference_t<T>> && std::copy_constructible<T>) ||
   std::move_constructible<T>;
} // namespace detail

/**
 * @brief A ranges slice view.
 * @details Implements a slice view that uses integral indices for the beginning and
 * ending of a range.
 */
template <std::ranges::viewable_range R>
class slice_view : public std::ranges::view_interface<slice_view<R>>
{

 public:
   /**
    * @brief Alias for the ranges difference type.
    */
   using difference_type = std::ranges::range_difference_t<R>;

   /**
    * @brief Defaulted constructor.
    */
   constexpr slice_view()
      requires std::default_initializable<R>
   = default;

   /**
    * @brief Constructor.
    * @param start Starting index of the slice.
    * @param end Ending index of the slice.
    */
   constexpr slice_view(R base, difference_type start, difference_type end)
       : base_{std::move(base)}, start_index_{start}, end_index_{end}
   {
      assert(start >= 0 && end >= 0 && end >= start);
   }

   /**
    * @brief Gets the underlying view.
    * @param self Explicit object parameter (deducing this)
    * @return A copy-constructed or move-constructed instance of the underlying view.
    */
   template <typename Self>
   [[nodiscard]] constexpr auto base(this Self&& self) -> R
      requires detail::const_copyable_or_movable<like_t<Self, R>>
   {
      return std::forward_like<Self>(self.base_);
   }

   /**
    * @brief Gets an iterator to the beginning of the slice view.
    * @return Iterator to the beginning of the slice view.
    */
   [[nodiscard]] constexpr auto begin() const
      requires std::ranges::range<const R>
   {
      return next_(start_index_);
   }

   /**
    * @brief Gets an iterator to the beginning of the slice view.
    * @return Iterator to the beginning of the slice view.
    */
   [[nodiscard]] constexpr auto begin()
      requires std::ranges::range<R>
   {
      if constexpr (cacheable_range<R>)
      {
         return cached_next_(begin_, start_index_);
      }
      else
      {
         return next_(start_index_);
      }
   }

   /**
    * @brief Gets an iterator to the end of the slice view.
    * @return Iterator to the end of the slice view.
    */
   [[nodiscard]] constexpr auto end() const
      requires std::ranges::range<const R>
   {
      return next_(end_index_);
   }

   /**
    * @brief Gets an iterator to the end of the slice view.
    * @return Iterator to the end of the slice view.
    */
   [[nodiscard]] constexpr auto end()
      requires std::ranges::range<R>
   {
      if constexpr (cacheable_range<R>)
      {
         return cached_next_(end_, end_index_);
      }
      else
      {
         return next_(end_index_);
      }
   }

   /**
    * @brief Gets the size of the sliced range.
    * @param self Explicit object parameter (deducing this)
    * @return Size of the range.
    */
   template <typename Self>
   [[nodiscard]] constexpr auto size(this Self&& self)
      requires std::ranges::sized_range<like_t<Self, R>>
   {
      constexpr auto zero = difference_type{0};
      const auto base_size = std::ranges::ssize(self.base_);
      auto slice_size = std::max(
         zero, std::clamp(self.end_index_, zero, base_size) -
                  std::clamp(self.start_index_, zero, base_size));
      return static_cast<std::make_unsigned_t<decltype(slice_size)>>(slice_size);
   }

 private:
   /**
    * @brief Get the next iterator advanced by n elements.
    * @param iter A cached iterator.
    * @param n number of elements to advance.
    * @return The resulting iterator advanced by n elements.
    */
   [[nodiscard]] constexpr auto cached_next_(cached_iterator_t<R>& iter, difference_type n)
      requires cacheable_range<R>
   {
      if (iter.has_value())
      {
         return *iter;
      }

      return iter.emplace(next_(n));
   }

   /**
    * @brief Get the next iterator advanced by n elements.
    * @param n number of elements to advance.
    * @return The resulting iterator advanced by n elements.
    */
   [[nodiscard]] constexpr auto next_(difference_type n) const
   {
      return std::ranges::next(std::ranges::begin(base_), n, std::ranges::end(base_));
   }

   R base_{};
   difference_type start_index_{0};
   difference_type end_index_{0};

   [[no_unique_address]] conditional_cached_iterator_t<R> begin_;
   [[no_unique_address]] conditional_cached_iterator_t<R> end_;
};

/**
 * @brief Deduction guide for slice_view.
 * @details Wraps the input range type in std::views::all_t.
 */
template <typename R>
slice_view(R&&, std::ranges::range_difference_t<R>, std::ranges::range_difference_t<R>)
   -> slice_view<std::views::all_t<R>>;

namespace views
{
namespace detail
{
template <typename R>
concept can_slice_view = requires {
   slice_view(
      std::declval<R>(), std::declval<std::ranges::range_difference_t<R>>(),
      std::declval<std::ranges::range_difference_t<R>>());
};

/**
 * @brief Range adaptor closure for @c slice_view.
 */
template <std::integral DifferenceType>
class slice_range_adaptor
    : public std::ranges::range_adaptor_closure<slice_range_adaptor<DifferenceType>>
{
 public:
   /*
    * @brief Constructor.
    * @param start Starting index of the slice.
    * @param end Ending index of the slice.
    */
   constexpr slice_range_adaptor(DifferenceType start, DifferenceType end)
       : start_{start}, end_{end}
   {
   }

   /**
    * @brief Required operator for range_adaptor_closure.
    * @param range The range to slice.
    * @return A slice view.
    */
   template <std::ranges::viewable_range R>
   [[nodiscard]] constexpr auto operator()(R&& r) const
      requires detail::can_slice_view<R>
   {
      return slice_view(
         std::forward<R>(r), static_cast<std::ranges::range_difference_t<R>>(start_),
         static_cast<std::ranges::range_difference_t<R>>(end_));
   }

 private:
   DifferenceType start_;
   DifferenceType end_;
};

struct slice_fn
{
   /**
    * @brief Constructs and returns a slice range adaptor.
    * @param start Starting index of the slice.
    * @param end Ending index of the slice.
    * @return A slice range adaptor object.
    */
   template <std::integral DifferenceType>
   [[nodiscard]] constexpr auto operator()(DifferenceType start, DifferenceType end) const
      -> slice_range_adaptor<DifferenceType>
   {
      return slice_range_adaptor{start, end};
   }
};
} // namespace detail

inline constexpr auto slice = detail::slice_fn{};

} // namespace views
} // namespace cxxlab::ranges

namespace cxxlab::views
{
using cxxlab::ranges::views::slice;
}
