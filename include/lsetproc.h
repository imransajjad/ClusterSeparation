#ifndef LSETPROC_H
#define LSETPROC_H

#include <iostream>
#include <vector>
#include <bitset>

#define thres 0.9

class lsetproc
{
	// this data type processes a list of lset objects in place
	// do not copy or assign an instance of this object to another of its type
	// even if destroyed, should leave the lset objects in place
	int num_sets;
	lset* sets;
	int batch_size;

	public:
		lsetproc(lset* sets_in, int num_sets_in, int batch_size_in)
		{
			num_sets = num_sets_in;
			batch_size = batch_size_in;
			sets = sets_in;
		}

		// copy constructor
		lsetproc(const lset& that)
		{
			std::cout << "Do not try and copy this object!!!";
			num_sets = 0;
			batch_size = 0;
			sets = NULL;

		}

		void process()
		{
			int setlist[num_sets];
			int li;
			int sets_rem = num_sets;

			for (int i = 0; i < num_sets; ++i)
				setlist[i] = i;

			while( sets_rem > 2)
		 	{
				li = process_subset(setlist,sets_rem);
				// std::cout << '\n' << sets_rem << " " << li<< " " ;
				if (li != sets_rem)
				{
					sets_rem--;
					// std::cout << setlist[li] << '\n'; 
					setlist[li] = setlist[sets_rem];					
				}
			}
		}

		int process_subset(int* index, int n)
		{
			lset intersection;
			int largest_size = 0;
			int largest_index_index = 0;
			for (int i = 0; i < n; ++i)
			{
				intersection &= sets[index[i]];
				if (sets[index[i]].num_ones() > largest_size)
				{
					largest_size =  sets[index[i]].num_ones();
					largest_index_index = i;
				}
				
			}
			
			double overlap = ((double)intersection.num_ones())/largest_size;

			// std::cout << "overlap is " << overlap << std::endl;

			if (overlap > thres)
			{
				lset amalgamation;
				for (int i = 0; i < n; ++i)
				{
					amalgamation |= sets[index[i]];
					sets[index[i]].resize(0);
				}
				sets[index[0]] = amalgamation;
				return n;
			}
			else
			{
				// std::cout << largest_index_index << "\n";
				lset inters_u_large = sets[index[largest_index_index]] ;
				for (int i = 0; i < n; ++i)
				{
					if (i != largest_index_index)
						sets[index[i]] = sets[index[i]] & (~inters_u_large);
					
				}
				return largest_index_index;
			}
			
		}
	

};

#endif