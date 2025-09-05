# Create the .ll file using 0 optimization and debug info
clang-16 -O0 -ggdb3 -emit-llvm -S test_arrays.c -o test_array_original.ll

# Run the optimizeron the .ll using the plugin
opt-16 -load-pass-plugin=../../../build-laptop/TMR/TMR.so -passes=tmr -configFile=../../../projects/dataflowProtection/functions.config test_array_original.ll -o test_array_tmr.ll -S

# Compile the .ll to bianry
clang-16 test_array_tmr.ll -o test_array_tmr
