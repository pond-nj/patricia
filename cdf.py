from os import listdir
import numpy as np
import matplotlib.pyplot as plt
from os.path import isfile, join
import sys

filepath = sys.argv[1]
out_dir = sys.argv[2]

# unfiltered_filename = [f for f in listdir(input_dir) if isfile(join(input_dir, f))]

def read_file(f):
    x = []
    for line in f.readlines():
        k = line.strip()
        y = 0
        y = int(k)
        if y < 0:
            print(y)
        x.append(y)
    
    return np.array(x)
    

try:
    filename = filepath.split("/")[-1]
    # print("start: " + filename)
    fig, axs = plt.subplots(1, 3, figsize=[15,5])
    f = open(filepath, 'r')
    x = read_file(f)

    # print(x)

    

    y = x
    print(y.shape)
    resolution = 0.1
    axs[0].hist(y, bins=int(y.shape[0]*resolution), cumulative=True, density=True, label='CDF', histtype='step',color='k')
    axs[0].set_title(filename)
    print("done axs[0]")

    mid = int(y.shape[0]/2)
    # if(len(y) > 1000000):
    #     # +- 100,000
    #     y_zoom = y[np.arange(mid-1e5, mid+1e5, 1, dtype=int)]
    #     axs[1].hist(y_zoom, bins=y_zoom.shape[0], cumulative=True, density=True, label='CDF', histtype='step',color='k')
    #     axs[1].set_title(filename + " zoom 100k")
    # else:
    #     # +- 50,000
    #     y_zoom = y[np.arange(mid-5e4, mid+5e4, 1, dtype=int)]
    #     axs[1].hist(y_zoom, bins=y_zoom.shape[0], cumulative=True, density=True, label='CDF', histtype='step',color='k')
    #     axs[1].set_title(filename + " zoom 50k")
    # print("done axs[1]")

    # +- 1000
    y_zoom = y[int(mid-1000): int(mid+1000)]
    print(y_zoom.shape[0])
    axs[2].hist(y_zoom, bins=y_zoom.shape[0], cumulative=True, density=True, label='CDF', histtype='step',color='k')
    axs[2].set_title(filename + " zoom 1k")
    print("done axs[2]")

    plt.savefig(out_dir + "/" + filename + '.png')
    plt.close()
except Exception as e:
    raise e
