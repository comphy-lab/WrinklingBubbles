#!/bin/bash
qcc -O2 -disable-dimensions wrinkling_bub_axi_v1.c -o wrinkling_bub_axi_v1 -lm
export OMP_NUM_THREADS=4
./wrinkling_bub_axi_v1