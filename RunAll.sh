mpic++ micomp.cpp -O3 -std=c++11 -o micomp
mpirun -np 6 ./micomp 4000000 10 5432123
python micviz.py