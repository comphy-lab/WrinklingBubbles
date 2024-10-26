#!/bin/bash
qcc -fopenmp -O2 -disable-dimensions get_vtu.c -o get_vtu -lm
python3 get_vtu.py