### How to run
```bash
sbatch graph_crawler.sh # Runs through different graph sizes and depths
```

Additionally, you can start at a node with: `./graph_crawler "[Actor or movie]" [depth]`

Example slurm out:
```
Depth 2: Tom Hanks
Total unique nodes visited: 888
Runtime: 6406ms
Depth 3: Tom Hanks
Total unique nodes visited: 5827
Runtime: 142967ms
Depth 2: Chris Evans
Total unique nodes visited: 1132
Runtime: 6732ms
Depth 2: Morgan Freeman
Total unique nodes visited: 1164
Runtime: 10608ms
```