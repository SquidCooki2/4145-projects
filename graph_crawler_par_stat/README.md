### How to run


```bash
sbatch level_client.sh # Runs through different graph sizes and depths
```

Additionally, you can start at a node with: `./level_client "[Actor or movie]" [depth]`

### Better alternatives
2/24/26 11:25pm - We can do better than a global lock on the `visited` and `next_level` sets. We can implement a bucketed hashmap for the visited set and have a mutex for each bucket so that there are less collisions. Buckets will make it way faster. We can also make it faster by adding to thread local vectors in place of `next_level` and combine afterwards. It'll be out of order but I think thats fine for this problem.

### Parallel runtime:
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