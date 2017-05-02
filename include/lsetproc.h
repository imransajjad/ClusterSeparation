#ifndef LSETPROC_H
#define LSETPROC_H

#include <iostream>
#include <algorithm>
#include <list>
#include <vector>
#include <bitset>
#include <thread>

#include "externs.h"

#define thres 0.9
#define thread_limit 25


class lsetproc
{
	// this data type processes a list of lset objects in place

	std::vector<lset> sets; // vector of lsets
	int batch_size; // for future use

	public:
		lsetproc(std::vector<lset> sets_in, int batch_size_in)
		{
			sets = sets_in;
			batch_size = batch_size_in;
		}

		
		void print_all(int mode)
		{
			// prints (some of) the sets as human readable
			// modes:
			// 0, print all nonzero
			// 1, print info only
			// 2, print all nonzero with intersection (very inefficient)
			// 3, print info only with intersection (very inefficient)
			// 4, print all
			int count = 0;
			int set_count = 0;
			int intersection_count = -1;
			for (size_t i = 0; i != sets.size(); ++i)
				if ( (sets[i].size() > 0) || (mode==4) )
				{
					if (mode%2 == 0) sets.at(i).print(2);
					count += sets.at(i).num_ones();
					set_count++;
				}
			if ( (mode == 2) || (mode == 3)) intersection_count = intersection_num();
			std::cout << "Total number of Sets: " << set_count << " containing " <<
			count << " elements with " << intersection_count << " intersections."<<std::endl;
		}

		lset union_all()
		{
			// return an lset which contains the union of all elements of set
			lset b;
			for (size_t i = 0; i != sets.size(); ++i)
				if (sets[i].size() > 0)
					b |= sets.at(i);
			return b;
		}

		lset intersection_all()
		{
			// return lset common to all elements of sets
			lset b;
			for (size_t i = 0; i != sets.size(); ++i)
				if (sets[i].size() > 0)
					b &= sets[i];
			return b;
		}

		void process_multi()
		{
			// generate a numbered subset of the sets and pass it to
			// the private processor function
			std::vector<std::thread> my_threads;
			std::list<int> setlist;
			int numthreads = 0;
			int maxthreads = 0;
			
			//batch_size = sets.size()/100;

			do
			{
				my_threads.clear();
				// if(my_threads.size() !=0) old_threads = my_threads.size();
				// my_threads.clear();
				// std::cout << "new pass\n";
				for (size_t i = 0; i != sets.size(); ++i)
				{
					if (sets[i].size() >0)
						setlist.push_back(i);
					if ( ( (setlist.size()%batch_size == 0) && (setlist.size() > 0) ) 
						|| (i+1 == sets.size()) )
					{
						std::cout << setlist.size() << ", " << i << std::endl;
						numthreads++;
						// std::cout << "pushing thread "<< my_threads.size() <<"\n";
						my_threads.push_back( std::thread( [&,this]
							(std::list<int> setlist_in)
						{ process_subset_thres(setlist_in);
							process_subset_inters_find(setlist_in); }, setlist ) );
						setlist.clear();
					}
				}

				for (auto& t: my_threads)
					t.join();
				if (maxthreads == 0) maxthreads = numthreads;
				batch_size = 2*batch_size;
			} while (my_threads.size() > 1);

			std:: cout << "numthreads: " << numthreads << ", maxthreads: " << maxthreads << "\n";
			//process();
		}

		void process()
		{
			// generate a numbered subset of the sets and pass it to
			// the private processor function
			std::list<int> setlist;

			for (size_t i = 0; i != sets.size(); ++i)
				if (sets[i].size() >0)
					setlist.push_back(i);
			process_subset_thres(setlist);
			process_subset_inters_find(setlist);
		}

		void cleanup()
		{
			// is inefficient but generally,
			// calling this function should not be required
			std::cout << "starting cleanup...\n";
			int count = 0;
			for (auto  i = sets.begin(); i != sets.end(); ++i)
				if ((*i).size() <= 0)
				{	sets.erase(i); i--;	count++;}
			std::cout<< "removed " << count << " elements\n";

		}

		void cleanup_fast()
		{
			// somewhat faster cleanup
			std::vector<lset> new_sets;
			int count = 0;
			std::cout << "starting fast cleanup...\n";
			for (auto i= sets.begin(); i != sets.end(); ++i)
				if ( (*i).size() > 0 )
					{ new_sets.push_back(*i); count++;}
			sets.clear();
			sets = new_sets;
			std::cout<< "replaced " << count << " elements\n";
		}

	private:

		int max_size()
		{
			// get the 'size' of the largest element in sets.
			// should not have to use this function ever...
			int size = 0;
			for (size_t i = 0; i != sets.size(); ++i)
				if (sets[i].size() > size)
					size = sets.at(i).size();
			return size;
		}

		int intersection_num()
		{
			// total number of common elements across all sets
			// WARNING: VERY SLOW!! do not use this function unless absolutely required
			int count = 0;
			for (size_t i = 0; i != sets.size(); ++i)
				for (size_t j =i+1; j != sets.size(); ++j)
					if ( (sets[i].size() > 0) && (sets[j].size() > 0) )
						count += (sets[i] & sets[j]).num_ones();
			return count;
		}

		int intersection_num_multi()
		{
			// total number of common elements across all sets
			// WARNING: VERY SLOW!! do not use this function unless absolutely required
			int count = 0;
			for (size_t i = 0; i != sets.size(); ++i)
				for (size_t j =i+1; j != sets.size(); ++j)
					if ( (sets[i].size() > 0) && (sets[j].size() > 0) )
						count += (sets[i] & sets[j]).num_ones();
			return count;
		}

		void process_subset_thres(std::list<int> &index)
		{
			// process a numbered (by list index) subset of sets (guaranteed nonempty)

			// if two elements have more than 90% common elements,
			// merge the two, save onto second, delete first

			// this part will remove elements the index list
			for (auto i = index.begin(); i != index.end(); ++i)
				for (auto j = std::next(i,1); j != index.end(); ++j)
					
					if ( (sets[*i] & sets[*j]).num_ones() > thres*sets[*i].num_ones() )
					{
						sets[*j] |= sets[*i];
						// std::cout << std::distance( index.begin(), i ) << '\n';
						// sets[*i].print();
						sets[*i] = lset(0);

						i =index.erase(i); --i;
						break;
					}

			// for (auto i = index.begin(); i != index.end(); ++i)
			// 	std::cout << sets[*i].num_ones() << std::endl;

		}

		void process_subset_inters_sort(std::list<int> &index)
		{

			// if there is an intersection among all the sets,
			// remove that from all but largest set

			// this function will empty the index list	
			
			// this part will work till no index remains

			// sort the list from largest to smallest
			index.sort( [&](int a, int b){ return (sets[a].num_ones() > 
				sets[b].num_ones()); }  );

			for (auto i = index.begin(); i != index.end();)
			{
				for (auto j = std::next(i,1); j != index.end(); ++j)
					if (sets[*j].size() > 0)
						sets[*j] &= ~sets[*i];
				index.sort( [&](int a, int b){ return (sets[a].num_ones() > 
				sets[b].num_ones()); }  ); // I cleverly sort here.
				i = index.erase(i);
			}

		}

		void process_subset_inters_find(std::list<int> &index)
		{
			// if there is an intersection among all the sets,
			// remove that from all but largest set

			// this function will empty the index list	
			
			// this part will work till no index remains

			lset el_rem;
			std::list<int>::iterator max_ii = index.begin();
			// int largest_size = 0;
			// int temp;

			while (index.begin() != index.end())
			{
				// largest_size = 0;
				// max_ii = index.begin();

				// for (auto  i = index.begin(); i != index.end(); ++i)
				// {
				// 	temp = sets[*i].num_ones();
				// 	// std::cout << temp << " " << sets[index[i]].size() << '\n';
				// 	if (temp > largest_size)
				// 	{
				// 		largest_size = temp;
				// 		max_ii = i;
				// 	}
					
				// }


				max_ii = std::max_element( index.begin(),index.end(),
					[&](int a, int b){ return (sets[a].num_ones() < sets[b].num_ones()); } );

				// max_ii = std::max_element( index.begin(),index.end(),
				// 	lsetproc::compare(*this));

				el_rem = ~sets[*max_ii] ;
				for (auto  i = index.begin(); i != index.end(); ++i)
					if ((i != max_ii) && (sets[*i].size() > 0))
						sets[*i] &= el_rem;
				index.erase(max_ii);
			}

			
		}

		struct compare
		{
			lsetproc &proc;
			compare(lsetproc &in) : proc(in){}
			bool operator() (int a, int b) { return (proc.sets[a].num_ones() 
				< proc.sets[b].num_ones()) ; }

		};

};



#endif