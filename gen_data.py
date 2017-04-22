#!/usr/bin/env python

import random, sys

if len(sys.argv) <= 1:
	ntimes = 1000
	avg_size = 1000
	range_max = 100
else:
	ntimes = int(sys.argv[1])
	avg_size = int(sys.argv[2])
	range_max = int(sys.argv[3])

print("ntimes: ", ntimes, ", avg_size: ", avg_size, ", range_max: ", range_max)
range_min = 0


f = open('data_py_set','w')

for k in range(0,ntimes):
	size = random.sample(range(0,2*avg_size),1)
	array = sorted([random.randint(range_min,range_max-1) for i in range(0,size[0])])
	for i in range(0,size[0]):
		f.write(str( array[i] )+",")
	f.write("\n")
	if k%(ntimes/10) == 0:
		print( str(k/ntimes*100) + "% done...")

f.close()


