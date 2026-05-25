/**
 * @file type_traits.hpp
 * @copyright Copyright (c) 2025, Devin DeLong. All rights reserved.
 *
 * @license This code is distributed under the BSD 3-Clause License.
 *          See the LICENSE file for the full text of the license.
 *
 * @author Devin DeLong
 */

#pragma once

#include "cxxlab/type_traits/copy_cvref.hpp"

namespace cxxlab
{

// --------------------------------------------------------------------------------
// like_t (same as remove_cvref_t)
// --------------------------------------------------------------------------------

/**
 * @brief Copies const, volatile, and reference qualifiers from the type From to the type
 * To.
 *
 * @tparam From The type to check for const, volatile, and references qualifiers.
 * @tparam To The type to copy const, volatile, and references qualifiers to.
 */
template <typename From, typename To>
using like = copy_cvref<From, To>;

/**
 * @brief Copies const, volatile, and reference qualifiers from the type From to the type
 * To.
 *
 * @tparam From The type to check for const, volatile, and references qualifiers.
 * @tparam To The type to copy const, volatile, and references qualifiers to.
 */
template <typename From, typename To>
using like_t = like<From, To>::type;

} // namespace cxxlab
