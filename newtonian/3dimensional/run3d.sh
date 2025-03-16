#!/bin/bash
Bo="0.001"
Oh="10"
k="10"
qcc -fopenmp -O2 -disable-dimensions wrinkling_bub_3d_v1.c -o wrinkling_bub_3d_v1 -lm
export OMP_NUM_THREADS=8
./wrinkling_bub_3d_v1 $Bo $Oh $k
