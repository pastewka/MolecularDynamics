#!/usr/bin/env bash

echo "nproc,time (ms)" > $1

for n in $(seq 1 40); do
    echo "$n,$(mpirun --oversubscribe -np $n ./wave_mpi)" >> $1
done
