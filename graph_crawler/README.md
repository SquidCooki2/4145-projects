### How to run
```bash
sbatch graph_crawler.sh # Runs through different graph sizes and depths
```

Additionally, you can start at a node with: `./graph_crawler "[Actor or movie]" [depth]`

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