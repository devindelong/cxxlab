/**
 * @copyright Copyright (c) 2026, Devin DeLong. All rights reserved.
 *
 * @license This code is distributed under the BSD 3-Clause License.
 *          See the LICENSE file for the full text of the license.
 *
 * @author Devin DeLong
 */

#include "cxxlab/type_traits/copy_cvref.hpp"

#include "catch2/catch_test_macros.hpp"

#include <concepts>

// const

TEST_CASE("type_traits - copy_const", "[type_traits][copy_const]")
{
   STATIC_CHECK(std::same_as<cxxlab::copy_const_t<int, int>, int>);
   STATIC_CHECK(std::same_as<cxxlab::copy_const_t<const int, int>, const int>);
   STATIC_CHECK(std::same_as<cxxlab::copy_const_t<int, const int>, int>);
}

// volatile

TEST_CASE("type_traits - copy_volatile", "[type_traits][copy_volatile]")
{
   STATIC_CHECK(std::same_as<cxxlab::copy_volatile_t<int, int>, int>);
   STATIC_CHECK(std::same_as<cxxlab::copy_volatile_t<volatile int, int>, volatile int>);
   STATIC_CHECK(std::same_as<cxxlab::copy_volatile_t<int, volatile int>, int>);
}

// cv

TEST_CASE("type_traits - copy_cv", "[type_traits][copy_cv]")
{
   STATIC_CHECK(std::same_as<cxxlab::copy_cv_t<int, int>, int>);
   // const only
   STATIC_CHECK(std::same_as<cxxlab::copy_cv_t<const int, int>, const int>);
   STATIC_CHECK(std::same_as<cxxlab::copy_cv_t<int, const int>, int>);
   // volatile only
   STATIC_CHECK(std::same_as<cxxlab::copy_cv_t<volatile int, int>, volatile int>);
   STATIC_CHECK(std::same_as<cxxlab::copy_cv_t<int, volatile int>, int>);
   // const and volatile
   STATIC_CHECK(std::same_as<cxxlab::copy_cv_t<volatile const int, int>, volatile const int>);
   STATIC_CHECK(std::same_as<cxxlab::copy_cv_t<int, volatile const int>, int>);
   STATIC_CHECK(std::same_as<cxxlab::copy_cv_t<const int, volatile int>, const int>);
   STATIC_CHECK(std::same_as<cxxlab::copy_cv_t<volatile int, const int>, volatile int>);
}

// reference

TEST_CASE("type_traits - copy_reference", "[type_traits][copy_reference]")
{
   STATIC_CHECK(std::same_as<cxxlab::copy_reference_t<int, int>, int>);
   STATIC_CHECK(std::same_as<cxxlab::copy_reference_t<int&, int>, int&>);
   STATIC_CHECK(std::same_as<cxxlab::copy_reference_t<int&&, int>, int&&>);
   STATIC_CHECK(std::same_as<cxxlab::copy_reference_t<int, int&>, int>);
   STATIC_CHECK(std::same_as<cxxlab::copy_reference_t<int, int&&>, int>);
}

// cvref

TEST_CASE("type_traits - copy_cvref", "[type_traits][copy_cvref]")
{
   STATIC_CHECK(std::same_as<cxxlab::copy_cvref_t<int, int>, int>);
   // const only
   STATIC_CHECK(std::same_as<cxxlab::copy_cvref_t<const int, int>, const int>);
   STATIC_CHECK(std::same_as<cxxlab::copy_cvref_t<int, const int>, int>);
   // volatile only
   STATIC_CHECK(std::same_as<cxxlab::copy_cvref_t<volatile int, int>, volatile int>);
   STATIC_CHECK(std::same_as<cxxlab::copy_cvref_t<int, volatile int>, int>);
   // reference only
   STATIC_CHECK(std::same_as<cxxlab::copy_cvref_t<int&, int>, int&>);
   STATIC_CHECK(std::same_as<cxxlab::copy_cvref_t<int&&, int>, int&&>);
   STATIC_CHECK(std::same_as<cxxlab::copy_cvref_t<int, int&>, int>);
   STATIC_CHECK(std::same_as<cxxlab::copy_cvref_t<int, int&&>, int>);
   // const and volatile
   STATIC_CHECK(std::same_as<cxxlab::copy_cvref_t<volatile const int, int>, volatile const int>);
   STATIC_CHECK(std::same_as<cxxlab::copy_cvref_t<int, volatile const int>, int>);
   STATIC_CHECK(std::same_as<cxxlab::copy_cvref_t<const int, volatile int>, const int>);
   STATIC_CHECK(std::same_as<cxxlab::copy_cvref_t<volatile int, const int>, volatile int>);
   // const, volatile, and reference
   STATIC_CHECK(std::same_as<cxxlab::copy_cvref_t<volatile const int&, int>, volatile const int&>);
   STATIC_CHECK(
      std::same_as<cxxlab::copy_cvref_t<volatile const int&&, int>, volatile const int&&>);
   STATIC_CHECK(std::same_as<cxxlab::copy_cvref_t<int, volatile const int&>, int>);
   STATIC_CHECK(std::same_as<cxxlab::copy_cvref_t<int, volatile const int&&>, int>);
   STATIC_CHECK(std::same_as<cxxlab::copy_cvref_t<const int&, volatile int>, const int&>);
   STATIC_CHECK(std::same_as<cxxlab::copy_cvref_t<const int&&, volatile int>, const int&&>);
   STATIC_CHECK(std::same_as<cxxlab::copy_cvref_t<volatile int&, const int>, volatile int&>);
   STATIC_CHECK(std::same_as<cxxlab::copy_cvref_t<volatile int&&, const int>, volatile int&&>);
}
