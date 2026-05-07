Usage:
```
make clean
make all
./nbody_gpu <input> <dt> <nbstep> <printevery> <blocksize>
```


Slurm output:
```
1000 particles (blocksize 128):
Simulation took 0.298188 seconds
10000 particles (blocksize 128):
Simulation took 4.57534 seconds
100000 particles (blocksize 128):
Simulation took 258.221 seconds

Solar System (blocksize 128):
```