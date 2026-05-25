# ==============================================================================
#
# Copyright (c) 2026 Devin DeLong
# SPDX-License-Identifier: BSD-3-clause
#
# Licensed under the BSD 3-Clause License.
# See the license file in the project root for full license information.
#
# ==============================================================================

function(target_enable_coverage target)
    if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(${target} PRIVATE --coverage -fno-inline)
        target_link_options(${target} PRIVATE --coverage)
    else()
        message(WARNING "Coverage not supported for ${CMAKE_CXX_COMPILER_ID}")
    endif()
endfunction()
