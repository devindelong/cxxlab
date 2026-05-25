/**
 * @copyright Copyright (c) 2026, Devin DeLong. All rights reserved.
 *
 * @license This code is distributed under the BSD 3-Clause License.
 *          See the LICENSE file for the full text of the license.
 *
 * @author Devin DeLong
 */

#include "cxxlab/pipes.hpp"

#include <algorithm>
#include <iterator>
#include <print>
#include <ranges>
#include <vector>

int main()
{
   auto evens = [](auto const& x) { return x % 2 == 0; };
   auto square = [](auto const& x) { return x * x; };
   {
      auto input = std::views::iota(0, 100);
      auto result = std::vector<int>{};
      std::ranges::copy(
         input, cxxlab::pipes::stride(3) | cxxlab::pipes::transform(square) | cxxlab::pipes::drop(2) |
                   cxxlab::pipes::take(20) | cxxlab::pipes::filter(evens) | std::back_inserter(result));
      std::print("RESULT : ");
      for (const auto elem : result)
      {
         std::print("{} ", elem);
      }
   }

   std::println();

   {
      auto less_than_50 = [](auto const& x) { return x < 50; };
      auto input = std::views::iota(0, 100);
      auto result = std::vector<int>{};
      std::ranges::copy(
         input,
         cxxlab::pipes::take_while(less_than_50) | cxxlab::pipes::stride(3) | std::back_inserter(result));
      std::print("RESULT : ");
      for (const auto elem : result)
      {
         std::print("{} ", elem);
      }
   }

   std::println();

   // func(cxxlab::pipes::filter_output_iterator{std::back_inserter(result), evens}, 1);

   return 0;
}
