#pragma once
#include <cstddef>

struct TlsfBlockHeader
{

	/*******WARNING: 	
	
	The header may be offset from the raw block start address to ensure proper alignment.
	Make sure to account for this when calculating the size of the block.
	This offset is indicated by the rawOffset field in the header.
	ie. The address of Header may not be same as the start address of the block.

	*******/

	


	TlsfBlockHeader* nextFreeBlock = nullptr; // Pointer to the next block in the free list
	TlsfBlockHeader* prevFreeBlock = nullptr; // Pointer to the previous block in the free list
	
	size_t rawOffset = 0; // Offset from the start of the raw block to the header, used for alignment purposes

	size_t rawBlockSize = 0; // Size of the block, including header and footer and padding

	size_t UserAreaSize = 0; // Size of user space, not including the header and footer and padding

	//since header also has alignment requirements, we need to ensure that the header is aligned properly
	
	bool isFree = false; // Flag to indicate if the block is free or allocated

};




struct TlsfFooter
{

	size_t rawBlocksize = 0; // Size of the block, including header and footer

};


struct Layout
{
	size_t rawStartAddress = 0;
	size_t paddingHeader = 0;
	size_t HeaderStartAddress = 0; // Address of the TlsfBlockHeader(INCLUSIVE)
	size_t userAreaStartAddress = 0; // Start address of the user area(INCLUSIVE)
	size_t userAreaExclusiveEnd = 0; // End address of the user area
	size_t paddingFooter = 0; // Padding after the user area, if any
	size_t FooterStartAddress = 0; // Address of the TlsfBlockFooter(INCLUSIVE)
	size_t rawExclusiveEndAddress = 0;
};




struct TlsfBlock
{
	// This structure represents a block in the TLSF allocator.
	// It contains a header, user area, and footer.
	size_t rawStartAddress = 0; // Raw start address of the block, used for alignment and calculations
	TlsfBlockHeader* header = nullptr; // Pointer to the block header
	void* userArea = nullptr; // Pointer to the user area

	TlsfFooter* footer = nullptr; // Pointer to the block footer
};

/*

 General user allocatable Block Layout:
+--------------------------------------------------------------------------------------------------------------------+
| Padding(if any) |  TlsfBlockHeader    |    User Area     |     Padding (if any)    |    TlsfBlockFooter            |
+--------------------------------------------------------------------------------------------------------------------+

*/


/*

 Special non-allocatable Block layout:
	
	Start Block :-
+-----------------------------------------------------------------------------------------------------------------------------+
| Padding(if any) |  TlsfBlockHeader    |    User Area (0 bytes)    |     Padding (if any)    |    TlsfBlockFooter            |
+-----------------------------------------------------------------------------------------------------------------------------+

 
	End Block :- 
+--------------------------------------------------------------------------------------------------------------------+
| Padding(if any) |  TlsfBlockHeader    |    User Area ( 0 bytes)    |            Remaining bytes                    |
+--------------------------------------------------------------------------------------------------------------------+


 These two blocks are special blocks that mark the end and start of the memory pool.
 
 
 Start Block

   The Start Block marks the beginning of the memory pool. It's an un-allocatable block thats meant to
   signal the allocator that it has reached the start of the memory pool.
 
 
 End Block
   The End Block marks the end of the memory pool. Similar to the start block, 
   it's a block that an allocator shouldn't try to use for a normal allocation and signals end of memory pool.

 
*/





	








class TlsfAllocator
{
private:

	const size_t BLOCK_HEADER_SIZE = sizeof(TlsfBlockHeader);
	const uint8_t ALIGNMENT_REQ_BLOCK_HEADER = alignof(TlsfBlockHeader);

	const uint8_t FOOTER_SIZE = sizeof(TlsfFooter);
	const uint8_t ALIGNMENT_REQ_FOOTER = alignof(TlsfFooter);


	//currently second level is 8 sub-bins;

	Uint32_t m_firstLevelBitmap = 0; // Bitmap for the first level of free blocks
	Uint8_t m_secondLevelBitmap[32] = { 0 }; // Bitmap for the second level of free blocks,

	TlsfBlockHeader* m_freeList[32][8] = { nullptr }; // Free list for managing free blocks


	Layout calculateLayout(void* ptr, size_t size) const;

	void storeInFreeList(TlsfBlockHeader* header);



	TlsfBlock getNextTlsfBlock(TlsfBlockHeader* header) const;

	void storeInFreeList(TlsfBlockHeader* header);




	bool checkForwardMerge(TlsfBlockHeader* header) const;
	bool checkBackwardMerge(TlsfBlockHeader* header) const;

	TlsfBlockHeader* mergeForward(TlsfBlockHeader* header);
	TlsfBlockHeader* mergeBackward(TlsfBlockHeader* header);




public:

	void* allocate(size_t size);
	void deallocate(void* ptr);
}
