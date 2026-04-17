#!/bin/bash
#SBATCH --job-name=zahi-shaikh
#SBATCH --partition=Centaurus
#SBATCH --time=00:10:00
#SBATCH --mem=32G
make clean
make 
echo "Depth 2: Tom Hanks"
./level_client "Tom Hanks" 2
echo "Depth 3: Tom Hanks"
./level_client "Tom Hanks" 3
echo "Depth 2: Chris Evans"
./level_client "Chris Evans" 2
echo "Depth 2: Morgan Freeman"
./level_client "Morgan Freeman" 2