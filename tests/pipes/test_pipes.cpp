/**
 * @copyright Copyright (c) 2026, Devin DeLong. All rights reserved.
 *
 * @license This code is distributed under the BSD 3-Clause License.
 *          See the LICENSE file for the full text of the license.
 *
 * @author Devin DeLong
 */

#include "cxxlab/pipes.hpp"

#include "catch2/catch_test_macros.hpp"

#include <algorithm>
#include <iterator>
#include <ranges>
#include <vector>

TEST_CASE("pipes: copy to filter adaptor", "[pipes][adaptor]")
{
   auto evens = [](auto const& x) { return x % 2 == 0; };
   auto input = std::views::iota(0, 10);
   auto result = std::vector<int>{};
   auto expected = {0, 2, 4, 6, 8};

   std::ranges::copy(input, cxxlab::pipes::filter(evens) | std::back_inserter(result));
   CHECK(std::ranges::equal(result, expected));
}

TEST_CASE("pipes: copy to transform adaptor", "[pipes][adaptor]")
{
   auto square = [](auto const& x) { return x * x; };
   auto input = std::views::iota(0, 10);
   auto result = std::vector<int>{};
   auto expected = {0, 1, 4, 9, 16, 25, 36, 49, 64, 81};

   std::ranges::copy(input, cxxlab::pipes::transform(square) | std::back_inserter(result));
   CHECK(std::ranges::equal(result, expected));
}

TEST_CASE("pipes: copy to stride adaptor", "[pipes][adaptor]")
{
   auto input = std::views::iota(0, 10);
   auto result = std::vector<int>{};
   auto expected = {0, 3, 6, 9};

   std::ranges::copy(input, cxxlab::pipes::stride(3) | std::back_inserter(result));
   CHECK(std::ranges::equal(result, expected));
}

TEST_CASE("pipes: copy to drop adaptor", "[pipes][adaptor]")
{
   auto input = std::views::iota(0, 10);
   auto result = std::vector<int>{};
   auto expected = {3, 4, 5, 6, 7, 8, 9};

   std::ranges::copy(input, cxxlab::pipes::drop(3) | std::back_inserter(result));
   CHECK(std::ranges::equal(result, expected));
}

TEST_CASE("pipes: copy to take adaptor", "[pipes][adaptor]")
{
   auto input = std::views::iota(0, 10);
   auto result = std::vector<int>{};
   auto expected = {0, 1, 2};

   std::ranges::copy(input, cxxlab::pipes::take(3) | std::back_inserter(result));
   CHECK(std::ranges::equal(result, expected));
}

TEST_CASE("pipes: copy to drop_while adaptor", "[pipes][adaptor]")
{
   auto less_than_5 = [](int x) { return x < 5; };
   auto input = std::views::iota(0, 10);
   auto result = std::vector<int>{};
   auto expected = {5, 6, 7, 8, 9};

   std::ranges::copy(input, cxxlab::pipes::drop_while(less_than_5) | std::back_inserter(result));
   CHECK(std::ranges::equal(result, expected));
}

TEST_CASE("pipes: copy to take_while adaptor", "[pipes][adaptor]")
{
   auto less_than_5 = [](int x) { return x < 5; };
   auto input = std::views::iota(0, 10);
   auto result = std::vector<int>{};
   auto expected = {0, 1, 2, 3, 4};

   std::ranges::copy(input, cxxlab::pipes::take_while(less_than_5) | std::back_inserter(result));
   CHECK(std::ranges::equal(result, expected));
}

TEST_CASE("pipes: copy to null iterator compiles", "[pipes][adaptor]")
{
   auto input = std::views::iota(0, 10);
   std::ranges::copy(input, cxxlab::pipes::null);
   SUCCEED();
}

TEST_CASE("pipes: copy to many chained adaptors", "[pipes][adaptor]")
{
   auto evens = [](auto const& x) { return x % 2 == 0; };
   auto square = [](auto const& x) { return x * x; };

   auto input = std::views::iota(0, 100);
   auto result = std::vector<int>{};
   auto expected = {36, 144, 324, 576, 900, 1296, 1764, 2304, 2916, 3600};
   std::ranges::copy(
      input, cxxlab::pipes::stride(3) | cxxlab::pipes::transform(square) | cxxlab::pipes::drop(2) |
                cxxlab::pipes::take(20) | cxxlab::pipes::filter(evens) |
                std::back_inserter(result));
   CHECK(std::ranges::equal(result, expected));
}
