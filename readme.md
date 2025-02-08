chmod +x run_no_weak.sh
./run_no_weak.sh


g++ -o a.out a.cpp -fopenmp
./a.out 4 emu 125 > a.txt
./b.out > b.txt

du -sh .

