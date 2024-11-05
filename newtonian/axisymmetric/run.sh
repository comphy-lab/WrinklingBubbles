#!/bin/bash
Bo="10.0"
Oh="10"
k="10"
qcc -fopenmp -O2 -disable-dimensions wrinkling_bub_axi_v1.c -o wrinkling_bub_axi_v1 -lm
export OMP_NUM_THREADS=4
./wrinkling_bub_axi_v1 $Bo $Oh $k