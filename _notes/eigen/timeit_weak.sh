#!/usr/bin/env bash

echo "nproc,time (ms)" > $1

# Points per processor
work=10000

for n in $(seq 1 40); do
    echo "$n,$(mpirun --oversubscribe -np $n ./wave_mpi $((n * work)))" >> $1
done
