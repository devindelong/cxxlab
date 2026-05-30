/**
 * @copyright Copyright (c) 2026, Devin DeLong. All rights reserved.
 *
 * @license This code is distributed under the BSD 3-Clause License.
 *          See the LICENSE file for the full text of the license.
 *
 * @author Devin DeLong
 */

#include "cxxlab/pipes/take.hpp"

#include "catch2/catch_test_macros.hpp"

#include <algorithm>
#include <iterator>
#include <ranges>
#include <vector>

TEST_CASE("pipes::take - satisfies output iterator concept", "[pipes][take]")
{
   auto result = std::vector<int>{};
   auto iter = cxxlab::pipes::take(result.begin(), 2);

   STATIC_CHECK(std::output_iterator<decltype(iter), int>);
}

TEST_CASE("pipes::take - manual iterator works", "[pipes][take]")
{
   SECTION("postfix increment")
   {
      auto result = std::vector<int>(2);
      auto iter = cxxlab::pipes::take(result.begin(), 2);
      auto expected = {1, 2};

      CHECK(iter.count() == 2);
      CHECK(iter.taken() == 0);

      *iter++ = 1;
      CHECK(iter.taken() == 1);
      *iter++ = 2;
      CHECK(iter.taken() == 2);
      *iter++ = 3;
      CHECK(iter.taken() == 2);
      *iter++ = 4;
      CHECK(iter.taken() == 2);

      CHECK(std::ranges::equal(result, expected));
   }

   SECTION("prefix increment")
   {
      auto result = std::vector<int>(2);
      auto iter = cxxlab::pipes::take(result.begin(), 2);
      auto expected = {1, 2};

      CHECK(iter.count() == 2);
      CHECK(iter.taken() == 0);

      *iter = 1;
      ++iter;
      CHECK(iter.taken() == 1);
      *iter = 2;
      ++iter;
      CHECK(iter.taken() == 2);
      *iter = 3;
      ++iter;
      CHECK(iter.taken() == 2);
      *iter = 4;
      ++iter;
      CHECK(iter.taken() == 2);

      CHECK(std::ranges::equal(result, expected));
   }
}

TEST_CASE("pipes::take - writable with ranges::copy", "[pipes][take]")
{
   auto input = std::views::iota(0, 10);
   auto result = std::vector<int>{};
   result.reserve(3);
   auto expected = {0, 1, 2};

   std::ranges::copy(input, cxxlab::pipes::take(3) | std::back_inserter(result));
   CHECK(std::ranges::equal(result, expected));
}
