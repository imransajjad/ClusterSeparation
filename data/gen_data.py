#!/usr/bin/env python

import random

ntimes = 100
avg_size = 10
range_min = 0
range_max = 100

f = open('data_py_set','w')

for k in range(0,ntimes):
	size = random.sample(range(0,2*avg_size),1)
	array = sorted([random.randint(range_min,range_max-1) for i in range(0,size[0])])
	for i in range(0,size[0]):
		f.write(str( array[i] )+",")
	f.write("\n")

f.close()


