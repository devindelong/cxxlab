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

struct Foo
{
};

template <typename Self, typename T, typename Expected>
constexpr bool like_yields = std::same_as<cxxlab::like_t<Self, T>, Expected>;

template <typename Self, typename T, typename Expected>
constexpr bool like_pointer_yields = std::same_as<cxxlab::like_pointer_t<Self, T>, Expected>;

TEST_CASE("like_t - unqualified Self yields unqualified T", "[type_traits][like]")
{
   STATIC_CHECK(like_yields<int, int, int>);
}

TEST_CASE("like_t - const propagates from Self, not from T", "[type_traits][like]")
{
   STATIC_CHECK(like_yields<const int, int, const int>);
   STATIC_CHECK(like_yields<int, const int, int>);
}

TEST_CASE("like_t - volatile propagates from Self, not from T", "[type_traits][like]")
{
   STATIC_CHECK(like_yields<volatile int, int, volatile int>);
   STATIC_CHECK(like_yields<int, volatile int, int>);
}

TEST_CASE("like_t - reference propagates from Self, not from T", "[type_traits][like]")
{
   STATIC_CHECK(like_yields<int&, int, int&>);
   STATIC_CHECK(like_yields<int&&, int, int&&>);
   STATIC_CHECK(like_yields<int, int&, int>);
   STATIC_CHECK(like_yields<int, int&&, int>);
}

TEST_CASE("like_t - const and volatile both propagate from Self", "[type_traits][like]")
{
   STATIC_CHECK(like_yields<const volatile int, int, const volatile int>);
   STATIC_CHECK(like_yields<int, const volatile int, int>);
   STATIC_CHECK(like_yields<const int, volatile int, const int>);
   STATIC_CHECK(like_yields<volatile int, const int, volatile int>);
}

TEST_CASE("like_t - const, volatile, and reference all propagate from Self", "[type_traits][like]")
{
   STATIC_CHECK(like_yields<const volatile int&, int, const volatile int&>);
   STATIC_CHECK(like_yields<const volatile int&&, int, const volatile int&&>);
   STATIC_CHECK(like_yields<int, const volatile int&, int>);
   STATIC_CHECK(like_yields<int, const volatile int&&, int>);
   STATIC_CHECK(like_yields<const int&, volatile int, const int&>);
   STATIC_CHECK(like_yields<const int&&, volatile int, const int&&>);
   STATIC_CHECK(like_yields<volatile int&, const int, volatile int&>);
   STATIC_CHECK(like_yields<volatile int&&, const int, volatile int&&>);
}

TEST_CASE("like_pointer_t - non-const Self yields T*", "[type_traits][like_pointer]")
{
   STATIC_CHECK(like_pointer_yields<Foo, int, int*>);
   STATIC_CHECK(like_pointer_yields<Foo&, int, int*>);
   STATIC_CHECK(like_pointer_yields<Foo&&, int, int*>);
   STATIC_CHECK(like_pointer_yields<Foo, Foo, Foo*>);
   STATIC_CHECK(like_pointer_yields<Foo&, Foo, Foo*>);
   STATIC_CHECK(like_pointer_yields<Foo&&, Foo, Foo*>);
}

TEST_CASE("like_pointer_t - const Self yields const T*", "[type_traits][like_pointer]")
{
   STATIC_CHECK(like_pointer_yields<const Foo, int, const int*>);
   STATIC_CHECK(like_pointer_yields<const Foo&, int, const int*>);
   STATIC_CHECK(like_pointer_yields<const Foo&&, int, const int*>);
   STATIC_CHECK(like_pointer_yields<const Foo, Foo, const Foo*>);
   STATIC_CHECK(like_pointer_yields<const Foo&, Foo, const Foo*>);
   STATIC_CHECK(like_pointer_yields<const Foo&&, Foo, const Foo*>);
}

TEST_CASE("like_pointer_t - volatile-only Self yields T*", "[type_traits][like_pointer]")
{
   STATIC_CHECK(like_pointer_yields<volatile Foo, int, int*>);
   STATIC_CHECK(like_pointer_yields<volatile Foo&, int, int*>);
   STATIC_CHECK(like_pointer_yields<volatile Foo&&, int, int*>);
}

TEST_CASE("like_pointer_t - const volatile Self yields const T*", "[type_traits][like_pointer]")
{
   STATIC_CHECK(like_pointer_yields<const volatile Foo, int, const int*>);
   STATIC_CHECK(like_pointer_yields<const volatile Foo&, int, const int*>);
   STATIC_CHECK(like_pointer_yields<const volatile Foo&&, int, const int*>);
}

TEST_CASE(
   "like_pointer_t - const T with non-const Self yields const T*", "[type_traits][like_pointer]")
{
   // T is already const — result is still just const T* (no double-const)
   STATIC_CHECK(like_pointer_yields<Foo, const int, const int*>);
   STATIC_CHECK(like_pointer_yields<const Foo, const int, const int*>);
}
