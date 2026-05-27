/**
 * @copyright Copyright (c) 2026, Devin DeLong. All rights reserved.
 *
 * @license This code is distributed under the BSD 3-Clause License.
 *          See the LICENSE file for the full text of the license.
 *
 * @author Devin DeLong
 */

#include "cxxlab/functional/repeat.hpp"

#include <catch2/catch_test_macros.hpp>

#include <cstddef>

TEST_CASE("functional: repeat", "[repeat]")
{
   auto result = std::size_t{0};
   auto expected = std::size_t{10};

   cxxlab::repeat([&result]() { ++result; }, expected);
   CHECK(result == expected);
}
