/**
 * @copyright Copyright (c) 2026, Devin DeLong. All rights reserved.
 *
 * @license This code is distributed under the BSD 3-Clause License.
 *          See the LICENSE file for the full text of the license.
 *
 * @author Devin DeLong
 */

#include "cxxlab/pipes/drop.hpp"

#include "catch2/catch_test_macros.hpp"

#include <algorithm>
#include <iterator>
#include <ranges>
#include <vector>

TEST_CASE("pipes::drop - satisfies output iterator concept", "[pipes][drop]")
{
   auto result = std::vector<int>{};
   auto drop_iter = cxxlab::pipes::drop(result.begin(), 2);

   STATIC_CHECK(std::output_iterator<decltype(drop_iter), int>);
}

TEST_CASE("pipes::drop - manual iterator works", "[pipes][drop]")
{
   SECTION("postfix increment")
   {
      auto result = std::vector<int>(2);
      auto drop_iter = cxxlab::pipes::drop(result.begin(), 2);
      auto expected = {3, 4};

      CHECK(drop_iter.count() == 2);
      CHECK(drop_iter.dropped() == 0);

      *drop_iter++ = 1;
      CHECK(drop_iter.dropped() == 1);
      *drop_iter++ = 2;
      CHECK(drop_iter.dropped() == 2);
      *drop_iter++ = 3;
      CHECK(drop_iter.dropped() == 2);
      *drop_iter++ = 4;
      CHECK(drop_iter.dropped() == 2);

      CHECK(std::ranges::equal(result, expected));
   }

   SECTION("prefix increment")
   {
      auto result = std::vector<int>(2);
      auto drop_iter = cxxlab::pipes::drop(result.begin(), 2);
      auto expected = {3, 4};

      CHECK(drop_iter.count() == 2);
      CHECK(drop_iter.dropped() == 0);

      *drop_iter = 1;
      ++drop_iter;
      CHECK(drop_iter.dropped() == 1);
      *drop_iter = 2;
      ++drop_iter;
      CHECK(drop_iter.dropped() == 2);
      *drop_iter = 3;
      ++drop_iter;
      CHECK(drop_iter.dropped() == 2);
      *drop_iter = 4;
      ++drop_iter;
      CHECK(drop_iter.dropped() == 2);

      CHECK(std::ranges::equal(result, expected));
   }
}

TEST_CASE("pipes::drop - writable with ranges::copy", "[pipes][drop]")
{
   auto input = std::views::iota(0, 10);
   auto result = std::vector<int>{};
   result.reserve(7);
   auto expected = {3, 4, 5, 6, 7, 8, 9};

   std::ranges::copy(input, cxxlab::pipes::drop(3) | std::back_inserter(result));
   CHECK(std::ranges::equal(result, expected));
}
