#!/bin/zsh
source ~/.zshrc

Bo="10"
Oh="0.01"

qcc -fopenmp -O2 -disable-dimensions surface_bubble.c -o surface_bubble -lm
export OMP_NUM_THREADS=4
./surface_bubble $Bo $Oh $k