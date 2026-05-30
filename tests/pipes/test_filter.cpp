/**
 * @copyright Copyright (c) 2026, Devin DeLong. All rights reserved.
 *
 * @license This code is distributed under the BSD 3-Clause License.
 *          See the LICENSE file for the full text of the license.
 *
 * @author Devin DeLong
 */

#include "cxxlab/pipes/filter.hpp"

#include "catch2/catch_test_macros.hpp"

#include <algorithm>
#include <iterator>
#include <ranges>
#include <vector>

namespace
{
constexpr auto evens = [](auto x) { return x % 2 == 0; };
} // namespace

TEST_CASE("pipes::filter - satisfies output iterator concept", "[pipes][filter]")
{
   auto result = std::vector<int>{};
   auto filter_iter = cxxlab::pipes::filter(result.begin(), evens);

   STATIC_CHECK(std::output_iterator<decltype(filter_iter), int>);
}

TEST_CASE("pipes::filter - manual iterator works", "[pipes][filter]")
{
   SECTION("postfix increment")
   {
      auto result = std::vector<int>(2);
      auto filter_iter = cxxlab::pipes::filter(result.begin(), evens);
      auto expected = {2, 4};

      *filter_iter++ = 1;
      *filter_iter++ = 2;
      *filter_iter++ = 3;
      *filter_iter++ = 4;

      CHECK(std::ranges::equal(result, expected));
   }

   SECTION("prefix increment")
   {
      auto result = std::vector<int>(2);
      auto filter_iter = cxxlab::pipes::filter(result.begin(), evens);
      auto expected = {2, 4};

      *filter_iter = 1;
      ++filter_iter;
      *filter_iter = 2;
      ++filter_iter;
      *filter_iter = 3;
      ++filter_iter;
      *filter_iter = 4;
      ++filter_iter;

      CHECK(std::ranges::equal(result, expected));
   }
}

TEST_CASE("pipes::filter - writable with ranges::copy", "[pipes][filter]")
{
   auto input = std::views::iota(0, 10);
   auto result = std::vector<int>{};
   auto expected = {0, 2, 4, 6, 8};

   std::ranges::copy(input, cxxlab::pipes::filter(evens) | std::back_inserter(result));
   CHECK(std::ranges::equal(result, expected));
}
