/**
 * @copyright Copyright (c) 2026, Devin DeLong. All rights reserved.
 *
 * @license This code is distributed under the BSD 3-Clause License.
 *          See the LICENSE file for the full text of the license.
 *
 * @author Devin DeLong
 */

#include "cxxlab/type_traits/apply_cvref.hpp"

#include "catch2/catch_test_macros.hpp"

#include <concepts>

// const

TEST_CASE("type_traits: apply_const", "[type_traits][apply_const]")
{
   REQUIRE(std::same_as<cxxlab::apply_const_t<int, int>, int>);
   REQUIRE(std::same_as<cxxlab::apply_const_t<const int, int>, const int>);
   REQUIRE(std::same_as<cxxlab::apply_const_t<int, const int>, const int>);
}

// volatile

TEST_CASE("type_traits: apply_volatile", "[type_traits][apply_volatile]")
{
   REQUIRE(std::same_as<cxxlab::apply_volatile_t<int, int>, int>);
   REQUIRE(std::same_as<cxxlab::apply_volatile_t<volatile int, int>, volatile int>);
   REQUIRE(std::same_as<cxxlab::apply_volatile_t<int, volatile int>, volatile int>);
}

// cv

TEST_CASE("type_traits: apply_cv", "[type_traits][apply_cv]")
{
   REQUIRE(std::same_as<cxxlab::apply_cv_t<int, int>, int>);
   // const only
   REQUIRE(std::same_as<cxxlab::apply_cv_t<const int, int>, const int>);
   REQUIRE(std::same_as<cxxlab::apply_cv_t<int, const int>, const int>);
   // volatile only
   REQUIRE(std::same_as<cxxlab::apply_cv_t<volatile int, int>, volatile int>);
   REQUIRE(std::same_as<cxxlab::apply_cv_t<int, volatile int>, volatile int>);
   // const and volatile
   REQUIRE(std::same_as<cxxlab::apply_cv_t<volatile const int, int>, volatile const int>);
   REQUIRE(std::same_as<cxxlab::apply_cv_t<int, volatile const int>, volatile const int>);
   REQUIRE(std::same_as<cxxlab::apply_cv_t<const int, volatile int>, volatile const int>);
   REQUIRE(std::same_as<cxxlab::apply_cv_t<volatile int, const int>, volatile const int>);
}

// reference

TEST_CASE("type_traits: apply_reference", "[type_traits][apply_reference]")
{
   REQUIRE(std::same_as<cxxlab::apply_reference_t<int, int>, int>);
   REQUIRE(std::same_as<cxxlab::apply_reference_t<int&, int>, int&>);
   REQUIRE(std::same_as<cxxlab::apply_reference_t<int&&, int>, int&&>);
   REQUIRE(std::same_as<cxxlab::apply_reference_t<int, int&>, int&>);
   REQUIRE(std::same_as<cxxlab::apply_reference_t<int, int&&>, int&&>);
}
