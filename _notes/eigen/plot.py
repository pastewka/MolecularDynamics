#!/usr/bin/env python3

import numpy as np
import sys
import matplotlib.pyplot as plt

data = np.loadtxt(sys.stdin)

x = np.linspace(-5, 5, data.shape[-1])

if data.ndim == 1:
    plt.plot(x, data)
    plt.xlabel('x')
    plt.ylabel('u')

elif data.ndim == 2:
    dt = 0.01

    plt.imshow(data, aspect='auto',
               extent=(x.min(), x.max(), 0, dt * data.shape[0]))
    plt.colorbar(label='u')

    plt.xlabel('x')
    plt.ylabel('t')
    plt.gcf().tight_layout()

plt.show()
