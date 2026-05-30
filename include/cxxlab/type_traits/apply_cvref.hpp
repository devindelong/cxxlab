/**
 * @copyright Copyright (c) 2026, Devin DeLong. All rights reserved.
 *
 * @license This code is distributed under the BSD 3-Clause License.
 *          See the LICENSE file for the full text of the license.
 *
 * @author Devin DeLong
 */

#pragma once

#include <type_traits>

namespace cxxlab
{

// --------------------------------------------------------------------------------
// apply_const
// --------------------------------------------------------------------------------

/**
 * @brief If the type From is const, this adds const to the type To.
 * @tparam From The type to check for const.
 * @tparam To The type to add const to.
 */
template <typename From, typename To>
struct apply_const
    : std::type_identity<std::conditional_t<std::is_const_v<From>, std::add_const_t<To>, To>>
{
};

/**
 * @brief If the type From is const, this adds const to the type To.
 * @tparam From The type to check for const.
 * @tparam To The type to add const to.
 */
template <typename From, typename To>
using apply_const_t = apply_const<From, To>::type;

// --------------------------------------------------------------------------------
// apply_volatile
// --------------------------------------------------------------------------------

/**
 * @brief If the type From is volatile, this adds volatile to the type To.
 * @tparam From The type to check for volatile.
 * @tparam To The type to add volatile to.
 */
template <typename From, typename To>
struct apply_volatile
    : std::type_identity<std::conditional_t<std::is_volatile_v<From>, std::add_volatile_t<To>, To>>
{
};

/**
 * @brief If the type From is volatile, this adds volatile to the type To.
 * @tparam From The type to check for volatile.
 * @tparam To The type to add volatile to.
 */
template <typename From, typename To>
using apply_volatile_t = apply_volatile<From, To>::type;

// --------------------------------------------------------------------------------
// apply_cv
// --------------------------------------------------------------------------------

/**
 * @brief Adds any top-level const or volatile qualifiers from the type From to the type
 * To.
 * @tparam From The type to check for const/volatile.
 * @tparam To The type to add const/volatile to.
 */
template <typename From, typename To>
struct apply_cv : std::type_identity<apply_const_t<From, apply_volatile_t<From, To>>>
{
};

/**
 * @brief Adds any top-level const or volatile qualifiers from the type From to the type
 * To.
 * @tparam From The type to check for const/volatile.
 * @tparam To The type to add const/volatile to.
 */
template <typename From, typename To>
using apply_cv_t = apply_cv<From, To>::type;

// --------------------------------------------------------------------------------
// apply_reference
// --------------------------------------------------------------------------------

/**
 * @brief Adds reference qualifiers from the type From to the type To.
 *
 * @tparam From The type to check for references.
 * @tparam To The type to add references to.
 */
template <typename From, typename To>
struct apply_reference
    : std::type_identity<std::conditional_t<
         std::is_lvalue_reference_v<From>,
         std::add_lvalue_reference_t<To>,
         std::conditional_t<std::is_rvalue_reference_v<From>, std::add_rvalue_reference_t<To>, To>>>
{
};

/**
 * @brief Adds top-level reference qualifiers from the type From to the type To.
 *
 * @tparam From The type to check for references.
 * @tparam To The type to add references to.
 */
template <typename From, typename To>
using apply_reference_t = apply_reference<From, To>::type;

} // namespace cxxlab
