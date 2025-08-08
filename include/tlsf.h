#pragma once
#include <cstddef>

struct TlsfBlockHeader
{

	/*******WARNING: 	
	
	The header may be offset from the raw block start address to ensure proper alignment.
	Make sure to account for this when calculating the size of the block.

	ie. The address of Header may not be same as the start address of the block.

	*******/

	


	TlsfBlockHeader* next; // Pointer to the next block in the free list
	TlsfBlockHeader* prev; // Pointer to the previous block in the free list
	

	size_t size; // Size of user space, not including the header and footer and padding

	//since header also has alignment requirements, we need to ensure that the header is aligned properly
	
	bool isFree; // Flag to indicate if the block is free or allocated

};




struct TlsfFooter
{

	size_t size; // Size of the block, including header and footer

};


struct Layout
{
	size_t rawStartAddress;
	size_t paddingHeader;
	size_t HeaderStartAddress; // Address of the TlsfBlockHeader(INCLUSIVE)
	size_t userAreaStartAddress; // Start address of the user area(INCLUSIVE)
	size_t userAreaExclusiveEnd; // End address of the user area
	size_t paddingFooter; // Padding after the user area, if any
	size_t FooterStartAddress; // Address of the TlsfBlockFooter(INCLUSIVE)
	size_t rawExclusiveEndAddress;
};



/*

Block Header Layout:
+--------------------------------------------------------------------------------------------------------------------+
| Padding(if any) |  TlsfBlockHeader    |    User Area     |     Padding (if any)    |    TlsfBlockFooter(size_t)    |
+--------------------------------------------------------------------------------------------------------------------+

*/

class TlsfAllocator
{
private:

	const size_t BLOCK_HEADER_SIZE = sizeof(TlsfBlockHeader);
	const uint8_t ALIGNMENT_REQ_BLOCK_HEADER = alignof(TlsfBlockHeader);

	const uint8_t FOOTER_SIZE = sizeof(TlsfFooter);
	const uint8_t ALIGNMENT_REQ_FOOTER = alignof(TlsfFooter);


	Layout calculateLayout(void* ptr, size_t size) const;

public:

	void* allocate(size_t size);
	void deallocate(void* ptr);
}
