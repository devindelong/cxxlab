/**
 * @copyright Copyright (c) 2026, Devin DeLong. All rights reserved.
 *
 * @license This code is distributed under the BSD 3-Clause License.
 *          See the LICENSE file for the full text of the license.
 *
 * @author Devin DeLong
 */

#pragma once

#include "cxxlab/pipes/iterator_interface.hpp"

namespace cxxlab::pipes
{

struct null_iterator : output_iterator_types<void>
{
   /**
    * @brief No-op.
    * @return *this
    */
   constexpr auto operator*() noexcept -> null_iterator& { return *this; }

   /**
    * @brief No-op.
    * @return *this
    */
   constexpr auto operator++() noexcept -> null_iterator& { return *this; }

   /**
    * @brief No-op.
    * @return *this
    */
   constexpr auto operator++(int) noexcept -> null_iterator { return *this; }

   /**
    * @brief No-op.
    * @return *this
    */
   constexpr auto operator=(auto&&) noexcept -> null_iterator& { return *this; }
};

inline constinit auto null = null_iterator{};
} // namespace cxxlab::pipes
