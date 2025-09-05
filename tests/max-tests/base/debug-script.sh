#!/bin/bash

echo "=== Debugging TMR Pass Execution ==="

# First, let's see what your TMR plugin actually contains
echo "1. Checking TMR plugin symbols:"
nm -D ../../../build-laptop/TMR/TMR.so | grep -E "(TMR|dataflow|llvmGetPassPluginInfo)"

echo ""
echo "2. Testing with opt tool (New Pass Manager):"

# Generate clean IR first
clang-16 -emit-llvm -S test_basic.c -o test_basic_clean.ll

echo "Original IR lines: $(wc -l < test_basic_clean.ll)"

# Test if the plugin loads and lists passes
echo ""
echo "3. Check if TMR pass is available:"
opt-16 -load-pass-plugin=../../../build-laptop/TMR/TMR.so -passes=help 2>&1 | grep -i tmr || echo "TMR pass not found in help"

echo ""
echo "4. Try to run TMR pass with opt:"
opt-16 -load-pass-plugin=../../../build-laptop/TMR/TMR.so -passes=tmr test_basic_clean.ll -o test_basic_tmr_opt.ll -S 2>&1

if [ -f test_basic_tmr_opt.ll ]; then
    echo "TMR opt output lines: $(wc -l < test_basic_tmr_opt.ll)"

    if [ $(wc -l < test_basic_tmr_opt.ll) -gt $(wc -l < test_basic_clean.ll) ]; then
        echo "✅ TMR transformation applied via opt!"
        echo "Showing differences:"
        diff test_basic_clean.ll test_basic_tmr_opt.ll | head -10
    else
        echo "⚠️ No transformation detected via opt"
    fi
else
    echo "❌ opt with TMR failed"
fi

echo ""
echo "5. Check if your TMR plugin was built with source inclusion:"
strings ../../../build-laptop/TMR/TMR.so | grep -i "dataflowprotection\|tmr\|coast" | head -5

echo ""
echo "6. Test if the plugin is a New PM plugin or Legacy:"
# Legacy plugins typically have different symbols
nm -D ../../../build-laptop/TMR/TMR.so | grep -E "(runOnModule|getAnalysisUsage)" && echo "Contains legacy pass symbols" || echo "No legacy pass symbols found"

echo ""
echo "7. Try with clang using New PM syntax:"
clang-16 -fpass-plugin=../../../build-laptop/TMR/TMR.so -mllvm -passes=tmr -emit-llvm -S test_basic.c -o test_basic_clang_npm.ll 2>&1

if [ -f test_basic_clang_npm.ll ]; then
    echo "Clang New PM output lines: $(wc -l < test_basic_clang_npm.ll)"

    if [ $(wc -l < test_basic_clang_npm.ll) -gt $(wc -l < test_basic_clean.ll) ]; then
        echo "✅ TMR transformation applied via clang New PM!"
    else
        echo "⚠️ No transformation detected via clang New PM"
    fi
else
    echo "❌ clang New PM failed"
fi

echo ""
echo "8. Try with clang using legacy syntax (your current approach):"
clang-16 -Xclang -load -Xclang ../../../build-laptop/TMR/TMR.so -emit-llvm -S test_basic.c -o test_basic_clang_legacy.ll 2>&1

if [ -f test_basic_clang_legacy.ll ]; then
    echo "Clang legacy output lines: $(wc -l < test_basic_clang_legacy.ll)"

    if [ $(wc -l < test_basic_clang_legacy.ll) -gt $(wc -l < test_basic_clean.ll) ]; then
        echo "✅ TMR transformation applied via clang legacy!"
    else
        echo "⚠️ No transformation detected via clang legacy - this is your current issue"
    fi
else
    echo "❌ clang legacy failed"
fi

echo ""
echo "=== Summary ==="
echo "Your current script uses legacy syntax but may need:"
echo "1. New Pass Manager syntax: -fpass-plugin + -mllvm -passes=tmr"
echo "2. Proper pass triggering mechanism"
echo "3. Check if TMR pass is actually registered in the plugin"

echo ""
echo "Files created for inspection:"
ls -la test_basic_*.ll 2>/dev/null || echo "No test files created"
