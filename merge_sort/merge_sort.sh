#!/bin/bash
#SBATCH --job-name=zahi-shaikh-merge-sort
#SBATCH --partition=Centaurus
#SBATCH --time=00:01:00
make
echo "10^1:"
./merge_sort 10
echo "10^2:"
./merge_sort 100
echo "10^3:"
./merge_sort 1000
echo "10^4:"
./merge_sort 10000
echo "10^5:"
./merge_sort 100000
echo "10^6:"
./merge_sort 1000000
echo "10^7:"
./merge_sort 10000000
echo "10^8:"
./merge_sort 100000000
echo "10^9:"
./merge_sort 1000000000