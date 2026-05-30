/**
 * @copyright Copyright (c) 2026, Devin DeLong. All rights reserved.
 *
 * @license This code is distributed under the BSD 3-Clause License.
 *          See the LICENSE file for the full text of the license.
 *
 * @author Devin DeLong
 */

#include "cxxlab/concepts/chrono.hpp"

#include <catch2/catch_test_macros.hpp>
#include <chrono>

using namespace std::chrono_literals;

using custom_duration = std::chrono::duration<double, std::ratio<1, 1000>>;
using custom_time_point = std::chrono::time_point<std::chrono::system_clock, custom_duration>;

// ---------------------------------------------------------------------------
// chrono_duration
// ---------------------------------------------------------------------------

TEST_CASE("chrono_duration - positive cases", "[concepts][chrono_duration]")
{
   STATIC_CHECK(cxxlab::chrono_duration<std::chrono::nanoseconds>);
   STATIC_CHECK(cxxlab::chrono_duration<std::chrono::milliseconds>);
   STATIC_CHECK(cxxlab::chrono_duration<std::chrono::seconds>);
   STATIC_CHECK(cxxlab::chrono_duration<std::chrono::minutes>);
   STATIC_CHECK(cxxlab::chrono_duration<std::chrono::hours>);
   STATIC_CHECK(cxxlab::chrono_duration<custom_duration>);
}

TEST_CASE("chrono_duration - cvref-qualified types are accepted", "[concepts][chrono_duration]")
{
   STATIC_CHECK(cxxlab::chrono_duration<const std::chrono::nanoseconds>);
   STATIC_CHECK(cxxlab::chrono_duration<volatile std::chrono::nanoseconds>);
   STATIC_CHECK(cxxlab::chrono_duration<const volatile std::chrono::nanoseconds>);
   STATIC_CHECK(cxxlab::chrono_duration<std::chrono::nanoseconds&>);
   STATIC_CHECK(cxxlab::chrono_duration<const std::chrono::nanoseconds&>);
   STATIC_CHECK(cxxlab::chrono_duration<std::chrono::nanoseconds&&>);
}

TEST_CASE("chrono_duration - negative cases", "[concepts][chrono_duration]")
{
   STATIC_CHECK_FALSE(cxxlab::chrono_duration<int>);
   STATIC_CHECK_FALSE(cxxlab::chrono_duration<double>);
   STATIC_CHECK_FALSE(cxxlab::chrono_duration<std::string>);
   STATIC_CHECK_FALSE(cxxlab::chrono_duration<decltype(std::ignore)>);
   // time_point is not a duration
   STATIC_CHECK_FALSE(cxxlab::chrono_duration<std::chrono::system_clock::time_point>);
   STATIC_CHECK_FALSE(cxxlab::chrono_duration<std::chrono::steady_clock::time_point>);
   STATIC_CHECK_FALSE(cxxlab::chrono_duration<custom_time_point>);
}

// ---------------------------------------------------------------------------
// chrono_time_point
// ---------------------------------------------------------------------------

TEST_CASE("chrono_time_point - positive cases", "[concepts][chrono_time_point]")
{
   STATIC_CHECK(cxxlab::chrono_time_point<std::chrono::system_clock::time_point>);
   STATIC_CHECK(cxxlab::chrono_time_point<std::chrono::steady_clock::time_point>);
   STATIC_CHECK(cxxlab::chrono_time_point<custom_time_point>);
   STATIC_CHECK(
      cxxlab::chrono_time_point<
         std::chrono::time_point<std::chrono::system_clock, std::chrono::milliseconds>>);
}

TEST_CASE("chrono_time_point - cvref-qualified types are accepted", "[concepts][chrono_time_point]")
{
   STATIC_CHECK(cxxlab::chrono_time_point<const std::chrono::system_clock::time_point>);
   STATIC_CHECK(cxxlab::chrono_time_point<volatile std::chrono::system_clock::time_point>);
   STATIC_CHECK(cxxlab::chrono_time_point<const volatile std::chrono::system_clock::time_point>);
   STATIC_CHECK(cxxlab::chrono_time_point<std::chrono::system_clock::time_point&>);
   STATIC_CHECK(cxxlab::chrono_time_point<const std::chrono::system_clock::time_point&>);
   STATIC_CHECK(cxxlab::chrono_time_point<std::chrono::system_clock::time_point&&>);
}

TEST_CASE("chrono_time_point - negative cases", "[concepts][chrono_time_point]")
{
   STATIC_CHECK_FALSE(cxxlab::chrono_time_point<int>);
   STATIC_CHECK_FALSE(cxxlab::chrono_time_point<double>);
   STATIC_CHECK_FALSE(cxxlab::chrono_time_point<std::string>);
   STATIC_CHECK_FALSE(cxxlab::chrono_time_point<decltype(std::ignore)>);

   // duration is not a time_point
   STATIC_CHECK_FALSE(cxxlab::chrono_time_point<std::chrono::nanoseconds>);
   STATIC_CHECK_FALSE(cxxlab::chrono_time_point<std::chrono::milliseconds>);
   STATIC_CHECK_FALSE(cxxlab::chrono_time_point<custom_duration>);
}
