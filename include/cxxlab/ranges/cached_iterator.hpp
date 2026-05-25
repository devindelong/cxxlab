/**
 * @copyright Copyright (c) 2026, Devin DeLong. All rights reserved.
 *
 * @license This code is distributed under the BSD 3-Clause License.
 *          See the LICENSE file for the full text of the license.
 *
 * @author Devin DeLong
 */

#pragma once

#include "cxxlab/ranges/transient_cache.hpp"
#include "cxxlab/type_traits/conditional_storage.hpp"

#include <ranges>

namespace cxxlab::ranges
{

/**
 * @brief Concept for checking if the iterators of a range can and should be cached.
 *
 * The purpose of a cacheable range is to cache the iterators from calling begin() and
 * end() for potentially expensive views. A cacheable range is technically any forward
 * range. Random access, sized ranges are not cacheable ranges, since their begin() and
 * end() functions are performant.
 */
template <typename R>
concept cacheable_range = std::ranges::forward_range<R> &&
                          !(std::ranges::random_access_range<R> && std::ranges::sized_range<R>);

/**
 * @brief Cached iterator type.
 *
 * The resulting type is a transient_cache that does not propagate it's state when
 * copied or moved.
 *
 * @tparam R A range.
 */
template <std::ranges::range R>
using cached_iterator_t = transient_cache<std::ranges::iterator_t<R>>;

/**
 * @brief Conditionally cached iterator type.
 *
 * If the range satisfies the concept cacheable_range, then the type is cached_iterator_t.
 * If not, then the resulting type is an unspecified unique empty type.
 *
 * This is intended to be used in conjunction with [[no_unique_address]].
 *
 * @tparam R A range.
 * @tparam E Must be an empty type.
 */
template <std::ranges::range R, typename E = CXXLAB_UNIQUE_EMPTY_TYPE>
using conditional_cached_iterator_t =
   conditional_storage_t<cacheable_range<R>, cached_iterator_t<R>, E>;

} // namespace cxxlab::ranges
