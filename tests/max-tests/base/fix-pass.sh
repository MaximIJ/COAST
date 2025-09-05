#!/bin/bash

echo "=== Fixing Pass Registration Issues ==="

# The issue is that your plugin contains both legacy and new PM code,
# but the New PM registration might not be working correctly.

echo "1. Testing Legacy Pass Manager (which should work):"

# Test legacy pass with explicit pass name
clang-16 -Xclang -load -Xclang ../../../build-laptop/TMR/TMR.so -Xclang -add-plugin -Xclang TMR -c test_basic.c -o test_basic_legacy_explicit.o 2>&1

if [ $? -eq 0 ]; then
    echo "✅ Legacy pass with explicit plugin name works"
else
    echo "❌ Legacy pass with explicit plugin name failed"
fi

echo ""
echo "2. Check what legacy passes are available:"
opt-16 -load ../../../build-laptop/TMR/TMR.so -help 2>&1 | grep -A5 -B5 "TMR\|Optimizations available" | head -20

echo ""
echo "3. Test legacy pass with opt:"
clang-16 -emit-llvm -S test_basic.c -o test_basic_for_opt.ll
opt-16 -load ../../../build-laptop/TMR/TMR.so -TMR test_basic_for_opt.ll -o test_basic_legacy_opt.ll -S 2>&1

if [ -f test_basic_legacy_opt.ll ]; then
    orig_lines=$(wc -l < test_basic_for_opt.ll)
    tmr_lines=$(wc -l < test_basic_legacy_opt.ll)
    echo "Legacy opt: $orig_lines → $tmr_lines lines"

    if [ $tmr_lines -gt $orig_lines ]; then
        echo "✅ Legacy TMR pass transforms code!"
        echo "First few differences:"
        diff test_basic_for_opt.ll test_basic_legacy_opt.ll | head -5
    else
        echo "⚠️ Legacy pass runs but no transformation"
    fi
else
    echo "❌ Legacy opt failed"
fi

echo ""
echo "4. Fix New Pass Manager registration:"

# Create a simpler TMRNewPM.cpp that definitely works
cat > /tmp/TMRNewPM_fixed.cpp << 'EOF'
#include "../dataflowProtection/dataflowProtection.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

namespace {

struct TMRPass : PassInfoMixin<TMRPass> {
  PreservedAnalyses run(Module &M, ModuleAnalysisManager &) {
    errs() << "TMR Pass starting on module: " << M.getName() << "\n";

    dataflowProtection DP;
    bool changed = DP.run(M, 3); // TMR mode

    errs() << "TMR Pass finished. Changed: " << (changed ? "YES" : "NO") << "\n";

    return PreservedAnalyses::none();
  }
};

} // namespace

extern "C" LLVM_ATTRIBUTE_WEAK PassPluginLibraryInfo llvmGetPassPluginInfo() {
  errs() << "TMR Plugin Info requested - registering passes\n";

  return {LLVM_PLUGIN_API_VERSION, "TMR_Plugin", "1.0",
          [](PassBuilder &PB) {
            errs() << "Registering TMR pass in pipeline\n";

            PB.registerPipelineParsingCallback(
                [](StringRef Name, ModulePassManager &MPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                  errs() << "Pipeline callback called with: '" << Name << "'\n";

                  if (Name == "tmr") {
                    errs() << "Adding TMR pass to pipeline\n";
                    MPM.addPass(TMRPass());
                    return true;
                  }
                  return false;
                });
          }};
}
EOF

echo "✅ Created fixed TMRNewPM.cpp with debug output"

# Backup original and replace
cp ../../../projects/TMR/TMRNewPM.cpp ../../../projects/TMR/TMRNewPM.cpp.backup
cp /tmp/TMRNewPM_fixed.cpp ../../../projects/TMR/TMRNewPM.cpp

echo ""
echo "5. Rebuild TMR with fixed registration:"
cd ../../../build-laptop
rm -rf TMR/CMakeFiles TMR/TMR.so
make TMR 2>&1 | grep -E "(error|Error|TMR)"

if [ -f TMR/TMR.so ]; then
    echo "✅ Rebuilt TMR plugin"

    echo ""
    echo "6. Test fixed New PM plugin:"

    # Test with verbose debug output
    echo "Testing with opt and debug output:"
    opt-16 -load-pass-plugin=./TMR/TMR.so -passes=tmr ../test_basic_for_opt.ll -o test_basic_npm_fixed.ll -S

    if [ -f test_basic_npm_fixed.ll ]; then
        orig_lines=$(wc -l < ../test_basic_for_opt.ll)
        npm_lines=$(wc -l < test_basic_npm_fixed.ll)
        echo "New PM: $orig_lines → $npm_lines lines"

        if [ $npm_lines -gt $orig_lines ]; then
            echo "🎉 NEW PM TMR PASS WORKS!"
        fi
    fi

    echo ""
    echo "7. Test with clang New PM:"
    cd ..
    clang-16 -fpass-plugin=../build-laptop/TMR/TMR.so -mllvm -passes=tmr -emit-llvm -S test_basic.c -o test_basic_clang_npm_fixed.ll

    if [ -f test_basic_clang_npm_fixed.ll ]; then
        echo "✅ Clang New PM compilation successful"

        # Compare with original
        if [ -f test_basic_for_opt.ll ]; then
            orig_lines=$(wc -l < test_basic_for_opt.ll)
            clang_lines=$(wc -l < test_basic_clang_npm_fixed.ll)
            echo "Clang New PM: $orig_lines → $clang_lines lines"
        fi
    fi

else
    echo "❌ TMR rebuild failed"
fi

echo ""
echo "=== Summary ==="
echo "The issue was likely in the New PM registration callback."
echo "The fixed version adds debug output so you can see exactly what's happening."
echo ""
echo "If this works, update your test script to use:"
echo "  clang-16 -fpass-plugin=TMR.so -mllvm -passes=tmr"
echo "Instead of:"
echo "  clang++-16 -Xclang -load -Xclang TMR.so"
