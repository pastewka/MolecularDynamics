#!/usr/bin/env python3
import sys
import pandas as pd
import matplotlib.pyplot as plt


def plot_scaling(ax, file):
    data = pd.read_csv(file)
    data['strong_scaling'] = data['time (ms)'][0] / data['time (ms)']
    ax.plot(data['nproc'], data['strong_scaling'], marker='.', label=file.split(".")[0])

fig, ax = plt.subplots()

for file in sys.argv[1:]:
    plot_scaling(ax, file)

ax.axline((1, 1), (16, 16), ls='--', color='k')
# ax.axvline(20, ls='--', color='gray')
ax.grid()
ax.set_xscale('log', base=2)
ax.set_yscale('log', base=2)
ax.set(xlabel='$N_p$', ylabel='$T_1 / T_p$', aspect='equal')
ax.legend()

plt.show()
