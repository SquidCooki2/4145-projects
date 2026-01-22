### How to run
```bash
sbatch merge_sort.sh # Runs through all 10^1 -> 10^9 sorting algorithms
```
Running this will output the results to a the slurm-out file in a decently nice format, which can then be used to plot. I used Microsoft Excel.

Additionally, I added different unrequired parameters to the command to allow for additional testing: `./merge_sort [number of items*] [min] [max]` *required