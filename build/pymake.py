#!/usr/bin/env python

# makefile using python 3.x for mingw32 without msys.
from os import listdir, system, makedirs
from os.path import isfile, join, getmtime, splitext, exists
import time, subprocess


lib_mode = "static" # "static" , "dynamic" or "off"
lib_sys = "dll" # "dll" or "so"

print("using build/pymake.py")
include_string = "-I../include  -std=c++11 -pthread"  + " -Wall"
linker_string = "-L../lib "
la_string = ""
main_exec = "main"
exec_ext = ".exe"

def get_names(file_path):
	if not exists(file_path):
		makedirs(file_path)
	return [(splitext(f)[0], getmtime( file_path+"/" +f ) ) \
	for f in listdir(file_path) if isfile(join(file_path, f))]

def print_first(title, some_list,suffix):
	if len(some_list) > 0:
		buff = title +": "
		for i in some_list:
			buff += i[0]+suffix
		print(buff)

def my_exec(buff):
	error_flag = 0
	print(write_buff)
	stdoutdata = subprocess.getoutput(write_buff)
	print(stdoutdata)
	if "error" in stdoutdata:
		error_flag = -1
	return error_flag

MAX_LEN = 50;
src_list =get_names("../src")
include_list = get_names("../include")
lib_list = [(i[0].replace("lib","",MAX_LEN),i[1]) for i in get_names("../lib")]
build_list = get_names("../build")
exec_list = get_names("../bin")

# print( src_list)
# print( include_list)
# print( lib_list)
# print( build_list)



# get list of all .c files to build
omake_list = [f  for f in src_list]


# make a list of all the source files that need to be dynamically linked
somake_list = [];
if lib_mode == "static":
	somake_list = [f for f in omake_list if f[0] in [g[0] for g in include_list] ]
	# add them to the linker string
	# and remove them from the list of c files
	for i in somake_list:
		if i[0] not in la_string:
			la_string = "-l" + i[0] + " " + la_string
		omake_list.remove(i)
	# also remove them if they have more recent .so files
	somake_list = [f  for f in somake_list if (f[0] in [g[0] for g in lib_list] and f[1] > \
		lib_list[[h[0] for h in lib_list].index(f[0])][1]) or not(f[0] in [g[0] for g in lib_list]) ]


# then remove 
obuilds = [i for i in build_list if (i[0] in [j[0] for j in omake_list]) ]
if obuilds and max( [i[1] for i in include_list] + [j[1] for j in omake_list] ) \
		<= min( [j[1] for j in obuilds]):
	omake_list = []

# at this point, we should have a valid list to compile dynamically
# and a valid list of compilation objects

# print( obuilds)
# print( somake_list)
# print( omake_list)



error_flag = 0;
# compile libraries (if needed)
for i in somake_list:
	print_first("Compling libraries",somake_list,".a")
	write_buff = "g++ -c ../src/" + i[0] + ".* -o ../lib/lib" + i[0] + ".a " + include_string
	error_flag += my_exec(write_buff)
	if i[0] not in la_string:
		la_string = "-l" + i[0] + " " + la_string

# compile object files
for i in omake_list:
	write_buff = "g++ -c ../src/" + i[0] + ".* -o " + i[0] + ".o " + include_string
	error_flag += my_exec(write_buff)

# link everything and make executable
if error_flag == 0 and ((omake_list or somake_list) or not (main_exec in [i[0] for i in exec_list] )):
	write_buff = "g++ *.o -o ../bin/" + main_exec + exec_ext + " "+ linker_string + " "+ la_string
	error_flag += my_exec(write_buff)

time.sleep(0.1)
# run executable
if error_flag == 0:
	system("..\\bin\\" + main_exec + exec_ext)
