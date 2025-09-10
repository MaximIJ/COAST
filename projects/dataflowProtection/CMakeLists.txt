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
