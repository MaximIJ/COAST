cmake_minimum_required(VERSION 3.13)

add_llvm_pass_plugin(DWC
  DWC.cpp
  LINK_COMPONENTS
    Core
    IRReader
    TransformUtils
    Analysis
    Support
    Passes
)

# Link against the shared dataflowProtection library
target_link_libraries(DWC PRIVATE DataflowProtectionLib)

# Add include directory for dataflowProtection headers
target_include_directories(DWC PRIVATE ../dataflowProtection)

# Ensure proper symbol visibility for plugin loading
set_target_properties(DWC PROPERTIES
  CXX_VISIBILITY_PRESET default
  VISIBILITY_INLINES_HIDDEN NO)

if (COAST_BUILD_LEGACY_PASSES)
  add_llvm_loadable_module(DWCLegacy
    DWC.cpp
  )
  target_include_directories(DWCLegacy PRIVATE ../dataflowProtection)
  target_link_libraries(DWCLegacy PRIVATE DataflowProtectionLib)
endif()