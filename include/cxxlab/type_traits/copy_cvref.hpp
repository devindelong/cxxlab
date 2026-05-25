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

#include "cxxlab/type_traits/apply_cvref.hpp"

#include <type_traits>

namespace cxxlab
{

// --------------------------------------------------------------------------------
// copy_const
// --------------------------------------------------------------------------------

/**
 * @brief Copies the const-ness of the type From to the type To.
 *
 * If the type From is const, then const is added to the type To. If the type From is not
 * const, then const will be removed from the type To.
 *
 * @tparam From The type to check for const.
 * @tparam To The type to add const to.
 */
template <typename From, typename To>
struct copy_const : apply_const<From, std::remove_const_t<To>>
{
};

/**
 * @brief Copies the const-ness of the type From to the type To.
 *
 * This overwrites the destination type To. If the type From is const, then const is added
 * to the type To. If the type From is not const, then const will be removed from the type
 * To.
 *
 * @tparam From The type to check for const.
 * @tparam To The type to add const to.
 */
template <typename From, typename To>
using copy_const_t = copy_const<From, To>::type;

// --------------------------------------------------------------------------------
// copy_volatile
// --------------------------------------------------------------------------------

/**
 * @brief Copies the volatile-ness of the type From to the type To.
 *
 * If the type From is volatile, then volatile is added to the type To. If the type From
 * is not volatile, then volatile will be removed from the type To.
 *
 * @tparam From The type to check for volatile.
 * @tparam To The type to copy volatile to.
 */
template <typename From, typename To>
struct copy_volatile : apply_volatile<From, std::remove_volatile_t<To>>
{
};

/**
 * @brief Copies the volatile-ness of the type From to the type To.
 *
 * If the type From is volatile, then volatile is added to the type To. If the type From
 * is not volatile, then volatile will be removed from the type To.
 *
 * @tparam From The type to check for volatile.
 * @tparam To The type to copy volatile to.
 */
template <typename From, typename To>
using copy_volatile_t = copy_volatile<From, To>::type;

// --------------------------------------------------------------------------------
// copy_cv
// --------------------------------------------------------------------------------

/**
 * @brief Copies top-level const or volatile qualifiers from the type From to the type To.
 *
 * Top-level const and volatile qualifiers for the type To are completely overwritten to
 * match the type From.
 *
 * @tparam From The type to check for const/volatile.
 * @tparam To The type to copy const/volatile to.
 */
template <typename From, typename To>
struct copy_cv : std::type_identity<copy_const_t<From, copy_volatile_t<From, To>>>
{
};

/**
 * @brief Copies top-level const or volatile qualifiers from the type From to the type To.
 *
 * Top-level const and volatile qualifiers for the type To are completely overwritten to
 * match the type From.
 *
 * @tparam From The type to check for const/volatile.
 * @tparam To The type to copy const/volatile to.
 */
template <typename From, typename To>
using copy_cv_t = copy_cv<From, To>::type;

// --------------------------------------------------------------------------------
// copy_reference
// --------------------------------------------------------------------------------

/**
 * @brief Copies reference qualifiers from the type From to the type To.
 *
 * References for the type To are completely overwritten to match the type From.
 *
 * @tparam From The type to check for references.
 * @tparam To The type to copy references to.
 */
template <typename From, typename To>
struct copy_reference : apply_reference<From, std::remove_reference_t<To>>
{
};

/**
 * @brief Copies reference qualifiers from the type From to the type To.
 *
 * References for the type To are completely overwritten to match the type From.
 *
 * @tparam From The type to check for references.
 * @tparam To The type to copy references to.
 */
template <typename From, typename To>
using copy_reference_t = copy_reference<From, To>::type;

// --------------------------------------------------------------------------------
// copy_cvref_t
//--------------------------------------------------------------------------------

/**
 * @brief Copies const, volatile, and reference qualifiers from the type From to the type
 * To.
 *
 * Const, volatile, and references qualifiers for the type To are completely overwritten
 * to match the type From.
 *
 * @tparam From The type to check for const, volatile, and references qualifiers.
 * @tparam To The type to copy const, volatile, and references qualifiers to.
 */
template <typename From, typename To>
struct copy_cvref : std::type_identity<copy_reference_t<
                       From,
                       copy_cv_t<std::remove_reference_t<From>, std::remove_reference_t<To>>>>
{
};

/**
 * @brief Copies const, volatile, and reference qualifiers from the type From to the type
 * To.
 *
 * Const, volatile, and references qualifiers for the type To are completely overwritten
 * to match the type From.
 *
 * @tparam From The type to check for const, volatile, and references qualifiers.
 * @tparam To The type to copy const, volatile, and references qualifiers to.
 */
template <typename From, typename To>
using copy_cvref_t = copy_cvref<From, To>::type;

} // namespace cxxlab
