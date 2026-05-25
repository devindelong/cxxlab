/**
 * @copyright Copyright (c) 2026, Devin DeLong. All rights reserved.
 *
 * @license This code is distributed under the BSD 3-Clause License.
 *          See the LICENSE file for the full text of the license.
 *
 * @author Devin DeLong
 */

#include "cxxlab/type_traits/like.hpp"

#include "catch2/catch_test_macros.hpp"

#include <concepts>

TEST_CASE("type_traits: like", "[type_traits][like]")
{
   REQUIRE(std::same_as<cxxlab::like_t<int, int>, int>);
   // const only
   REQUIRE(std::same_as<cxxlab::like_t<const int, int>, const int>);
   REQUIRE(std::same_as<cxxlab::like_t<int, const int>, int>);
   // volatile only
   REQUIRE(std::same_as<cxxlab::like_t<volatile int, int>, volatile int>);
   REQUIRE(std::same_as<cxxlab::like_t<int, volatile int>, int>);
   // reference only
   REQUIRE(std::same_as<cxxlab::like_t<int&, int>, int&>);
   REQUIRE(std::same_as<cxxlab::like_t<int&&, int>, int&&>);
   REQUIRE(std::same_as<cxxlab::like_t<int, int&>, int>);
   REQUIRE(std::same_as<cxxlab::like_t<int, int&&>, int>);
   // const and volatile
   REQUIRE(std::same_as<cxxlab::like_t<volatile const int, int>, volatile const int>);
   REQUIRE(std::same_as<cxxlab::like_t<int, volatile const int>, int>);
   REQUIRE(std::same_as<cxxlab::like_t<const int, volatile int>, const int>);
   REQUIRE(std::same_as<cxxlab::like_t<volatile int, const int>, volatile int>);
   // const, volatile, and reference
   REQUIRE(std::same_as<cxxlab::like_t<volatile const int&, int>, volatile const int&>);
   REQUIRE(std::same_as<cxxlab::like_t<volatile const int&&, int>, volatile const int&&>);
   REQUIRE(std::same_as<cxxlab::like_t<int, volatile const int&>, int>);
   REQUIRE(std::same_as<cxxlab::like_t<int, volatile const int&&>, int>);
   REQUIRE(std::same_as<cxxlab::like_t<const int&, volatile int>, const int&>);
   REQUIRE(std::same_as<cxxlab::like_t<const int&&, volatile int>, const int&&>);
   REQUIRE(std::same_as<cxxlab::like_t<volatile int&, const int>, volatile int&>);
   REQUIRE(std::same_as<cxxlab::like_t<volatile int&&, const int>, volatile int&&>);
}
