#!/bin/bash
Bo="0.001"
Oh="20"
k="20"
qcc -fopenmp -O2 -disable-dimensions wrinkling_bub_axi_v1.c -o wrinkling_bub_axi_v1 -lm
export OMP_NUM_THREADS=4
./wrinkling_bub_axi_v1 $Bo $Oh $k