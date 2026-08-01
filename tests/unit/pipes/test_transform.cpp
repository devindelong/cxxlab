/**
 * @copyright Copyright (c) 2026, Devin DeLong. All rights reserved.
 *
 * @license This code is distributed under the BSD 3-Clause License.
 *          See the LICENSE file for the full text of the license.
 *
 * @author Devin DeLong
 */

#include "cxxlab/pipes/transform.hpp"

#include "catch2/catch_test_macros.hpp"

#include <algorithm>
#include <iterator>
#include <ranges>
#include <vector>

namespace
{
constexpr auto square = [](auto x) { return x * x; };
} // namespace

TEST_CASE("pipes::transform - satisfies output iterator concept", "[pipes][transform]")
{
   auto result = std::vector<int>{};
   auto transform_iter = cxxlab::pipes::transform(result.begin(), square);

   STATIC_CHECK(std::output_iterator<decltype(transform_iter), int>);
}

TEST_CASE("pipes::transform - manual iterator works", "[pipes][transform]")
{
   SECTION("postfix increment")
   {
      auto result = std::vector<int>(4);
      auto transform_iter = cxxlab::pipes::transform(result.begin(), square);
      auto expected = {1, 4, 9, 16};

      for (auto i : std::views::iota(1, 5))
      {
         *transform_iter++ = i;
      }

      CHECK(std::ranges::equal(result, expected));
   }

   SECTION("prefix increment")
   {
      auto result = std::vector<int>(4);
      auto transform_iter = cxxlab::pipes::transform(result.begin(), square);
      auto expected = {1, 4, 9, 16};

      for (auto i : std::views::iota(1, 5))
      {
         *transform_iter = i;
         ++transform_iter;
      }

      CHECK(std::ranges::equal(result, expected));
   }
}

TEST_CASE("pipes::transform - writable with ranges::copy", "[pipes][transform]")
{
   auto input = std::views::iota(0, 10);
   auto result = std::vector<int>{};
   auto expected = {0, 1, 4, 9, 16, 25, 36, 49, 64, 81};

   std::ranges::copy(input, cxxlab::pipes::transform(square) | std::back_inserter(result));
   CHECK(std::ranges::equal(result, expected));
}
