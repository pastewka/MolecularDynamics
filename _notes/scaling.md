---
layout: default
title:  "Parallel Scaling"
parent: Notes
categories: notes
author : Lucas Frérot
nav_order: 3
---

Here are the helper files for this section:

- [CMakeLists.txt](eigen/CMakeLists.txt)
- [wave_mpi.cpp](eigen/wave_mpi.cpp)

# Strong scaling

A typical scaling metric in HPC is *strong scaling*, or the reduction of
time-to-solution (TTS) when the number of processing units is increased, for a
fixed problem size. It is usually plotted as the *speedup* $$T\_1 / T\_p$$, where
$$ T\_1 $$ is the TTS for a single process, and $$ T\_p $$ for $$p$$ processes.
