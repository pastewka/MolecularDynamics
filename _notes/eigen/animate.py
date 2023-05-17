#!/usr/bin/env python3

import numpy as np
import sys
import matplotlib.pyplot as plt
import matplotlib.animation as animation

data = np.loadtxt(sys.stdin)

x = np.linspace(-5, 5, data.shape[-1])
line, = plt.plot(x, data[0])
plt.xlabel('x')
plt.ylabel('u')


def animate(i):
    line.set_ydata(data[i])
    return line,


ani = animation.FuncAnimation(
    plt.gcf(), animate, interval=1, blit=True,
    frames=data.shape[0],
)

plt.show()
