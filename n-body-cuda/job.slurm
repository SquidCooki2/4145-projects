#!/bin/bash
#SBATCH --job-name=nbody_benchmark
#SBATCH --partition=GPU
#SBATCH --time=00:30:00
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --gres=gpu:1

module load cuda/12.4

make clean
make all

echo ""
echo "1000 particles (blocksize 128):"
./nbody_gpu 1000 0.01 100 1000 128
echo "10000 particles (blocksize 128):"
./nbody_gpu 10000 0.01 100 1000 128
echo "100000 particles (blocksize 128):"
./nbody_gpu 100000 0.01 100 1000 128

echo ""
echo "Solar System (blocksize 128):"
./nbody_gpu planet 200 1000 100 128 > solar_gpu.out