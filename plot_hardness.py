import csv
import os
import matplotlib as mpl
mpl.use('Agg')
import matplotlib.pyplot as plt
import numpy as np
import sys

dataset_abbreviation = {
    "osm_cellids_200M_uint64" : "osm",
    "stovf_vote_id_200M_uint64" : "stack",
    "wise_all_sky_data_htm_200M_uint64" : "wise",
    "osm_history_node_200M_uint64" : "history",
    "books_200M_uint64" : "books",
    "covid_tweets_200M_uint64" : "covid",
    "libraries_io_repository_dependencies_200M_uint64" : "libio",
    "planet_features_osm_id_200M_uint64" : "planet",
    "biology_200M_uint64" : "genome",
    "fb_200M_uint64" : "fb",
}

global_hardness = {}
local_hardness = {}
point = {}

point = {'books': [0.00131302, 4.85e-07], 'osm': [0.003305575, 2.7475e-05], 'fb': [0.00527654, 8.435e-06], 'genome': [0.00645104, 7.13e-06], 'wise': [0.000395175, 1.91e-06], 'planet': [0.003067985, 1.157e-05], 'covid': [0.00040954, 4.25e-06], 'history': [0.00052734, 2.34e-06], 'libio': [0.00072904, 3.195e-06], 'stack': [8.9165e-05, 6.65e-07]}

origin_point = {"emails-validated-random-only-30-characters_clean.txt": [0.00726681, 5.72e-05], "git_repo_name_clean.txt": [0.00747368, 5.82132e-05], "url_data_set_clean.txt":[0.0116822, 0.00233645]}
x = []
y = []
n = []
for name in point.keys():
    n.append(name)
    x.append(point[name][0]) # local
    y.append(point[name][1]) # global

filepath = sys.argv[1]
name = filepath.split("/")[-1]
out_dir = sys.argv[2]

assert name in origin_point

with open(filepath, "r") as f:
    one = float(f.readline().split("= ")[-1])
    loc = float(f.readline().split("= ")[-1])
    glob = float(f.readline().split("= ")[-1])

fig = plt.figure(figsize=[10,10])
plt.xlabel("Local hardness")
plt.ylabel("Global hardness")
plt.title(name)

plt.scatter(x, y, color = 'green')
plt.scatter(loc, glob, color = 'red')

origin_loc = origin_point[name][0]
origin_glob = origin_point[name][1]
plt.scatter(origin_loc, origin_glob, color='red')
plt.annotate('origin', (origin_loc, origin_glob))

for data_name in point.keys():
    plt.annotate( data_name, (point[data_name][0], point[data_name][1]) )

all_x = list(x) + [loc, origin_loc]
all_y = list(y) + [glob, origin_glob]

plt.xlim( min(all_x) * 0.95, max(all_x) * 1.05 )
plt.ylim( min(all_y) * 0.95, max(all_y) * 1.05 )

print("sucess")
plt.savefig(out_dir + "/" + name.split("/")[-1] + ".png")
plt.close()
