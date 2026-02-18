#!/bin/bash
#SBATCH --job-name=zahi-shaikh-merge-sort
#SBATCH --partition=Centaurus
#SBATCH --time=00:10:00
#SBATCH --mem=32G
make
./graph_crawler "Tom Hanks" 2