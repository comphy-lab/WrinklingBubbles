#!/bin/bash
## Submission script for Cluster
#SBATCH --job-name=wrinklingbub3d
#SBATCH --time=10:00:00 # hh:mm:ss
#SBATCH --nodes=2
#SBATCH --ntasks-per-node=40
#SBATCH --cpus-per-task=1
#SBATCH --partition=medium
#SBATCH -e foamjobname.std.err_%j
#SBATCH -o foamjobname.std.out_%j

#Load modules
module load compiler/gcc/10.2.0
module load compiler/openmpi/4.0.2 
#module load compiler/gcc/10.2.0
#Source basilisk
#export PATH=$PATH:/home/20me10075/basilisk/src/
source ~/.bashrc
#export PATH="~/.local/bin:$PATH"
#

Bo="0.0"
Oh="10"
k="10"

export OMP_NUM_THREADS=80
qcc -fopenmp -O2 -disable-dimensions wrinkling_bub_3d_v1.c -o wrinkling_bub_3d_v1 -lm
./wrinkling_bub_3d_v2 $Bo $Oh $k

