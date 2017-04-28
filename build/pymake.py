#!/usr/bin/env python

# makefile using python 3.x for mingw32 without msys.
from os import listdir, system, makedirs
from os.path import isfile, join, getmtime, splitext, exists
import time, subprocess

print("using build/pymake.py")
include_string = "-I../include  -std=c++11 -pthread"   + " -Wall"
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



# delete old main.exe if it is there
# if main_exec in [i[0] for i in exec_list]:
# 	system("del "+ "..\\bin\\" + main_exec + exec_ext)


# make a list of c files that are more recent than their .o files,
# or the ones whos .o files don't exist
#cmake_list = [f  for f in src_list if (f[0] in [g[0] for g in build_list]\
# and f[1] >	build_list[[h[0] for h in build_list].index(f[0])][1])\
#  or not(f[0] in [g[0] for g in build_list]) ] 
cmake_list = [f  for f in src_list]

#cmake_list = src_list[ src_list[:][1] == build_list[:][1] and src_list[:][2] > build_list[:][2]]

# make a list of all the .c files that have headers (libraries)
libamake_list = [f for f in cmake_list if f[0] in [g[0] for g in include_list] ]

# and remove them from the list of c files
for i in libamake_list:
	if i[0] not in la_string:
		la_string = "-l" + i[0] + " " + la_string
	cmake_list.remove(i)

# also remove them if they have more recent .a files
libamake_list = [f  for f in libamake_list if (f[0] in [g[0] for g in lib_list] and f[1] > \
	lib_list[[h[0] for h in lib_list].index(f[0])][1]) or not(f[0] in [g[0] for g in lib_list]) ] 

print_first("Building libraries",libamake_list,".a")
print_first("Building binaries",cmake_list,".o")

# compile libraries for dynamic linking
for i in libamake_list:
	write_buff = "g++ -c ../src/" + i[0] + ".* -o ../lib/lib" + i[0] + ".a " + include_string
	# write_buff = "g++ -c ../lib/" + i[0] + ".* -o " + i[0] + ".o " + include_string
	print(write_buff)
	result_lib = subprocess.run(write_buff, stdout=subprocess.PIPE)
	print(result_lib.stdout.decode('utf-8'))
	if i[0] not in la_string:
		la_string = "-l" + i[0] + " " + la_string

# compile object files
for i in cmake_list:
	write_buff = "g++ -c ../src/" + i[0] + ".* -o " + i[0] + ".o " + include_string
	print(write_buff)
	result_compile = subprocess.run(write_buff, stdout=subprocess.PIPE)
	print(result_compile.stdout.decode('utf-8'))

# link everything and make executable
if len(cmake_list)>0 or len(libamake_list)>0 or (main_exec not in [i[0] for i in exec_list]):
	if len(cmake_list)>0 or len(libamake_list)>0:
		system("del ..\\bin\\" + main_exec + exec_ext)
	write_buff = "g++ *.o -o ../bin/" + main_exec + exec_ext + " "+ linker_string + " "+ la_string
	print(write_buff)
	result_link = subprocess.run(write_buff, stdout=subprocess.PIPE)
	print(result_link.stdout.decode('utf-8'))

time.sleep(0.1)

# run executable
print(result_compile.stdout.decode('utf-8'))
if not result_link.stdout.decode('utf-8') and not result_link.stdout.decode('utf-8'):
	system("..\\bin\\" + main_exec + exec_ext)
