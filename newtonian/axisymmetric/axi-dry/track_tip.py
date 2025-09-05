# Author: Saumili Jana
# jsaumili@gmail.com
# Physics of Fluids

import numpy as np
import os
import sys


Oh = float(sys.argv[1])
k = float(sys.argv[2])
tsnap = float(sys.argv[3])
nGFS = 1000
name = "track_tip.dat"


if os.path.exists(name):
    print("File %s found! New data will be appended to the file" % name)
for ti in range(nGFS):
    t = tsnap * ti
    place = "intermediate/snapshot-%5.4f" % t
    if not os.path.exists(place):
        print("File %s not found!" % place)
    else:
        exe = "./track_tip %s %s %s %s" % (place, name, str(Oh), str(k))
        #os.system(exe)
        ret = os.system(exe)
        if ret != 0:
            print(f"track_tip ended with return code {ret}. Film reached the bottommost point. Stopping execution.")
            break
    print(("Done %d of %d" % (ti, nGFS)))