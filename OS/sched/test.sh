#!/bin/bash
for i in {1..3}; do
  ex1 > out.txt
  cat out.txt
  cat out.txt | grep "exit"
done
rm -f out.txt

echo "3 runs completed"

FILE = "ex1.cpp"
OPT0 = "const int nice_vals[3] = {0, 10, 19};"
OPT1 = "const int nice_vals[3] = {0, 0, 0};"
OPT2 = "const int nice_vals[3] = {0, 5, 19};"
OPT3 = "const int nice_vals[3] = {0, 15, 19};"

for i in {1..3} ; do
  eho "\n"
  echo "option $((i))"
  PREV = OPT$((i-1))
  NEXT = OPT$i
  's/${PREV}/${NEXT}/g' "$FILE"
  make clean
  make
  ./ex1
done
