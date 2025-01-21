#!/bin/bash

Oh="1"
k="10"
dh="0.1"
qcc -Wall -O2 getview3D.c -o getview3D -L$BASILISK/gl -lglutils -lfb_tiny -lm
python3 render3D.py
ffmpeg -framerate 10 -pattern_type glob -i 'Video_view/*.png' -vf scale=750:600 -c:v libx264 -r 30 -pix_fmt yuv420p vid_$Oh.mp4 -y
