#!/bin/bash

Oh="0.01"
k="20"
tsnap="0.01"

qcc -Wall -O2 track_tip.c -o track_tip -lm
python3 track_tip.py $Oh $k $tsnap
python3 plot_tracktip.py $Oh $k