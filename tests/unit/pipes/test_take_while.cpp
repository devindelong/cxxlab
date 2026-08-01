/**
 * @copyright Copyright (c) 2026, Devin DeLong. All rights reserved.
 *
 * @license This code is distributed under the BSD 3-Clause License.
 *          See the LICENSE file for the full text of the license.
 *
 * @author Devin DeLong
 */

#include "cxxlab/pipes/take_while.hpp"

#include "catch2/catch_test_macros.hpp"

#include <algorithm>
#include <iterator>
#include <ranges>
#include <vector>

namespace
{
constexpr auto less_than_5 = [](int x) { return x < 5; };
} // namespace

TEST_CASE("pipes::take_while - satisfies output iterator concept", "[pipes][take_while]")
{
   auto result = std::vector<int>{};
   auto take_while_iter = cxxlab::pipes::take_while(result.begin(), less_than_5);

   STATIC_CHECK(std::output_iterator<decltype(take_while_iter), int>);
}

TEST_CASE("pipes::take_while - manual iterator works", "[pipes][take_while]")
{
   SECTION("postfix increment")
   {
      auto result = std::vector<int>(5);
      auto take_while_iter = cxxlab::pipes::take_while(result.begin(), less_than_5);
      auto expected = {0, 1, 2, 3, 4};

      for (auto i : std::views::iota(0, 10))
      {
         *take_while_iter++ = i;
      }

      CHECK(std::ranges::equal(result, expected));
   }

   SECTION("prefix increment")
   {
      auto result = std::vector<int>(5);
      auto take_while_iter = cxxlab::pipes::take_while(result.begin(), less_than_5);
      auto expected = {0, 1, 2, 3, 4};

      for (auto i : std::views::iota(0, 10))
      {
         *take_while_iter = i;
         ++take_while_iter;
      }

      CHECK(std::ranges::equal(result, expected));
   }
}

TEST_CASE("pipes::take_while - writable with ranges::copy", "[pipes][take_while]")
{
   auto input = std::views::iota(0, 10);
   auto result = std::vector<int>{};
   result.reserve(5);
   auto expected = {0, 1, 2, 3, 4};

   std::ranges::copy(input, cxxlab::pipes::take_while(less_than_5) | std::back_inserter(result));
   CHECK(std::ranges::equal(result, expected));
}
