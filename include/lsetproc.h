#ifndef LSETPROC_H
#define LSETPROC_H

#include <iostream>
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
	// do not copy or assign an instance of this object to another of its type
	// even if destroyed, should leave the lset objects in place
	int num_sets;
	lset* sets;
	int batch_size;
	int max_size;

	public:
		lsetproc(lset* sets_in, int num_sets_in, int batch_size_in)
		{
			num_sets = num_sets_in;
			batch_size = batch_size_in;
			sets = sets_in;
			find_largest_size();
		}

		// copy constructor
		lsetproc(const lset& that)
		{
			std::cout << "Do not try and copy this object!!!";
			num_sets = 0;
			batch_size = 0;
			sets = NULL;

		}

		void find_largest_size()
		{
			int t;
			for (int i = 0; i < num_sets; ++i)
			{
				t = sets[i].size();
				if (t> max_size)
					max_size = t;
			}
		}

		void set_max_size(int in){	max_size = in;	}

		void process()
		{
			int setlist[num_sets];
			int li;
			int sets_rem = num_sets;

			for (int i = 0; i < num_sets; ++i)
				setlist[i] = i;

			while( sets_rem > 0)
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
			int temp;
			for (int i = 0; i < n; ++i)
			{
				intersection &= sets[index[i]];
				temp = sets[index[i]].num_ones();
				// std::cout << temp << " " << sets[index[i]].size() << '\n';
				if (temp > largest_size)
				{
					largest_size = temp;
					largest_index_index = i;
				}
				
			}
			
			double overlap = ((double)intersection.num_ones())/largest_size;

			// std::cout << "overlap is " << overlap << std::endl;

			if (overlap > thres)
			{
				std::cout << "thres met" << largest_index_index << " " << index[largest_index_index] << " " <<largest_size << "\n";
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
				// std::cout << "thres not met " << n << " " << largest_index_index <<
				//  " " << index[largest_index_index] << " " <<largest_size << "\n";
				// for (int i = 0; i < n; ++i)
				// 	std::cout << index[i] << " ";
				// std::cout << '\n';	
				lset inters_u_large_comp = ~sets[index[largest_index_index]] ;
				inters_u_large_comp.resize( max_size, (ctype)-1 );
				for (int i = 0; i < n; ++i)
				{
					if (i != largest_index_index)
						sets[index[i]] &= inters_u_large_comp;
					
				}
				return largest_index_index;
			}
			
		}
	

};

#endif