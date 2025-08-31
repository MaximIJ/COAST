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
  add_llvm_pass_plugin(TMR
    TMRNewPM.cpp
    # Optionally keep legacy pass (commented):
    # TMR.cpp
    LINK_COMPONENTS
      Core
      IRReader
      TransformUtils
      Analysis
      Support)
else()
  add_llvm_loadable_module(TMR
    TMR.cpp)
endif()

