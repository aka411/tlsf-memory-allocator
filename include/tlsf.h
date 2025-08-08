#pragma once

class TlsfAllocator
{
public:

	void* allocate(size_t size);
	void deallocate(void* ptr);
}
