#!/bin/bash
#SBATCH --job-name=life
#SBATCH --partition=any
#SBATCH --nodes=2
#SBATCH --ntasks=2
#SBATCH --ntasks-per-node=1
#SBATCH --cpus-per-task=4
#SBATCH --time=0-00:15:00

make

time ./life
# gprof life gmon.out > profile.txt

time mpiexec -n 4 -map-by node -bind-to none ./life_mpi
# gprof life_mpi gmon.out > profile-imp.txt

