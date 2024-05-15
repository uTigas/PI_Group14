import struct
import numpy as np
import matplotlib.pyplot as plt

type_map = { 'd' : 8 }

def read_signal(filename: str, type = 'd' ):
    with open(filename,"rb") as file:
        for i in range(7):
            file.readline()
        data = file.read()
        siz = len(data) // type_map[type]

        return np.array( struct.unpack( type * siz , data ) ) 

sim = read_signal("signals/aux13.sgn")
clone = read_signal("signals/aux12.sgn")

# substract the signals
diffs = sim - clone

# reshape sim and clone to 2xN
sim = sim.reshape(-1,2)
clone = clone.reshape(-1,2)

# plot in one figure 'histogram of diffs' and in another 'scatter plot sim and clone'
fig, ax = plt.subplots(2)
ax[0].hist(diffs, bins=100)

# limits of x axis
ax[0].set_xlim(-2.5,2.5)

# draw x = 0 and y = 0 lines
ax[1].axvline(0, color='black',alpha=0.5)
ax[1].axhline(0, color='black',alpha=0.5)

# alpha of scatter to 0.1 to see the density of points
ax[1].scatter(clone[:,0], clone[:,1], c='b', label='clone',alpha=0.1, s=2)
ax[1].scatter(sim[:,0], sim[:,1], c='r', label='sim')
ax[1].legend()

# get maxs of clone
maxs = np.max(clone, axis=0)
max = np.max(maxs) + 0.2

# set limits of x and y axis
ax[1].set_xlim(-max,max)
ax[1].set_ylim(-max,max)
plt.show()







    

