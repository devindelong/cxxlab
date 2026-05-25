/**
 * @copyright Copyright (c) 2026, Devin DeLong. All rights reserved.
 *
 * @license This code is distributed under the BSD 3-Clause License.
 *          See the LICENSE file for the full text of the license.
 *
 * @author Devin DeLong
 */

#include "cxxlab/ranges/slice_view.hpp"

#include "catch2/catch_test_macros.hpp"

#include <algorithm>
#include <array>
#include <ranges>

TEST_CASE("slice_view: basic slicing and size", "[slice_view][constexpr]")
{
   constexpr static auto v = std::array{10, 11, 12, 13, 14};

   // Uses slice_view constructor.
   constexpr auto sv = cxxlab::ranges::slice_view{v, 1, 4};
   constexpr auto expected = std::array{11, 12, 13};

   REQUIRE(std::ranges::equal(sv, expected));
   REQUIRE(sv.size() == expected.size());
}

TEST_CASE("slice_view: range adaptor (cxxlab::views::slice) works", "[slice_view][constexpr]")
{
   constexpr static auto v = std::array{0, 1, 2, 3, 4, 5};

   // Using the range adaptor closure.
   constexpr auto sv = v | cxxlab::views::slice(2, 5);
   constexpr auto expected = std::array{2, 3, 4};

   // Should be equal to {2,3,4}.
   REQUIRE(std::ranges::equal(sv, expected));
   REQUIRE(sv.size() == expected.size());
}

TEST_CASE(
   "slice_view: mutation through non-const view updates underlying container",
   "[slice_view][mutation]")
{
   auto v = std::array{1, 2, 3, 4, 5};
   auto sv = cxxlab::views::slice(1, 4)(v);
   std::ranges::for_each(sv, [](auto& x) { x += 10; });

   // Underlying container should reflect the changes.
   auto expected = std::array{1, 12, 13, 14, 5};
   REQUIRE(v == expected);
}

TEST_CASE("slice_view: const correctness (view over const range)", "[slice_view][const]")
{
   const auto v = std::array{5, 6, 7, 8};
   auto csv = v | cxxlab::views::slice(1, 3);

   auto expected = std::array{6, 7};
   REQUIRE(std::ranges::equal(csv, expected));
   REQUIRE(csv.size() == expected.size());
}

TEST_CASE(
   "slice_view: base() returns underlying view that equals the original container view",
   "[slice_view][base]")
{
   auto v = std::array{1, 2, 3, 4, 5};
   auto sv = v | cxxlab::views::slice(0, 3);

   // base() returns a view of the underlying sequence; verify it iterates over the same
   // elements as the base container.
   auto base_view = sv.base();
   REQUIRE(std::ranges::equal(base_view, v));
}

TEST_CASE("slice_view: check upper index bound is clamped", "[slice_view][bounds]")
{
   auto v = std::array{10, 11, 12, 13, 14};

   auto sv = cxxlab::ranges::slice_view{v, 3, 10};
   auto expected = std::array{13, 14};

   REQUIRE(std::ranges::equal(sv, expected));
   REQUIRE(sv.size() == expected.size());
}

TEST_CASE("slice_view: check upper and lower index bounds are clamped", "[slice_view][bounds]")
{
   auto v = std::array{10, 11, 12, 13, 14};
   auto sv = cxxlab::ranges::slice_view{v, 10, 10};
   REQUIRE(std::ranges::empty(sv));
}
