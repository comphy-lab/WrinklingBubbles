# Author: Saumili Jana
# jsaumili@gmail.com
# Date:July 29, 2025

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
#Oh = float(sys.argv[1])
#k = float(sys.argv[2])
filename = "track_tip.dat"
df1 = pd.read_csv(filename, delimiter=' ', index_col = False)
#print(df1) #debug

#Plot the film tip -locus
plt.plot(df1["y2"], df1["x2"], 'm--', linewidth = "1.5")
plt.plot(df1["y2"], df1["x2"], 'mo', mec = 'black',  markersize=8, markeredgewidth = 1, label="innermost point")
plt.plot(df1["y1"], df1["x1"], 'b--', linewidth = "1.5")
plt.plot(df1["y1"], df1["x1"], 'bo',mec = 'black',  markersize=8, markeredgewidth = 1, label="topmost point")
plt.plot(df1["y_tip"], df1["x_tip"], 'r--', linewidth = "1.5")
plt.plot(df1["y_tip"], df1["x_tip"], 'ro', mec = 'black',  markersize=8, markeredgewidth = 1, label="film tip")
plt.xlabel(r"$r/R$")
plt.ylabel(r'$Z/R$')
plt.legend(loc='upper left', bbox_to_anchor=(1, 1), frameon=True)
#plt.legend(loc = "best")
#plt.savefig('Zvsr_locus.png')
plt.savefig('Zvsr_locus.pdf', bbox_inches='tight')
#plt.show()
plt.clf()

#Plot x_tip(height-z) vs t
plt.plot(df1["t"], df1["x2"], 'm--', linewidth = "1.5")
plt.plot(df1["t"], df1["x2"], 'mo', mec = 'black',  markersize=8, markeredgewidth = 1, label="innermost point")
plt.plot(df1["t"], df1["x1"], 'b--', linewidth = "1.5")
plt.plot(df1["t"], df1["x1"], 'bo',mec = 'black',  markersize=8, markeredgewidth = 1, label="topmost point")
plt.plot(df1["t"], df1["x_tip"], 'r--', linewidth = "1.5")
plt.plot(df1["t"], df1["x_tip"], 'ro',mec = 'black',  markersize=8, markeredgewidth = 1, label="film tip")
plt.xlabel(r"$t/t_c$")
plt.ylabel(r'$Z/R$')
#plt.savefig('zvst.png')
plt.legend(loc='upper left', bbox_to_anchor=(1, 1), frameon=True)
#plt.ylim(None, 1)
plt.savefig('Zvst.pdf', bbox_inches='tight')
#plt.show()
plt.clf()

#Plot r_tip(spread) vs t
plt.plot(df1["t"], df1["y2"], 'm--', linewidth = "1.5")
plt.plot(df1["t"], df1["y2"], 'mo', mec = 'black',  markersize=8, markeredgewidth = 1, label="innermost point")
plt.plot(df1["t"], df1["y1"], 'b--', linewidth = "1.5")
plt.plot(df1["t"], df1["y1"], 'bo',mec = 'black',  markersize=8, markeredgewidth = 1, label="topmost point")
plt.plot(df1["t"], df1["y_tip"], 'r--', linewidth = "1.5")
plt.plot(df1["t"], df1["y_tip"], 'ro', mec = 'black',  markersize=8, markeredgewidth = 1,label="film tip")
plt.xlabel(r"$t/t_c$")
plt.ylabel(r'$r/R$')
#plt.savefig('rvst.png')
plt.legend(loc='upper left', bbox_to_anchor=(1, 1), frameon=True)
plt.ylim(None, 1)
plt.savefig('Zvst.pdf', bbox_inches='tight')
#plt.show()
plt.clf()

##get tip velocity;
df1["u1"] = 0.0
df1["v1"] = 0.0
df1["u2"] = 0.0
df1["v2"] = 0.0
df1["u_tip"] = 0.0
df1["v_tip"] = 0.0
for i in range(1,len(df1)-1, 1):
    df1.loc[i, "u1"] = 0.5*(((df1.loc[i, "x1"]-df1.loc[i-1, "x1"])/(df1.loc[i, "t"]-df1.loc[i-1, "t"])) + ((df1.loc[i+1, "x1"]-df1.loc[i, "x1"])/(df1.loc[i+1, "t"]-df1.loc[i, "t"])))
    df1.loc[i, "v1"] = 0.5*(((df1.loc[i, "y1"]-df1.loc[i-1, "y1"])/(df1.loc[i, "t"]-df1.loc[i-1, "t"])) + ((df1.loc[i+1, "y1"]-df1.loc[i, "y1"])/(df1.loc[i+1, "t"]-df1.loc[i, "t"])))
    df1.loc[i, "u2"] = 0.5*(((df1.loc[i, "x2"]-df1.loc[i-1, "x2"])/(df1.loc[i, "t"]-df1.loc[i-1, "t"])) + ((df1.loc[i+1, "x2"]-df1.loc[i, "x2"])/(df1.loc[i+1, "t"]-df1.loc[i, "t"])))
    df1.loc[i, "v2"] = 0.5*(((df1.loc[i, "y2"]-df1.loc[i-1, "y2"])/(df1.loc[i, "t"]-df1.loc[i-1, "t"])) + ((df1.loc[i+1, "y2"]-df1.loc[i, "y2"])/(df1.loc[i+1, "t"]-df1.loc[i, "t"])))
    df1.loc[i, "u_tip"] = 0.5*(((df1.loc[i, "x_tip"]-df1.loc[i-1, "x_tip"])/(df1.loc[i, "t"]-df1.loc[i-1, "t"])) + ((df1.loc[i+1, "x_tip"]-df1.loc[i, "x_tip"])/(df1.loc[i+1, "t"]-df1.loc[i, "t"])))
    df1.loc[i, "v_tip"] = 0.5*(((df1.loc[i, "y_tip"]-df1.loc[i-1, "y_tip"])/(df1.loc[i, "t"]-df1.loc[i-1, "t"])) + ((df1.loc[i+1, "y_tip"]-df1.loc[i, "y_tip"])/(df1.loc[i+1, "t"]-df1.loc[i, "t"])))

df1["v1_mag"] =  np.sqrt(df1["u1"]**2 + df1["v1"]**2)
df1["v2_mag"] =  np.sqrt(df1["u2"]**2 + df1["v2"]**2)
df1["vTip_mag"] =  np.sqrt(df1["u_tip"]**2 + df1["v_tip"]**2)

#print(df1)#debug
##Plot velocities-u comp
df2 = df1.iloc[:-1]
plt.plot(df2["t"], df2["u2"], 'm--', linewidth = "1.5")
plt.plot(df2["t"], df2["u2"], 'mo', mec = 'black',  markersize=8, markeredgewidth = 1,label="innermost point")
plt.plot(df2["t"], df2["u1"], 'b--', linewidth = "1.5")
plt.plot(df2["t"], df2["u1"], 'bo',mec = 'black',  markersize=8, markeredgewidth = 1, label="topmost point")
plt.plot(df2["t"], df2["u_tip"], 'r--', linewidth = "1.5")
plt.plot(df2["t"], df2["u_tip"], 'ro',mec = 'black',  markersize=8, markeredgewidth = 1, label="film tip")
plt.xlabel(r"$t/t_c$")
plt.ylabel(r'$u$')
#plt.savefig('uvst.png')
plt.legend(loc='upper left', bbox_to_anchor=(1, 1), frameon=True)
plt.savefig('uvst.pdf', bbox_inches='tight')
#plt.show()
plt.clf()

##Plot velocities-v comp
plt.plot(df2["t"], df2["v2"], 'm--', linewidth = "1.5")
plt.plot(df2["t"], df2["v2"], 'mo',mec = 'black',  markersize=8, markeredgewidth = 1, label="innermost point")
plt.plot(df2["t"], df2["v1"], 'b--', linewidth = "1.5")
plt.plot(df2["t"], df2["v1"], 'bo',mec = 'black',  markersize=8, markeredgewidth = 1, label="topmost point")
plt.plot(df2["t"], df2["v_tip"], 'r--', linewidth = "1.5")
plt.plot(df2["t"], df2["v_tip"], 'ro',mec = 'black',  markersize=8, markeredgewidth = 1, label="film tip")
plt.xlabel(r"$t/t_c$")
plt.ylabel(r'$v$')
#plt.savefig('vvst.png')
plt.legend(loc='upper left', bbox_to_anchor=(1, 1), frameon=True)
plt.savefig('vvst.pdf', bbox_inches='tight')
#plt.show()
plt.clf()

##Plot velocities-vmagnitudes
plt.plot(df2["t"], df2["v2_mag"], 'm--', linewidth = "1.5")
plt.plot(df2["t"], df2["v2_mag"], 'mo', mec = 'black',  markersize=8, markeredgewidth = 1,label="innermost point")
plt.plot(df2["t"], df2["v1_mag"], 'b--', linewidth = "1.5")
plt.plot(df2["t"], df2["v1_mag"], 'bo',mec = 'black',  markersize=8, markeredgewidth = 1, label="topmost point")
plt.plot(df2["t"], df2["vTip_mag"], 'r--', linewidth = "1.5")
plt.plot(df2["t"], df2["vTip_mag"], 'ro', mec = 'black',  markersize=8, markeredgewidth = 1,label="film tip")
plt.xlabel(r"$t/t_c$")
plt.ylabel(r'$|v|$')
#plt.savefig('vmagvst.png')
plt.legend(loc='upper left', bbox_to_anchor=(1, 1), frameon=True)
#plt.ylim(None, 1)
plt.savefig('vmagvst.pdf', bbox_inches='tight')
#plt.show()
plt.clf()