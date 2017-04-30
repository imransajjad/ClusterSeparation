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

void lset_fileread(std::vector<lset> &my_sets, std::string filename)
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
		my_sets.push_back(lset(temp, j));

		// done processing line
		// std::cout << "\n";
		j = 0; i++;
	}
}


int main ()
{
	std::cout << __cplusplus << '\n';
	std::cout << "csize: " << csize << '\n';

	std::vector<lset> my_sets;
	lset_fileread(my_sets, "../data/data_py_set");
	

	std::cout << "\nInput Data...\n\n" ;

	lsetproc set_processor = lsetproc(my_sets, 100 );

	set_processor.print_all(1);
	
	std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();

	set_processor.process();
	//set_processor.cleanup_fast();

	std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();

	std::cout << "\nDone.\n\n" ;
	set_processor.print_all(2);

	auto duration = std::chrono::duration_cast<std::chrono::microseconds>( t2 - t1 ).count();
	std::cout << duration*1e-6 << " seconds.\n" ;


	return 0;
}
