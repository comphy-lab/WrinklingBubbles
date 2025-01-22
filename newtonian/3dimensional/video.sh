#!/bin/bash

Oh="10"
k="20"
dh="0.7"
qcc -Wall -O2 getview3D.c -o getview3D -L$BASILISK/gl -lglutils -lfb_tiny -lm
python3 render3D.py
ffmpeg -framerate 10 -pattern_type glob -i 'Video_view/*.png' -vf scale=750:600 -c:v libx264 -r 30 -pix_fmt yuv420p vid_$Oh.mp4 -y

qcc -Wall -O2 getview3D_2.c -o getview3D_2 -L$BASILISK/gl -lglutils -lfb_tiny -lm
python3 render3D_2.py
ffmpeg -framerate 10 -pattern_type glob -i 'Video_view2/*.png' -c:v libx264 -r 30 -pix_fmt yuv420p vid2_$Oh.mp4 -y



