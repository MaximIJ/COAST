cmake_minimum_required(VERSION 3.13)
project(coast)

set ( COAST_BASE_DIR ..)
if(NOT EXISTS ${COAST_BASE_DIR})
	message ( FATAL_ERROR " COAST_BASE_DIR (${COAST_BASE_DIR}) is invalid ")
endif ()

# Allow the user to specify a recent LLVM; require >= 14
find_package(LLVM 16 REQUIRED CONFIG)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

list(APPEND CMAKE_MODULE_PATH "${LLVM_CMAKE_DIR}")

include(HandleLLVMOptions)
include(AddLLVM)


# Compatibility shim: legacy builds used add_llvm_loadable_module, which is
# removed in newer LLVM. Map it to add_llvm_pass_plugin by default.
if(NOT COMMAND add_llvm_loadable_module)
  function(add_llvm_loadable_module target)
    # Forward sources and add safe default link components
    add_llvm_pass_plugin(${target}
      ${ARGN}
      LINK_COMPONENTS
        Core
        IRReader
        TransformUtils
        Analysis
        Support)
  endfunction()
endif()


add_definitions(${LLVM_DEFINITIONS})
include_directories(${LLVM_INCLUDE_DIRS})

# Build flags for legacy passes (off by default on modern LLVM)
option(COAST_BUILD_LEGACY_PASSES "Build legacy ModulePass plugins (deprecated)" OFF)

add_subdirectory (dataflowProtection)
add_subdirectory (TMR)

if (COAST_BUILD_LEGACY_PASSES)
  add_subdirectory (debugStatements)
  add_subdirectory (exitMarker)
  add_subdirectory (CFCSS)
  add_subdirectory (DWC)
  add_subdirectory (EDDI)
  add_subdirectory (smallProfile)
endif()
