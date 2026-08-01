# ==============================================================================
#
# Copyright (c) 2026 Devin DeLong
# SPDX-License-Identifier: BSD-3-clause
#
# Licensed under the BSD 3-Clause License.
# See the license file in the project root for full license information.
#
# ==============================================================================

# ------------------------------------------------------------------------------
# Fetch Catch2
# ------------------------------------------------------------------------------

include (FetchContent)
FetchContent_Declare (
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.15.3
)
FetchContent_MakeAvailable (Catch2)
include(Catch)

# ------------------------------------------------------------------------------

set (CXXLAB_UNIT_TEST_PATH       "${CMAKE_CURRENT_SOURCE_DIR}/unit")
set (CXXLAB_BENCHMARK_TEST_PATH  "${CMAKE_CURRENT_SOURCE_DIR}/benchmark")

# Unit testing
message (STATUS "Building unit tests - ${ENABLE_TESTING}")
if (ENABLE_TESTING)
  add_subdirectory (${CXXLAB_UNIT_TEST_PATH})
endif()

# Benchmark testing
message (STATUS "Building benchmarks - ${ENABLE_BENCHMARKS}")
if (ENABLE_BENCHMARKS)
  add_subdirectory (${CXXLAB_BENCHMARK_TEST_PATH})
endif()
