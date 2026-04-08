#!/bin/bash
#SBATCH --job-name=zahi-shaikh-n-body-par
#SBATCH --partition=Centaurus
#SBATCH --mem=32G
#SBATCH --time=00:10:00

make clean && make

echo ""
echo "=== Solar system (dt=200, nbstep=5000000) ==="
for t in 1 2 4 8 16; do
  ./nbody planet 200 5000000 5000000 $t
done

echo ""
echo "=== Random 100 particles (dt=1, nbstep=10000) ==="
for t in 1 2 4 8 16; do
  ./nbody 100 1 10000 10000 $t
done

echo ""
echo "=== Random 1000 particles (dt=1, nbstep=10000) ==="
for t in 1 2 4 8 16; do
  ./nbody 1000 1 10000 10000 $t
done
echo ""