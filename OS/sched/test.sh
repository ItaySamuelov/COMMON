#!/bin/bash
make clean
make
for i in {1..3}; do
  echo -e "\n"
  echo "run number $((i))"
  ./ex1
done
rm -f out.txt

echo "3 runs completed"

FILE="ex1.cpp"
OPT0="{0, 10, 19};"
OPT1="{0, 0, 0};"
OPT2="{0, 5, 19};"
OPT3="{0, 15, 19};"

for i in {1..3} ; do
  echo -e "\n"
  echo "option $((i))"
  PREV="OPT$(($i-1))"
  NEXT="OPT$i"

  sed -i "s/${!PREV}/${!NEXT}/g" "$FILE"
  make clean
  make
  ./ex1
done
