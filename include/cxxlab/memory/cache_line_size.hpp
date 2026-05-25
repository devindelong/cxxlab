/**
 * @copyright Copyright (c) 2026, Devin DeLong. All rights reserved.
 *
 * @license This code is distributed under the BSD 3-Clause License.
 *          See the LICENSE file for the full text of the license.
 *
 * @author Devin DeLong
 */

#pragma once

#include <cstddef>
#include <new>

namespace cxxlab::constants
{

#if defined(CXXLAB_CACHE_LINE_SIZE)
inline constexpr std::size_t cache_line_size = CXXLAB_CACHE_LINE_SIZE;
#else
inline constexpr std::size_t cache_line_size = std::hardware_destructive_interference_size;
#endif // CXXLAB_CACHE_LINE_SIZE

} // namespace cxxlab::constants
