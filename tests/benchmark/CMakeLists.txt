# ==============================================================================
#
# Copyright (c) 2026 Devin DeLong
# SPDX-License-Identifier: BSD-3-clause
#
# Licensed under the BSD 3-Clause License.
# See the license file in the project root for full license information.
#
# ==============================================================================

set (CXXLAB_BENCHMARK_TARGET "${CXXLAB_LIBRARY_NAME}_benchmark")

file (GLOB_RECURSE
  CXXLAB_BENCHMARK_SOURCES
  CONFIGURE_DEPENDS
  "*.cpp"
)

add_executable (
  ${CXXLAB_BENCHMARK_TARGET}
  ${CXXLAB_BENCHMARK_SOURCES}
)

target_link_libraries (
  ${CXXLAB_BENCHMARK_TARGET} PRIVATE
  ${CXXLAB_LIBRARY_NAME}
  ${CXXLAB_SANITIZERS_LIBRARY}
  Catch2::Catch2WithMain
)

target_compile_options (
  ${CXXLAB_BENCHMARK_TARGET}
  PRIVATE
  ${CXXLAB_WARNING_FLAGS}
)
