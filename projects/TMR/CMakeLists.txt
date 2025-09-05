cmake_minimum_required(VERSION 3.13)

# Silence LLVM's strict source list checking for legacy file not built in new PM
set(LLVM_OPTIONAL_SOURCES TMR.cpp)

# New Pass Manager plugin build
if (DEFINED LLVM_ENABLE_NEW_PASS_MANAGER)
  set(USE_NEW_PM ${LLVM_ENABLE_NEW_PASS_MANAGER})
else()
  set(USE_NEW_PM ON)
endif()

if (USE_NEW_PM)
  # Include all dataflowProtection sources directly in the TMR plugin
  add_llvm_pass_plugin(TMR
    TMRNewPM.cpp
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
      Passes)

  # Add include directory for dataflowProtection headers
  target_include_directories(TMR PRIVATE ../dataflowProtection)

  # Ensure proper symbol visibility for plugin loading
  set_target_properties(TMR PROPERTIES
    CXX_VISIBILITY_PRESET default
    VISIBILITY_INLINES_HIDDEN NO)

else()
  add_llvm_loadable_module(TMR
    TMR.cpp
    # Include dataflowProtection sources for legacy build too
    ../dataflowProtection/dataflowProtection.cpp
    ../dataflowProtection/cloning.cpp
    ../dataflowProtection/synchronization.cpp
    ../dataflowProtection/utils.cpp
    ../dataflowProtection/verification.cpp
    ../dataflowProtection/interface.cpp
    ../dataflowProtection/inspection.cpp)

  target_include_directories(TMR PRIVATE ../dataflowProtection)
endif()
