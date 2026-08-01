# ================================================================================
#
# Copyright (c) 2026 Devin DeLong
# SPDX-License-Identifier: BSD-3-clause
#
# Licensed under the BSD 3-Clause License.
# See the license file in the project root for full license information.
#
# ================================================================================

function (add_executable_for_each_source)
  set(options "")
  set(oneValueArgs "")
  set(multiValueArgs SOURCES INCLUDE_PATHS LINK_LIBRARIES)

  cmake_parse_arguments(ARGS
    "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

  foreach(main ${ARGS_SOURCES})
    get_filename_component(TARGET ${main} NAME_WE)

    add_executable(${TARGET} ${main})

    target_include_directories(
      ${TARGET}
      PUBLIC
      ${ARGS_INCLUDE_PATHS}
    )

    target_link_libraries(
      ${TARGET}
      ${ARGS_LINK_LIBRARIES}
      ${CXXLAB_SANITIZERS_LIBRARY}
    )

    target_compile_options (
      ${TARGET}
      PRIVATE
      ${CXXLAB_WARNING_FLAGS}
    )

  endforeach()
endfunction()

file(GLOB EXAMPLES_SOURCES CONFIGURE_DEPENDS *.cpp *.hpp)

add_executable_for_each_source(
  SOURCES
    ${EXAMPLES_SOURCES}
  LINK_LIBRARIES
    ${CXXLAB_LIBRARY}
)
