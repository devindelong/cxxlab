/**
 * @copyright Copyright (c) 2026, Devin DeLong. All rights reserved.
 *
 * @license This code is distributed under the BSD 3-Clause License.
 *          See the LICENSE file for the full text of the license.
 *
 * @author Devin DeLong
 */

#include "cxxlab/memory/aligned_wrapper.hpp"

#include "catch2/catch_test_macros.hpp"

#include <new>

TEST_CASE("aligned_wrapper: alignment is correct", "[aligned_wrapper][alignof]")
{
   STATIC_CHECK(alignof(cxxlab::aligned_wrapper<int, 64>) == 64);
   STATIC_CHECK(
      alignof(cxxlab::cache_aligned_wrapper<int>) == std::hardware_destructive_interference_size);
}
