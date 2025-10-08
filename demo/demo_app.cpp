#include "tlsf.h"
#include <iostream>


int main()
{

	TlsfAllocator tlsfAllocator(1024);// Get 1KB of memory for the pool

	void* ptr = tlsfAllocator.allocate(20); // get a pointer to block of size 20 bytes

	if (ptr != nullptr)
	{
		std::cout << "Successfully allocated 20 bytes of memory" << std::endl;
		tlsfAllocator.deallocate(ptr);

	}
	else
	{
		std::cout << "Failed to allocat 20 bytes of memory" << std::endl;
		return -1;
	}
	std::cout << "Press Enter key to Exit" << std::endl;
	std::cin.get();// wait for user to press enter to avoid exiting fast

	return 1;
}