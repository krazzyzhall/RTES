rm -f *.out
#cd ./source/
gcc -pthread -g -o Main.out ./source/Main.c -lm
cp Main.out ./bin/
#cd ..
