#!/usr/bin/env bash

# 1) 編譯 no_weak.cpp，生成 a.out
g++ no_weak.cpp -o a.out

# 2) 執行 a.out 時帶參數 2 all_no_weak，並將輸出導入 all_no_weak.txt
./a.out 4 all_no_weak > all_no_weak.txt

# 3) 編譯 unique.cpp，生成 b.out
g++ unique_no_weak.cpp -o b.out

# 4) 執行 b.out，將輸出導入 unique_no_weak.txt
./b.out > unique_no_weak.txt

echo "Done. Outputs are in all_no_weak.txt and unique_no_weak.txt"
