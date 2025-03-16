#!/bin/bash
Bo="0.001"
Oh="20"
k="20"
Ec="0.01"
De="1.0"

qcc -fopenmp -O2 -disable-dimensions wrinkling_bub_axi.c -o wrinkling_bub_axi -lm
export OMP_NUM_THREADS=4
./wrinkling_bub_axi $Bo $Oh $k $Ec $De