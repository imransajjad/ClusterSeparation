#ifndef LSET_H
#define LSET_H


#include <iostream>
#include <vector>
#include <bitset>

#define ctype uint32_t
const int csize = sizeof(ctype)*8; // num of bits used by ctype. char(8), bool(1), int(32)

//types and constants used in the functions below
//uint64_t is an unsigned 64-bit integer variable type (defined in C99 version of C language)
const uint64_t m1  = 0x5555555555555555; //binary: 0101...
const uint64_t m2  = 0x3333333333333333; //binary: 00110011..
const uint64_t m4  = 0x0f0f0f0f0f0f0f0f; //binary:  4 zeros,  4 ones ...
const uint64_t m8  = 0x00ff00ff00ff00ff; //binary:  8 zeros,  8 ones ...
const uint64_t m16 = 0x0000ffff0000ffff; //binary: 16 zeros, 16 ones ...
const uint64_t m32 = 0x00000000ffffffff; //binary: 32 zeros, 32 ones
const uint64_t hff = 0xffffffffffffffff; //binary: all ones
const uint64_t h01 = 0x0101010101010101; //the sum of 256 to the power of 0,1,2,3...

//This uses fewer arithmetic operations than any other known  
//implementation on machines with slow multiplication.
//This algorithm uses 17 arithmetic operations.
int popcount64b(uint64_t x)
{
    x -= (x >> 1) & m1;             //put count of each 2 bits into those 2 bits
    x = (x & m2) + ((x >> 2) & m2); //put count of each 4 bits into those 4 bits 
    x = (x + (x >> 4)) & m4;        //put count of each 8 bits into those 8 bits 
    x += x >>  8;  //put count of each 16 bits into their lowest 8 bits
    x += x >> 16;  //put count of each 32 bits into their lowest 8 bits
    x += x >> 32;  //put count of each 64 bits into their lowest 8 bits
    return x & 0x7f;
}

class lset
{
	friend class lsetproc;
	private:
		// data
		std::vector<ctype> v;
		int (*getHamm)(uint64_t) ;
		// funtion pointer to a standard Hamming weight function

	public:
		// constructor
		lset()
		{	getHamm = &popcount64b; }
		lset(int* cluster, int n)
		{	getHamm = &popcount64b;	init_data(cluster,n); }

		// copy constructor
		lset(const lset& that)
		{	v = that.v; /* does a deep copy*/ getHamm = &popcount64b;	}
		// copy assignment operator
		lset& operator=(const lset& that)
		{	v.clear();	v = that.v;	getHamm = &popcount64b; return *this; }


		// insert/initialize data
		void init_data(int* cluster, int n)
		{
			ctype temp  = 0;
			int count = 0;
			for(int i = 0; i <n; ++i)
			{

				// if value goes outside current block, append blocks
				while (cluster[i] >= (csize*(count+1)))
				{	v.push_back(temp); temp = 0; count++; }
				temp |= ((ctype)1 << (cluster[i]-csize*count)   );
				// std::cout << temp << " " << (cluster[i]-csize*count) <<'\n';
			}
			if (temp!=0)
				v.push_back(temp);
		}

		void insert(int in)
		{
			// find i
			int i = in/csize;
			if (i >= v.size())
				v.resize(i+1,0);
			v[i] |= (1 << (in-csize*i)   );
		}
		void remove(int in)
		{
			// find i
			int i = in/csize;
			if (i < v.size())
				v[i] &= ~(1 << (in-csize*i)   );
			i = v.size();
			while (v[i-1] == 0)
				i--;
			if (i < v.size())
				v.resize(i);
		}

		void print(int mode)
		{
			// 0 print all, 1 print stats, 2 print stats+bits, 3 print stats+num
			std:: cout << "at " << this << ", size is " << v.size() <<"x" 
				<< csize << ", numel is " << this->num_ones() << " elements are: " ;
			if (mode%2 == 0)
				print_list();
			// i print the array backwards, so that it is human readable
			if (mode%3 == 0)
				for (auto i = v.end(); i != v.begin(); --i)
					std::cout << std::bitset<csize>(*(i-1)) << ' ';
			std::cout << '\n';
		}

		void print_list()
		{
			ctype temp ;
			int j = 0;
			for (auto i = 0; i < v.size(); ++i)
			{
				temp = v[i];
				while (temp != 0)
				{
					if ( (temp & (ctype)1) != 0) std::cout << (i*csize)+ j << ", " ;
					temp = temp >> 1;
					j++;
				}
				j = 0;

			}
		}

		void print(){print(2);}

		int size() {return v.size();}
		ctype back() {return v.back();}
		void pop_back() {v.pop_back();}
		void resize(int target_size) {v.resize(target_size,0); }
		void resize(int target_size, ctype fill) {v.resize(target_size,fill); }

		int num_ones()
		{
			int count = 0;
			if (size() > 0)
			for (int i = 0; i < size(); ++i)
				count += getHamm( (uint64_t) v[i] );
			return count;
		}

		lset operator|(const lset& rhs)
		{
			lset b = rhs;
			b.resize( size() > b.size()?size():b.size(),0 );
			for (int i = 0; i < size(); ++i)
				b.v[i] |= v[i];
			return b;
		}

		lset& operator|=(const lset& rhs)
		{
			lset a = rhs;		
			resize( a.size() > size()?a.size():size(),0 );
			for (int i = 0; i < a.size(); ++i)
				v[i] |= a.v[i];
			return *this;
		}

		lset operator&(const lset& rhs)
		{
			lset b = rhs;
			b.resize( size() < b.size()?size():b.size() );
			for (int i = 0; i < b.size(); ++i)
				b.v[i] &= v[i];
			if (b.num_ones() == 0)
				b.resize(0);
			else while (b.back()==0) b.pop_back();
			return b;
		}

		lset& operator&=(const lset& rhs)
		{
			lset a = rhs;
			resize( a.size() < size()?a.size():size() );
			for (int i = 0; i < size(); ++i)
				v[i] &= a.v[i];
			if (num_ones() == 0)
				resize(0);
			else while (back()==0) pop_back();
			return *this;
		}

		lset operator~()
		{
			lset a = *this;
			for (int i = 0; i < a.size(); ++i)
				a.v[i] = ~a.v[i];
			return a;
		}
};

#endif