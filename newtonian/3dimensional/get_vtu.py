
import numpy as np
import os
import sys

nGFS = 100
#sos.system("mkdir itermediate_vtu")
for ti in range(nGFS):
    t = 0.010*0.1 * ti
    place = "intermediate/snapshot-%5.4f" % t
    if not os.path.exists(place):
        print("File %s not found!" % place)
    else:
        exe = "./get_vtu %s" % (place)
        os.system(exe)
    print(("Done %d of %d" % (ti, nGFS)))