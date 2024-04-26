#!/bin/bash
#SBATCH --job-name=life
#SBATCH --partition=cosc    # running on 'cosc'
#SBATCH --nodes=2
#SBATCH --ntasks=2
#SBATCH --ntasks-per-node=1
#SBATCH --cpus-per-task=4
#SBATCH --time=0-00:15:00

module load mpi

make

time mpiexec -n 4 -map-by node -bind-to none ./life2

# gprof life2 gmon.out > profile-imp.txt
