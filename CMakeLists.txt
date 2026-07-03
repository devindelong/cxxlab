# ==============================================================================
#
# Copyright (c) 2026 Devin DeLong
# SPDX-License-Identifier: BSD-3-clause
#
# Licensed under the BSD 3-Clause License.
# See the license file in the project root for full license information.
#
# ==============================================================================

cmake_minimum_required (VERSION 3.27)

project (cxxlab
  LANGUAGES CXX
)

# Sanitizer build options.
option(ENABLE_ASAN "Enable address sanitizer" "OFF")
option(ENABLE_TSAN "Enable thread sanitizer" "OFF")
option(ENABLE_UBSAN "Enable UB sanitizer" "OFF")

# Build options.
option(ENABLE_TESTING "Enable testing" ${PROJECT_IS_TOP_LEVEL})
option(ENABLE_APPS "Enable example apps" ${PROJECT_IS_TOP_LEVEL})

set (CXXLAB_CACHE_LINE_SIZE "" CACHE STRING
  "Override cache line size (leave empty to use std::hardware_destructive_interference_size)")

# Set aliases for the project directories.
set (CXXLAB_LIBRARY_NAME         "cxxlab")
set (CXXLAB_ROOT_PATH            "${PROJECT_SOURCE_DIR}")
set (CXXLAB_ROOT_INCLUDE_PATH    "${CXXLAB_ROOT_PATH}/include")
set (CXXLAB_LIBRARY_INCLUDE_PATH "${CXXLAB_ROOT_INCLUDE_PATH}/${CXXLAB_LIBRARY_NAME}")
set (CXXLAB_TEST_PATH            "${CXXLAB_ROOT_PATH}/tests")
set (CXXLAB_APPS_PATH            "${CXXLAB_ROOT_PATH}/apps")
set (CXXLAB_CMAKE_PATH           "${CXXLAB_ROOT_PATH}/cmake")

################################################################################
# CxxLab library
################################################################################

# C++ version.
set (CXXLAB_CXX_VERSION cxx_std_26)

# Library name
set (CXXLAB_LIBRARY ${CXXLAB_LIBRARY_NAME})

set(CXXLAB_WARNING_FLAGS
    -Wall # enables most common warnings (misleadingly not "all")
    -Wextra # the next tier that -Wall misses
    -Wpedantic # strict ISO conformance, catches non-standard extensions
    -Wconversion # implicit narrowing conversions, catches int → float etc.
    -Wshadow # local variable shadows outer scope variable
    -Wnon-virtual-dtor # class with virtual functions but non-virtual destructor
    -Wold-style-cast # C-style casts, encourages static_cast etc.
    -Wcast-align # casting pointer to a type with stricter alignment
    -Woverloaded-virtual # derived class hides base class virtual method
    -Wnull-dereference # detectable null dereference paths
)

# Disables the warning for using std::hardware_destructive_interference_size.
if (NOT CXXLAB_CACHE_LINE_SIZE AND CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    list(APPEND CXXLAB_WARNING_FLAGS -Wno-interference-size)
endif()

# Create header only library.
add_library (${CXXLAB_LIBRARY} INTERFACE)

target_include_directories (
  ${CXXLAB_LIBRARY}
  INTERFACE
    ${CXXLAB_ROOT_INCLUDE_PATH}
)

target_compile_features (
  ${CXXLAB_LIBRARY}
  INTERFACE
    ${CXXLAB_CXX_VERSION}
)

if(CXXLAB_CACHE_LINE_SIZE)
  target_compile_definitions (
    ${CXXLAB_LIBRARY}
    PRIVATE
      CXXLAB_CACHE_LINE_SIZE=${CXXLAB_CACHE_LINE_SIZE}
  )
endif()

################################################################################
# Sanitizers
################################################################################

message (STATUS "Building with address sanitizer - ${ENABLE_ASAN}")
message (STATUS "Building with thread sanitizer - ${ENABLE_TSAN}")
message (STATUS "Building with UB sanitizer - ${ENABLE_UBSAN}")

set (SANITIZERS_LIBRARY ${PROJECT_NAME}_sanitizers)
add_library(${SANITIZERS_LIBRARY} INTERFACE)

if(ENABLE_ASAN)
  target_compile_options(${SANITIZERS_LIBRARY} INTERFACE -fsanitize=address -fno-omit-frame-pointer)
  target_link_options(${SANITIZERS_LIBRARY} INTERFACE -fsanitize=address)
endif()

if(ENABLE_TSAN)
  target_compile_options(${SANITIZERS_LIBRARY} INTERFACE -fsanitize=thread)
  target_link_options(${SANITIZERS_LIBRARY} INTERFACE -fsanitize=thread)
endif()

if(ENABLE_UBSAN)
  target_compile_options(${SANITIZERS_LIBRARY} INTERFACE -fsanitize=undefined -fno-omit-frame-pointer)
  target_link_options(${SANITIZERS_LIBRARY} INTERFACE -fsanitize=undefined)
endif()

################################################################################
# Testing
################################################################################

# Testing
message (STATUS "Building unit tests - ${ENABLE_TESTING}")
if (ENABLE_TESTING)
  enable_testing()
  add_subdirectory ("${CXXLAB_TEST_PATH}")
endif()

################################################################################
# Example apps
################################################################################

message (STATUS "Building example apps - ${ENABLE_APPS}")
if (ENABLE_APPS)
  add_subdirectory ("${CXXLAB_APPS_PATH}")
endif()
