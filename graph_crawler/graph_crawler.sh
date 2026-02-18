#!/bin/bash
#SBATCH --job-name=zahi-shaikh-merge-sort
#SBATCH --partition=Centaurus
#SBATCH --time=00:10:00
#SBATCH --mem=32G
make
echo "Depth 2:"
./graph_crawler "Tom Hanks" 2
echo "Depth 3:"
./graph_crawler "Tom Hanks" 3
echo "Depth 4:"
./graph_crawler "Tom Hanks" 4
echo "Depth 5:"
./graph_crawler "Tom Hanks" 5