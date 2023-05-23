---
layout: default
title:  "bwUniCluster"
parent: Notes
categories: notes
author : Lars Pastewka
nav_order: 6
---

# bwUniCluster

## Getting access

Accessing bwUniCluster is a multi-step procedure that is described in detail
here: <https://wiki.bwhpc.de/e/Registration/bwUniCluster>. Please
follow these steps as described. When being asked why you need access, please
mention the "HPC with C++ class at the University of Freiburg".

## Logging in and transferring data

You can the login to bwUniCluster with the `ssh` (*s*ecure *sh*ell) command:

```bash
ssh -Y <username>@uc2.scc.kit.edu
```

The option `-Y` tells ssh to forward X11 connections. This allows you to display plots while remotely logged in to bwUniCluster. This will not work if you are logging in from a Windows machine, unless you have set up a dedicated X11 server on your windows computer.

You can copy files to bwUniCluster with the `scp` (*s*ecure *c*o*p*y) command:

```bash
scp my_interesting_file <username>@uc2.scc.kit.edu:
```

Don't forget the colon `:` after the machine name when running the `scp` command. Note that you can use the same command to get the data back from bwUniCluster. For example, to copy the file `result.npy` from your home directory on bwUniCluster to the current directory simply execute (on your local machine):

```bash
scp <username>@uc2.scc.kit.edu:result.npy .
```

The dot `.` refers to your current directory. You can also specify full paths on either remote or local machines when executing `scp`.

## Setting up the software environment

After logging in to bwUniCluster at `uc2.scc.kit.edu`, you will need to set up your local environment to be able to compile and run parallel applications. The `module` command loads a specific environment:
* You can see all possible modules with `module avail`. Note that the list changes when you load a module as there are dependencies.
* You search for a specific module with `module spider`.
* You can list the presently load modules with `module list`.
* You can remove (unload) all presently load modules with `module purge`.

Please load the following modules (just execute these commands at the command line):

```bash
module load compiler/gnu devel/cmake mpi/openmpi
module list
```

## Compiling your code

You will first need to checkout your code on bwUniCluster. To compile your code on the command line, create a directory for building the code. This could be a directory named `build` in your source directory, e.g.:

```bash
mkdir build
cd build
```

Run `CMake` to configure your build:
```bash
cmake -DCMAKE_BUILD_TYPE=Release ..
```
The two dots `..` tell CMake where the sources are located. If you create a build directory somewhere else, you may need to provide the full path to your sources there.

Compile the code:
```bash
make
```

## Running simulations

bwUniCluster has extensive documentation that can be found here: <https://www.bwhpc-c5.de/wiki/index.php/Category:BwUniCluster>. Simulations are typically run as batch jobs. They have to be submitted through a batch or queueing system that takes care of assigning the actual hardware (compute node) to your job. A description of the queueing system can be found [here](https://wiki.bwhpc.de/wiki/index.php?title=BwUniCluster_2.0_Slurm_common_Features). Please make sure you understand the concept of a _partition_ described [here](https://wiki.bwhpc.de/wiki/index.php?title=BwUniCluster_2.0_Batch_Queues).

**IMPORTANT**: never *ever* run a simulation on the front/login node (i.e.
without submitting a job using the `sbatch` command, see below). Your access to
bwUniCluster could be revoked if you do this.

### Job scripts

To run you job, you need to write a job script. The job script is executed by the `bash` command which is the shell that you are using on Linux systems. (See the first tutorial [here](https://ilias.uni-freiburg.de/data/unifreiburg/lm_data/lm_1926798/Part_1_Unix_shell.html) for an introduction to the shell.) The job script specifies how many CPUs you require for your job, how to set up the software environment and how to execute your job. An example job script (you can use this one almost as is) looks like this:

```bash
#!/bin/bash -x
#SBATCH --nodes=2
#SBATCH --ntasks-per-node=40
#SBATCH --time=00:40:00
#SBATCH -J YAMD
#SBATCH --mem=6gb
#SBATCH --export=ALL
#SBATCH --partition=multiple

module load compiler/gnu/11.1
module load mpi/openmpi/4.1

echo "Running on ${SLURM_JOB_NUM_NODES} nodes with ${SLURM_JOB_CPUS_PER_NODE} cores each."
echo "Each node has ${SLURM_MEM_PER_NODE} of memory allocated to this job."
time mpirun ./yamd
```

Lines starting with a `#` are ignored by the shell. Lines starting with `#SBATCH` are ignored by the shell but are treated like command-line options to the `sbatch` command that you need to submit your job. You can see all options but running `man sbatch`.

The `--nodes` option specifies how many nodes you want to use. `--ntasks-per-node` specifies how many processors per node you need. The `--time` option specifies how long your job is allowed to run. You job is terminated if it exceeds this time! The `-J` option is simply the name of your job. It determines the names of output files generated by the batch system. Check the documentation linked above to understand the other options.

The `time` command in front of `mpirun` measures the execution time of the code and prints it to screen. Note that you do not tell `mpirun` how many cores to use. The batch system passes this information on automatically.

### Submitting jobs

Assume the filename of the above script is `run.job`, you can submit this script with

```bash
sbatch run.job
```

Everything that is included via `#SBATCH` in the script above can also be specified on the command line. For example, to change the number of cores to 320 you can issue the command:

```bash
sbatch --nodes=4 --ntasks-per-node=80 run.job
```

Your job may need to wait for resources and will not run immediately. You can inquire the status of your job with

```bash
squeue
```

Once it ran, you will find a file that start with `job_` and has the extension `.out`. This file contains the output of the simulation, i.e. the output that is normally written to screen when you run from the command line. Note that `bash -x` (the first line in the job submission script above) instructs bash to print every command that is executed onto the screen. This makes debugging (of the job script, not your simulation code) easier.

`squeue` shows the `JOBID` of you job. You cancel a job by executing

```bash
scancel <JOBID>
```

More information on a certain job can be obtained by

```bash
scontrol show job <JOBID>
```
