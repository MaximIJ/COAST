cmake_minimum_required(VERSION 3.13)

add_llvm_pass_plugin(TMR
  TMR.cpp
  # Include all dataflowProtection sources
  ../dataflowProtection/dataflowProtection.cpp
  ../dataflowProtection/cloning.cpp
  ../dataflowProtection/synchronization.cpp
  ../dataflowProtection/utils.cpp
  ../dataflowProtection/verification.cpp
  ../dataflowProtection/interface.cpp
  ../dataflowProtection/inspection.cpp
  LINK_COMPONENTS
    Core
    IRReader
    TransformUtils
    Analysis
    Support
    Passes
)

# Add include directory for dataflowProtection headers
target_include_directories(TMR PRIVATE ../dataflowProtection)

# Ensure proper symbol visibility for plugin loading
set_target_properties(TMR PROPERTIES
  CXX_VISIBILITY_PRESET default
  VISIBILITY_INLINES_HIDDEN NO
)

if (COAST_BUILD_LEGACY_PASSES)
  add_llvm_loadable_module(TMRLegacy
    TMR.cpp
    # Include dataflowProtection sources for legacy build too
    ../dataflowProtection/dataflowProtection.cpp
    ../dataflowProtection/cloning.cpp
    ../dataflowProtection/synchronization.cpp
    ../dataflowProtection/utils.cpp
    ../dataflowProtection/verification.cpp
    ../dataflowProtection/interface.cpp
    ../dataflowProtection/inspection.cpp
  )
  target_include_directories(TMRLegacy PRIVATE ../dataflowProtection)
endif()
