#!/bin/bash
#SBATCH --job-name=zahi-shaikh-merge-sort
#SBATCH --partition=Centaurus
#SBATCH --time=00:10:00
#SBATCH --mem=32G
make
echo "DT: 200, STEPS: 5E6"
./n-body 1 200 5000000 10000 solar.tsv
echo "PARTICLES: 100, DT: 1, STEPS: 10000"
./n-body 100 1 10000 100
echo "PARTICLES: 1000, DT: 1, STEPS: 10000"
./n-body 1000 1 10000 100
exit 0