import numpy as np
import matplotlib.pyplot as plt
import matplotlib.lines as mlines
import glob
from scipy.interpolate import interp1d
from scipy.optimize import root

l1 = 32e3
l1l2 = l1 + 256e3

sorts = ['insertion', 'merge']
#linestyles for the two sorting algorithms
slstyles = {'insertion':'-', 'merge':(0,(3,1,1,1,1,1))}
types = ['char', 'int', 'double']
tcolors = {'char':'C0','int':'C1','double':'C2'}

files = glob.glob("MICRank*.csv")

def find_turnover(psize, mdata, idata):
  """Use linear interpolants to find the crossover point.
  
  Parameters
  ----------
  psize : np.array
    The byte sizes at which timing data was collected. 
  mdata : np.array
    Timing data given by using merge sort. 
  idata : np.array
    Timing data given by using insertion sort

  Returns
  -------
  ncross : float
    The approximate problem size at which merge sort and insertion sort have the 
    same execution time.
  """
  f = interp1d(psize, mdata-idata)
  sol = root(f, 1000)
  if sol.success:
    ncross = sol.x
  else:
    ncross = sol.x
  return ncross



data_dict = {}
pdata = {}
psizes = [2**i for i in range(1,22)]

for s in sorts:
  data_dict[s] = {}
  pdata[s] = {}
  for t in types:
    data_dict[s][t] = {}
    pdata[s][t] = {}
    pdata[s][t]['mean'] = []
    pdata[s][t]['std'] = []
    for ps in psizes:
      data_dict[s][t][ps] = []

#get data from all files
for file in files:
  fobj = open(file, 'r')
  lines = fobj.readlines()
  for i in range(1, len(lines)):
    dat = lines[i].split(',')
    data_dict[dat[0]][dat[1]][int(dat[2])].append(float(dat[4]))


#convert all lists to arrays
for s in sorts:
  for t in types:
    for ps in psizes:
      data_dict[s][t][ps] = np.array(data_dict[s][t][ps])

#get means and standard deviations
for s in sorts:
  for t in types:
    for ps in psizes:
      pdata[s][t]['mean'].append(np.mean(data_dict[s][t][ps]))
      pdata[s][t]['std'].append(np.std(data_dict[s][t][ps]))
    pdata[s][t]['mean'] = np.array(pdata[s][t]['mean'])
    pdata[s][t]['std'] = np.array(pdata[s][t]['std'])

psizes = np.array(psizes)
#generate problem complexity in terms of bytes
cbytes = psizes
ibytes = psizes*4
dbytes = psizes*8
bsizes = {'char':cbytes, 'int':ibytes, 'double':dbytes}

#print out the turnover times
for t in types:
  ncross = find_turnover(bsizes[t], pdata['merge'][t]['mean'], pdata['insertion'][t]['mean'])
  print("Turnover for type %s occurs at %e" % (t, ncross))

slines = []
tlines = []

n2_ref = cbytes[:10]**2 + 1e-3
nlogn_ref = cbytes[:10]*np.log10(cbytes[:10]) + 1e-3

for s in sorts:
  slines.append(mlines.Line2D([], [], color='k', linestyle=slstyles[s], label=s))

for t in types:
  tlines.append(mlines.Line2D([], [], color=tcolors[t], label=t))

plt.figure(figsize=(8,5))
for s in sorts:
  for t in types:
    plt.errorbar(bsizes[t], pdata[s][t]['mean'], yerr=pdata[s][t]['std'],
                  color=tcolors[t], linestyle=slstyles[s])
plt.plot(cbytes[:10], n2_ref, color='k')
plt.text(1e2, 10**3.2, r'$\propto n^{2}$', rotation=40.0)
plt.plot(cbytes[:10], nlogn_ref, color='k')
plt.text(1e2, 10**1.5, r'$\propto n\log n$', rotation=25.0)
plt.axvline(x=l1, label='L1 Cache', linestyle='-.', color='k')
plt.axvline(x=l1l2, label='L1&L2 Cache', linestyle=':', color='k')
legend1 = plt.legend(loc='upper left')
ax = plt.gca().add_artist(legend1)
legend2 = plt.legend(handles=slines, loc='upper right')
ax = plt.gca().add_artist(legend2)
legend3 = plt.legend(handles=tlines, loc='lower right')
plt.xlabel('Problem Size (bytes)')
plt.ylabel('Runtime (seconds)')
plt.xscale('log')
plt.yscale('log')
plt.savefig('MIComp.png')





