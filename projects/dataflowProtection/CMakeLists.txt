cmake_minimum_required(VERSION 3.13)

# Create a shared library for dataflowProtection code
add_llvm_library(DataflowProtectionLib SHARED
    dataflowProtection.cpp
    commandLineOptions.cpp
    cloning.cpp
    synchronization.cpp
    utils.cpp
    verification.cpp
    interface.cpp
    inspection.cpp
    dataflowProtection.h
)

# Also create the loadable module for backward compatibility
add_llvm_loadable_module(DataflowProtection
    dataflowProtection.cpp
    commandLineOptions.cpp
    cloning.cpp
    synchronization.cpp
    utils.cpp
    verification.cpp
    interface.cpp
    inspection.cpp
    dataflowProtection.h
)

# Create new pass manager plugin for DataflowProtection
add_llvm_pass_plugin(DataflowProtectionNewPM
    DataflowProtectionNewPM.cpp
    LINK_COMPONENTS
        Core
        IRReader
        TransformUtils
        Analysis
        Support
        Passes
)

# Link against the shared dataflowProtection library
target_link_libraries(DataflowProtectionNewPM PRIVATE DataflowProtectionLib)

# Add include directory for dataflowProtection headers
target_include_directories(DataflowProtectionNewPM PRIVATE .)

# Ensure proper symbol visibility for plugin loading
set_target_properties(DataflowProtectionNewPM PROPERTIES
    CXX_VISIBILITY_PRESET default
    VISIBILITY_INLINES_HIDDEN NO
)