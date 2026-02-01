### How to Run

Usage: (Enter non-zero positive args)
```
./nbody N dt iterations dump_iterations [input_file]
```

Additionally, if you want to use an input file that contains initial values (like solar.tsv) provide a file path.

### Runs
Run 0: Solar system with a dt of 200 and 5e6 steps
Run 1: 100 particles with a dt of 1 and 1e4 steps
Run 1: 1000 particles with a dt of 1 and 1e4 steps

Runtime data for each run can be found in `runtime.csv` and raw run data for plotting can be found in `output.tsv`s respectively