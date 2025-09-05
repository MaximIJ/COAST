#!/bin/bash

echo "=== Fixing TMR Plugin Linking Issues ==="

cd ../../../build-laptop

echo "1. Current linking status:"
echo "TMR undefined symbols:"
nm -u TMR/TMR.so | grep dataflow
echo ""
echo "DataflowProtection exported symbols:"
nm -D dataflowProtection/DataflowProtection.so | grep "dataflowProtection.*run"
echo ""

echo "2. Method 1: Rebuild with proper linking"
echo "Cleaning TMR..."
rm -f TMR/TMR.so TMR/libTMR.so
rm -rf TMR/CMakeFiles

echo "Rebuilding TMR with verbose linking..."
make TMR VERBOSE=1 2>&1 | tee tmr_build.log

echo ""
echo "3. Check if rebuild fixed the linking:"
if [ -f TMR/TMR.so ]; then
    echo "TMR plugin rebuilt. Checking symbols:"
    nm -u TMR/TMR.so | grep dataflow || echo "✓ No undefined dataflow symbols"

    echo ""
    echo "Testing plugin loading:"
    opt-16 -load-pass-plugin=./TMR/TMR.so -passes=help 2>&1 | head -5
else
    echo "❌ TMR plugin build failed"
fi

echo ""
echo "4. Method 2: Try loading both plugins together"
if [ -f TMR/TMR.so ] && [ -f dataflowProtection/DataflowProtection.so ]; then
    echo "Testing with dataflowProtection loaded first:"

    # Method A: Load dataflow as legacy, TMR as plugin
    opt-16 -load ./dataflowProtection/DataflowProtection.so -load-pass-plugin=./TMR/TMR.so -passes=help 2>&1 | head -5

    echo ""
    echo "Method B: Create combined test"
    echo "opt-16 -load ./dataflowProtection/DataflowProtection.so -load-pass-plugin=./TMR/TMR.so -passes=tmr test_basic.ll -o test_basic_tmr.ll -S"
fi

echo ""
echo "5. Method 3: Build legacy passes instead"
echo "Reconfiguring with legacy support..."
cmake .. -DCOAST_BUILD_LEGACY_PASSES=ON -DLLVM_DIR=$(llvm-config-16 --prefix)/lib/cmake/llvm

echo "Building legacy passes..."
make clean
make -j$(nproc) 2>&1 | tee legacy_build.log

echo ""
echo "Testing legacy TMR pass:"
if [ -f TMR/TMR.so ]; then
    echo "Legacy TMR symbols:"
    nm -D TMR/TMR.so | grep -E "(TMR|dataflow)" | head -5

    echo ""
    echo "Testing legacy pass loading:"
    clang-16 -Xclang -load -Xclang ./dataflowProtection/DataflowProtection.so -Xclang -load -Xclang ./TMR/TMR.so -c ../test_basic.c -o test_basic_legacy.o

    if [ $? -eq 0 ]; then
        echo "✅ Legacy pass compilation successful!"
    else
        echo "❌ Legacy pass compilation failed"
    fi
fi

echo ""
echo "=== Summary ==="
echo "Check the following files for detailed output:"
echo "- tmr_build.log: New PM build details"
echo "- legacy_build.log: Legacy build details"
