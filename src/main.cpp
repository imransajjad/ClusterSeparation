// in order to use the 'auto' type, use c++11
// g++ main.cpp -std=c++11 -o main && ./main

// g++ -c ../src/main.* -o main.o -I../include -std=c++11
// g++ *.o -o ../bin/main.exe -L../lib

#include <cstdlib>
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <chrono>

#include "lset.h"
#include "lsetproc.h"

#define line_length 8192


int file_length(std::string filename)
{
	std::ifstream file ( filename );
	std::string line_buf;
	int i = 0;
	while ( file.good() )
	{
		getline ( file, line_buf, '\n' );
		i++;
	}
	return i;
}

void lset_fileread(lset* my_sets, std::string filename)
{
	std::ifstream file ( filename );
	std::string line_buf;
	std::string int_buf;
	
	int temp[line_length];
	int i = 0;
	int j = 0;
	while ( file.good() )
	{
		// get line
		getline ( file, line_buf, '\n' );

		// start processing line
		std::stringstream line(line_buf);

		while (line)
		{
			getline ( line, int_buf, ',' );
			// std::cout << int_buf << " , ";
			if (!int_buf.empty())
			{
				temp[j] = std::atoi(int_buf.c_str());
				j++;
			}
		}
		// for (int k = 0; k < j; ++k)
		// 	std::cout << temp[k] << " ";
		// std::cout << '\n';
		my_sets[i].init_data(temp, j);

		// done processing line
		// std::cout << "\n";
		j = 0; i++;
	}
}


int main ()
{
	// std::cout << __cplusplus << '\n';
	std::cout << "csize: " << csize << '\n';

	std::string filename = "../data/data_py_set";
	int num_sets = file_length(filename);
	lset my_sets[num_sets];
	lset_fileread(my_sets, filename);

	// my_sets[1] = my_sets[2];

	lset union_before = lset();
	lset union_after = lset();
	lset intersection_after = lset();

	// union_after.insert(0);
	// union_before.insert(1); // union_before.remove(1);
	// union_before.insert(118);

	// union_before.print();
	// union_after.print();
	
	// union_before &=union_after;
	// union_before.print();

	// union_after = union_before & union_after; 
	// union_after.print();
	// union_after = union_after | union_before; // this one used to work
	// union_after.print();

	

	std::cout << "\nInput Data...\n\n" ;

	for (int i = 0; i < num_sets; ++i)
	{
		my_sets[i].print(2);
		union_before |= my_sets[i];
	}

	

 	std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();

 	lsetproc set_processor = lsetproc(my_sets, num_sets, (int)(100) );
 	//set_processor.set_max_element(100);
 	set_processor.process();

	std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();

	std::cout << "\nDone.\n\n" ;

	for (int i = 0; i < num_sets; ++i)
	{
		if (my_sets[i].num_ones() > 0)
		{ my_sets[i].print(2);}
		union_after |= my_sets[i] ;
		intersection_after &= my_sets[i];
	}

	std::cout << "\nUnion before and after, and intersection_after \n\n" ;
	union_before.print(1);
	union_after.print(1);
	intersection_after.print(1);

	auto duration = std::chrono::duration_cast<std::chrono::microseconds>( t2 - t1 ).count();
	std::cout << duration << " microseconds.\n" ;


	return 0;
}
