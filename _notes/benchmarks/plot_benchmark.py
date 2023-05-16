#!/usr/bin/env python3

import sys
import pandas as pd
import seaborn as sns
import matplotlib.pyplot as plt

plt.rcParams.update({
    'font.size': 14,
    'lines.linewidth': 2,
})


data = pd.read_csv(sys.stdin)
data = data.query('name.str.contains("/")')
data['range'] = [int(x.split('/')[-1]) for x in data['name']]
data['Benchmark'] = [x.split('/')[0] for x in data['name']]

sns.relplot(data=data,
            x='range', y='cpu_time',
            style='Benchmark',
            hue='Benchmark', kind='line')

plt.xscale('log')
plt.yscale('log')

plt.xlabel('N')
plt.ylabel('CPU time [ns]')

plt.grid()
plt.gca().set_aspect('equal')
# plt.gcf().tight_layout()
plt.show()
