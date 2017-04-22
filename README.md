# ClusterSeparation

Compile any way you want or make sure you are in the build folder and run the pymake.py file (written in python 3). Use -std=c++11 or newer since the auto type is used several times.

This code tries to solve the problem stated below.

Following is the question: if something is not stated, make an assumption. 
It is related to clustering/classification. Perhaps, maybe you can help a little. 

A “cluster” is defined as a set of identifiers (integers). Each identifier (integer) inside a cluster is present only once.
A list of clusters is given as input. The objective is to output a new set of clusters that verifies the following properties:
1.       Each identifier appears only once: in other words, each cluster is exclusive to all others. The list of clusters at input do not guarantee this (one identifier can be present in N of the clusters, but at most once in each cluster)

2.       When two (or more) clusters overlap by more than some threshold (set to 90%), then they are all “merged” into one. The operation is actually a union. Property (1) as too be verified on the merged cluster too.

3.       When the overlap is below that threshold, the intersection will be moved into the largest of the group overlapping. Intersection can be complex (cluster one overlaps 2 & 3, cluster 3 overlapping 1, 4, 5, cluster 2 overlapping 1, 5, etc…)
 
Your objective is to solve this problem in the most efficient way possible. In the case of a CPU vs memory tradeoff, prefer improving CPU time; however, memory complexity shall be analyzed.
Assume having access to a powerful CPU: multi-threading is fair game.



The way I try to solve this is is to create bitstrings (of some native C++ type) from the sets of integers. For example:

int[] set1 = {0,3,4,9} ------> std::vector<uint8_t> v, where v[0] = 00011001, v[1] = 00000010 

Then for the operations of union and intersection, bitwise operations &, |, ~ can be used. The lset.h class implements this functionality with these operators overloaded.

The lsetproc.h class implements a processor class that takes a pointer to an array of lset objects, and processes it.

Currently I can see the following issues that need to be improved:

If there is a set like {0,19000}, a lot of unecessary zero bits will be declared in between.
Batch processing can be easily multi threaded I imagine, but I don't know how.
