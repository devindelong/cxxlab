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
  GIT_TAG        v3.15.0
)
FetchContent_MakeAvailable (Catch2)
include(Catch)

# ------------------------------------------------------------------------------
# Unit tests
# ------------------------------------------------------------------------------

include(${CXXLAB_CMAKE_PATH}/coverage.cmake)

set (CXXLAB_TESTS_TARGET "${CXXLAB_LIBRARY_NAME}_tests")

file (GLOB_RECURSE
  CXXLAB_TEST_SOURCES
  CONFIGURE_DEPENDS
  "*.cpp"
)

add_executable (
  ${CXXLAB_TESTS_TARGET}
  ${CXXLAB_TEST_SOURCES}
)

target_link_libraries (
  ${CXXLAB_TESTS_TARGET} PRIVATE
  ${CXXLAB_LIBRARY_NAME}
  ${SANITIZERS_LIBRARY}
  Catch2::Catch2WithMain
)

target_compile_options (
  ${CXXLAB_TESTS_TARGET}
  PRIVATE
  ${CXXLAB_WARNING_FLAGS}
)

add_test (
  NAME ${CXXLAB_TESTS_TARGET}
  COMMAND ${CXXLAB_TESTS_TARGET}
 )

# Silence this warning
# Clang 22 error: '__COUNTER__' is a C2y extension [-Werror,-Wc2y-extensions]
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  target_compile_options(${CXXLAB_TESTS_TARGET} PRIVATE -Wno-c2y-extensions)
endif()


if (ENABLE_COVERAGE)
  target_enable_coverage(${CXXLAB_TESTS_TARGET})
endif()

catch_discover_tests (${CXXLAB_TESTS_TARGET})
