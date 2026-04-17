#!/bin/bash
#SBATCH --job-name=zahi-shaikh_merge_sort
#SBATCH --output=merge_sort.out
#SBATCH --partition=Centaurus
#SBATCH --time=00:10:00
#SBATCH --mem=32G

SIZES=(10 100 1000 10000 100000)
THREADS=(1 2 4 8)

for n in "${SIZES[@]}"; do
    echo "Array Size (N = $n)"
    echo "--------------------------------------------------"
    
    for t in "${THREADS[@]}"; do
        export OMP_NUM_THREADS=$t
        
        echo -n "Threads: $t | "
        
        ./merge_sort $n $t
    done
    
    echo "" 
done