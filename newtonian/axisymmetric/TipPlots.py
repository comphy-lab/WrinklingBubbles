# Author: Saumili Jana
# jsaumili@gmail.com
# Date:Apr 18, 2024


#import libraries
import numpy as np
import sys
import matplotlib.pyplot as plt
from matplotlib.colors import ListedColormap
import pandas as pd
FigureWidth = 3.375
factor = 1.5
# Set global parameters
params = {
          'lines.linewidth': 3,
          'axes.labelsize': int(13*factor),
          'legend.fontsize': int(10*factor),
          'xtick.labelsize': int(10*factor),
          'ytick.labelsize': int(10*factor),
          'text.usetex': True,
          'font.family': 'serif'}
plt.rcParams.update(params)
plt.rcParams['text.latex.preamble'] = r"\usepackage{amsmath}"
#take imputs relevant parameters
Oh = float(sys.argv[1])
k = float(sys.argv[2])
tmax = float(sys.argv[3])
#read data and preprocess
filename = "film_tip.dat"
df1 = pd.read_csv(filename, delimiter=' ', index_col = False)
#df1['t'] = df1['t'].astype(float)
df_f = df1[df1['t']<tmax]
#print(df_f["x_tip"])


#Plot x_tip(height-z) vs t
plt.plot(df_f["t"], df_f["x_tip"], 'b--', linewidth = "1.5")
plt.plot(df_f["t"], df_f["x_tip"], 'bo')
plt.xlabel(r"$t/t_c$")
plt.ylabel(r'$Z/R$')
plt.savefig('zvst.png')
plt.ylim(None, 1)
plt.savefig('Zvst.pdf', bbox_inches='tight')
#plt.show()
plt.clf()

#Plot r_tip(spread) vs t
plt.plot(df_f["t"], df_f["y_tip"], 'r--', linewidth = "1.5")
plt.plot(df_f["t"], df_f["y_tip"], 'ro')
plt.xlabel(r"$t/t_c$")
plt.ylabel(r'$r/R$')
#plt.ylim(None, 1)
plt.savefig('rvst.png')
plt.savefig('rvst.pdf', bbox_inches='tight')
#plt.show()
plt.clf()

#Plot the film tip
plt.plot(df_f["y_tip"], df_f["x_tip"], 'm--', linewidth = "1.5")
plt.plot(df_f["y_tip"], df_f["x_tip"], 'mo')
plt.xlabel(r"$r/R$")
plt.ylabel(r'$Z/R$')
plt.savefig('Zvsr_locus.png')
#plt.ylim(None, 1)
plt.savefig('Zvsr_locus.pdf', bbox_inches='tight')
#plt.show()
plt.clf()

#Plot tip velocities with time
v_mag = np.sqrt(df_f["uTip"]**2 + df_f["vTip"]**2)
df_final = df_f.copy()
df_final.loc[:, 'v_mag'] = v_mag

df_final["vTip"] = np.abs(df_f["vTip"])
df_final["uTip"] = np.abs(df_f["uTip"])
#Plot the film tip
plt.plot(df_f["t"], df_final["uTip"], 'r--', linewidth = "2", label = r'$v_z$')
#plt.plot(df_f["t"], df_f["uTip"], 'r.', label = r'$v_z$')
plt.plot(df_f["t"], df_final["vTip"], 'b--', linewidth = "2", label = r'$v_r$' )
#plt.plot(df_f["t"], df_final["vTip"], 'bo', label = r'$v_r$')
plt.plot(df_f["t"], df_final["v_mag"], 'k-', linewidth = "2", label = r'$|v|$')
#plt.plot(df_f["t"], df_final["v_mag"], 'ko', label = r'$|V|$')
plt.ylabel(r"$|V|/\sqrt{\frac{\gamma}{\rho_l R}}$")
plt.xlabel(r'$t$')
plt.legend()
plt.savefig('vel.png')
plt.savefig('vel.pdf', bbox_inches='tight')
#plt.show()
plt.clf()

#get and write av velocity
v_avg = df_final["v_mag"].iloc[1:-1].mean()
#print(v_avg)
f = open("../vel_oh_k.dat", "a")
f.write(str(Oh)+" "+str(k)+" "+str(v_avg)+'\n')
f.close()

#Plot x_tip(height-z) vs t_sacled
df_final["scaled_t"] = df_f["t"]*v_avg

plt.plot(df_final["scaled_t"], df_final["x_tip"], 'b--', linewidth = "1.5")
plt.plot(df_final["scaled_t"], df_final["x_tip"], 'bo')
plt.xlabel(r"$t|V|/R$")
plt.ylabel(r'$Z/R$')
plt.savefig('zvst_scaled.png')
plt.ylim(None, 1)
plt.savefig('Zvst_scaled.pdf', bbox_inches='tight')
plt.clf()
#plt.show()
#write the final dataframe to csv for future use
df_final.to_csv('outputFilmTip.dat', sep=' ', index=False)