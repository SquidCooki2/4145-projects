### How to run


```bash
sbatch level_client.sh # Runs through different graph sizes and depths
```

Additionally, you can start at a node with: `./level_client "[Actor or movie]" [depth]`

### Better alternatives
2/24/26 11:25pm - We can do better than a global lock on the `visited` and `next_level` sets. We can implement a bucketed hashmap for the visited set and have a mutex for each bucket so that there are less collisions. I'm pretty sure that this current solution will be pretty close if not slower than the sequential solution. Buckets will make it way faster. We can also make it faster by adding to thread local vectors in place of `next_level` and combine afterwards. It'll be out of order but I think thats fine for this problem.