/**
 * @copyright Copyright (c) 2026, Devin DeLong. All rights reserved.
 *
 * @license This code is distributed under the BSD 3-Clause License.
 *          See the LICENSE file for the full text of the license.
 *
 * @author Devin DeLong
 */

#include "cxxlab/pipes/drop_while.hpp"

#include "catch2/catch_test_macros.hpp"

#include <algorithm>
#include <iterator>
#include <ranges>
#include <vector>

namespace
{
constexpr auto less_than_5 = [](int x) { return x < 5; };
} // namespace

TEST_CASE("pipes::drop_while - satisfies output iterator concept", "[pipes][drop_while]")
{
   auto result = std::vector<int>{};
   auto drop_while_iter = cxxlab::pipes::drop_while(result.begin(), less_than_5);

   STATIC_CHECK(std::output_iterator<decltype(drop_while_iter), int>);
}

TEST_CASE("pipes::drop_while - manual iterator works", "[pipes][drop_while]")
{
   SECTION("postfix increment")
   {
      auto result = std::vector<int>(5);
      auto drop_while_iter = cxxlab::pipes::drop_while(result.begin(), less_than_5);
      auto expected = {5, 6, 7, 8, 9};

      for (auto i : std::views::iota(0, 10))
      {
         *drop_while_iter++ = i;
      }

      CHECK(std::ranges::equal(result, expected));
   }

   SECTION("prefix increment")
   {
      auto result = std::vector<int>(5);
      auto drop_while_iter = cxxlab::pipes::drop_while(result.begin(), less_than_5);
      auto expected = {5, 6, 7, 8, 9};

      for (auto i : std::views::iota(0, 10))
      {
         *drop_while_iter = i;
         ++drop_while_iter;
      }

      CHECK(std::ranges::equal(result, expected));
   }
}

TEST_CASE("pipes::drop_while - writable with ranges::copy", "[pipes][drop_while]")
{
   auto input = std::views::iota(0, 10);
   auto result = std::vector<int>{};
   result.reserve(5);
   auto expected = {5, 6, 7, 8, 9};

   std::ranges::copy(input, cxxlab::pipes::drop_while(less_than_5) | std::back_inserter(result));
   CHECK(std::ranges::equal(result, expected));
}
