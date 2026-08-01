/**
 * @copyright Copyright (c) 2026, Devin DeLong. All rights reserved.
 *
 * @license This code is distributed under the BSD 3-Clause License.
 *          See the LICENSE file for the full text of the license.
 *
 * @author Devin DeLong
 */

#include "cxxlab/pipes/null.hpp"

#include "catch2/catch_test_macros.hpp"

#include <algorithm>
#include <iterator>
#include <ranges>

TEST_CASE("pipes::null - satisfies output iterator concept", "[pipes][null]")
{
   STATIC_CHECK(std::output_iterator<decltype(cxxlab::pipes::null), int>);
}

TEST_CASE("pipes:null - copy to null iterator compiles", "[pipes][null]")
{
   auto input = std::views::iota(0, 10);
   std::ranges::copy(input, cxxlab::pipes::null);
   SUCCEED();
}
