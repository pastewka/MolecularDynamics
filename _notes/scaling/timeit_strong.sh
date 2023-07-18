#!/usr/bin/env bash

echo "nproc,time (ms)" > $1

for n in $(seq 1 12); do
    echo "$n,$(mpirun -np $n ./wave_mpi $2)" >> $1
done
