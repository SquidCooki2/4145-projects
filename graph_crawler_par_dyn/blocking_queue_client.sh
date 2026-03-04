#!/bin/bash
#SBATCH --job-name=zahi-shaikh-merge-sort
#SBATCH --partition=Centaurus
#SBATCH --time=00:10:00
#SBATCH --mem=32G
make clean
make 
echo "Depth 2: Tom Hanks"
./blocking_queue_client "Tom Hanks" 2
echo "Depth 3: Tom Hanks"
./blocking_queue_client "Tom Hanks" 3
echo "Depth 2: Chris Evans"
./blocking_queue_client "Chris Evans" 2
echo "Depth 2: Morgan Freeman"
./blocking_queue_client "Morgan Freeman" 2