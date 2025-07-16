#!/bin/bash
Oh="0.01"
k="20"
tmax="0.18"

qcc -Wall -O2 -disable-dimensions tip_vel.c -o tip_vel -lm
python3 tip_vel.py
python3 TipPlots.py $Oh $k $tmax