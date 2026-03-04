### How to run


```bash
sbatch blocking_queue_client.sh # Runs through different graph sizes and depths
```

Additionally, you can start at a node with: `./blocking_queue_client "[Actor or movie]" [depth]`

### Levels runtime:
```
Depth 2: Tom Hanks
Total unique nodes visited: 888
Runtime: 1045ms
Depth 3: Tom Hanks
Total unique nodes visited: 5827
Runtime: 18176ms
Depth 2: Chris Evans
Total unique nodes visited: 1132
Runtime: 1315ms
Depth 2: Morgan Freeman
Total unique nodes visited: 1164
Runtime: 1609ms
```

### Blocking queue runtime:
```
Depth 2: Tom Hanks
Total unique nodes visited: 888
Runtime: 974ms
Depth 3: Tom Hanks
Total unique nodes visited: 5827
Runtime: 18087ms
Depth 2: Chris Evans
Total unique nodes visited: 1132
Runtime: 1127ms
Depth 2: Morgan Freeman
Total unique nodes visited: 1164
Runtime: 1609ms
```

### Sequential runtime
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