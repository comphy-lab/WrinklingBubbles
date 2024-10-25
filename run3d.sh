#!/bin/bash
qcc -fopenmp -O2 -disable-dimensions wrinkling_bub_3d_v2.c -o wrinkling_bub_3d_v2 -lm
export OMP_NUM_THREADS=8
./wrinkling_bub_3d_v2
