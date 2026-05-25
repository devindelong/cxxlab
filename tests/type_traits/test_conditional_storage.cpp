/**
 * @file test_maybe_present.cpp
 * @copyright Copyright (c) 2025, Devin DeLong. All rights reserved.
 *
 * @license This code is distributed under the BSD 3-Clause License.
 *          See the LICENSE file for the full text of the license.
 *
 * @author Devin DeLong
 */

#include "cxxlab/type_traits/conditional_storage.hpp"

#include "catch2/catch_test_macros.hpp"

#include <tuple>
#include <type_traits>
#include <vector>

namespace
{
// Each empty type should be unique and allow them to overlap memory when using
// [[no_unique_acxxlabress]].
template <bool present>
struct conditional_storage_pair
{
   [[no_unique_address]] cxxlab::conditional_storage_t<present, int> int_one;
   [[no_unique_address]] cxxlab::conditional_storage_t<present, int> int_two;
};
} // namespace

TEST_CASE(
   "conditional_storage_t: check conditional sizeof", "[type_traits][conditional_storage][sizeof]")
{
   REQUIRE(sizeof(cxxlab::conditional_storage_t<true, int>) == sizeof(int));
   REQUIRE(std::is_empty_v<cxxlab::conditional_storage_t<false, int>>);
}

TEST_CASE("conditional_storage_t: empty type uniqueness", "[sizeof]")
{
   REQUIRE(std::is_empty_v<conditional_storage_pair<false>>);
   REQUIRE(sizeof(conditional_storage_pair<true>) == 2 * sizeof(int));
}

TEST_CASE(
   "conditional_storage_t: initializer list construction",
   "[type_traits][conditional_storage][constructor]")
{
   using container = std::vector<int>;

   REQUIRE(
      std::constructible_from<
         cxxlab::conditional_storage_t<true, container>, std::initializer_list<int>>);
   REQUIRE(
      std::constructible_from<
         cxxlab::conditional_storage_t<false, container>, std::initializer_list<int>>);
}

TEST_CASE(
   "conditional_storage_t: variadic constructor", "[type_traits][conditional_storage][constructor]")
{
   using container = std::tuple<int, int, int>;

   REQUIRE(std::constructible_from<cxxlab::conditional_storage_t<true, container>, int, int, int>);
   REQUIRE(std::constructible_from<cxxlab::conditional_storage_t<false, container>, int, int, int>);
}
