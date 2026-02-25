### How to run
```bash
sbatch merge_sort.sh # Runs through all 10^1 -> 10^9 sorting algorithms
```
Running this will output the results to a the slurm-out file in a decently nice format, which can then be used to plot. I used Microsoft Excel.

Additionally, I added different unrequired parameters to the command to allow for additional testing: `./merge_sort [number of items*] [min] [max]` *required

Example slurm out:
```
rm -f graph_crawler
g++ graph_crawler.cpp -std=c++17 -Wall -Irapidjson/include -lcurl -o graph_crawler
Depth 2: Tom Hanks
Runtime: 6488389μs
Depth 2: Chris Evans
Runtime: 6391928μs
Depth 2: Morgan Freeman
Runtime: 10557828μs
```