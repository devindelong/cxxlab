/**
 * @copyright Copyright (c) 2026, Devin DeLong. All rights reserved.
 *
 * @license This code is distributed under the BSD 3-Clause License.
 *          See the LICENSE file for the full text of the license.
 *
 * @author Devin DeLong
 */

#include "cxxlab/pipes/stride.hpp"

#include "catch2/catch_test_macros.hpp"

#include <algorithm>
#include <iterator>
#include <ranges>
#include <vector>

TEST_CASE("pipes::stride - satisfies output iterator concept", "[pipes][stride]")
{
   auto result = std::vector<int>{};
   auto stride_iter = cxxlab::pipes::stride(result.begin(), 2);

   STATIC_CHECK(std::output_iterator<decltype(stride_iter), int>);
}

TEST_CASE("pipes::stride - manual iterator works", "[pipes][stride]")
{
   SECTION("postfix increment")
   {
      auto result = std::vector<int>(2);
      auto stride_iter = cxxlab::pipes::stride(result.begin(), 2);
      auto expected = {1, 3};

      CHECK(stride_iter.stride() == 2);

      *stride_iter++ = 1;
      *stride_iter++ = 2;
      *stride_iter++ = 3;
      *stride_iter++ = 4;

      CHECK(std::ranges::equal(result, expected));
   }

   SECTION("prefix increment")
   {
      auto result = std::vector<int>(2);
      auto stride_iter = cxxlab::pipes::stride(result.begin(), 2);
      auto expected = {1, 3};

      CHECK(stride_iter.stride() == 2);

      *stride_iter = 1;
      ++stride_iter;
      *stride_iter = 2;
      ++stride_iter;
      *stride_iter = 3;
      ++stride_iter;
      *stride_iter = 4;
      ++stride_iter;

      CHECK(std::ranges::equal(result, expected));
   }
}

TEST_CASE("pipes::stride - writable with ranges::copy", "[pipes][stride]")
{
   auto input = std::views::iota(0, 10);
   auto result = std::vector<int>{};
   result.reserve(4);
   auto expected = {0, 3, 6, 9};

   std::ranges::copy(input, cxxlab::pipes::stride(3) | std::back_inserter(result));
   CHECK(std::ranges::equal(result, expected));
}
