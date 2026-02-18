#!/bin/bash
#SBATCH --job-name=zahi-shaikh-merge-sort
#SBATCH --partition=Centaurus
#SBATCH --time=00:10:00
#SBATCH --mem=32G
make clean
make 
echo "Depth 2: Tom Hanks"
./graph_crawler "Tom Hanks" 2
echo "Depth 3: Tom Hanks"
./graph_crawler "Tom Hanks" 3
echo "Depth 2: Chris Evans"
./graph_crawler "Tom Hanks" 2
echo "Depth 2: Morgan Freeman"
./graph_crawler "Morgan Freeman" 2