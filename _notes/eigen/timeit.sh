#!/usr/bin/env fish

echo "nproc,time (ms)" > $argv[1]

for n in (seq 1 16)
    echo "$n,$(mpirun --oversubscribe -np $n ./wave_mpi)" >> $argv[1]
end
