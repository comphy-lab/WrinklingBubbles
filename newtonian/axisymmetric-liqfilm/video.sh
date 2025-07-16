#!/bin/bash
 
 
Lo="1.2"
nR="512"
Oh="20"
tsnap="0.01"
mu21="1e-4"

qcc -O2 -disable-dimensions getFacet.c -o getFacet -lm
qcc -O2 -disable-dimensions getData.c -o getData -lm
python3 VideoFullDomain.py $Lo $nR $Oh $tsnap $mu21
ffmpeg -framerate 30 -pattern_type glob -i 'Video_v1/*.png' -vf scale=850:880 -c:v libx264 -r 30 -pix_fmt yuv420p vid_$Oh.mp4 -y


