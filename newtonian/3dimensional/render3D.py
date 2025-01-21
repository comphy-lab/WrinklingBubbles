# Author: Saumili Jana and Vatsal Sanjay
# jsaumili@gmail.com
# Physics of Fluids
# Date: 21-01-2025

import numpy as np
import subprocess as sp
import os
from multiprocessing import Pool, cpu_count
from functools import partial
nGFS = 100 

def gettingview(args):
    filename, name, t = args
    exe = ["./getview3D", filename, name, t]
    p = sp.Popen(exe, stdout=sp.PIPE, stderr=sp.PIPE)
    stdout, stderr = p.communicate()
    return name

def process_timestep(ti,folder):
    t = 0.01 * ti
    place = "intermediate/snapshot-%5.4f" % t
    name = "%s/%6.6d.png" %(folder, int(1e4*t))
    if not os.path.exists(place):
        print("File %s not found!" % place)
        return None
    if os.path.exists(name):
        print("Image %s found!" % name)
        return None
    t = str(round(t,2))
    return gettingview((place, name, t))

if __name__ == '__main__':
    folder = 'Video_view'  # output folder
    if not os.path.isdir(folder):
        os.makedirs(folder)

    # Create a pool of workers
    num_cores = 4
    pool = Pool(processes=num_cores)
    
    # Process time steps in parallel
    timesteps = range(nGFS)
    process_with_folder = partial(process_timestep, folder=folder)
    results = pool.map(process_with_folder, timesteps)
    
    # Close the pool
    pool.close()
    pool.join()
    
    # Count completed renders
    completed = sum(1 for r in results if r is not None)
    print(f"Completed {completed} renders out of {nGFS}")