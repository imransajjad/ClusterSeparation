#ifndef LSETPROC_H
#define LSETPROC_H

#include <iostream>
#include <list>
#include <vector>
#include <bitset>

#define thres 0.9

void quickSort(int arr[], int left, int right)
{
	int i = left, j = right;
	int tmp;
	int pivot = arr[(left + right) / 2];

	/* partition */
	while (i <= j)
	{
		while (arr[i] < pivot)
			i++;
		while (arr[j] > pivot)
			j--;
		if (i <= j)
		{
			tmp = arr[i];
			arr[i] = arr[j];
			arr[j] = tmp;
			i++;
			j--;
		}
	}

	/* recursion */
	if (left < j)
	quickSort(arr, left, j);
	if (i < right)
	quickSort(arr, i, right);
}


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

		void process()
		{
			// generate a numbered subset of the sets and pass it to
			// the private processor function
			std::list<int> setlist;
			for (size_t i = 0; i != sets.size(); ++i)
				if (sets[i].size() >0)
					setlist.push_back(i);
			process_subset(setlist);
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

		void process_subset(std::list<int> &index)
		{
			// process a numbered (by list index) subset of sets

			// if two elements have more than 90% common elements,
			// merge the two, save onto second, delete first

			// if there is an intersection among all the sets,
			// remove that from all but largest set

			// this function will empty the index list	
			// lset intersection;
			lset inters_u_large_comp;
			// intersection = lset(1);
			std::list<int>::iterator largest_index_index = index.begin();
			int largest_size = 0;
			int temp;

			// this part will remove elements the index list
			for (auto  i = index.begin(); i != index.end(); ++i)
				for (auto j = std::next(i,1); j != index.end(); ++j)
					
					if ( (sets[*i].size() > 0) && (sets[*j].size() > 0) && 
						((double((sets[*i] & sets[*j]).num_ones()))/
						double(sets[*i].num_ones()) > thres) )
					{
						sets[*j] |= sets[*i];
						// std::cout << std::distance( index.begin(), i ) << '\n';
						sets[*i] = lset(0);
						i = index.erase(i);
						break;
					
					}

			// this part will work till no index remains
			while (index.begin() != index.end())
			{
				largest_size = 0;
				largest_index_index = index.begin();
				// intersection = lset(1);

				for (auto  i = index.begin(); i != index.end(); ++i)
				{
					// intersection &= sets[*i];
					temp = sets[*i].num_ones();
					// std::cout << temp << " " << sets[index[i]].size() << '\n';
					if (temp > largest_size)
					{
						largest_size = temp;
						largest_index_index = i;
					}
					
				}

				inters_u_large_comp = ~sets[*largest_index_index] ;
				//inters_u_large_comp.resize( max_size());
				for (auto  i = index.begin(); i != index.end(); ++i)
				{
					if ((i != largest_index_index) && (sets[*i].size() > 0))
						sets[*i] &= inters_u_large_comp;
					
				}
				index.erase(largest_index_index);
			}

			
		}

};

#endif