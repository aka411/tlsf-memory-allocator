#pragma once
#include <cstdint>

struct TlsfBlockHeader
{

	/*******WARNING: 	
	
	The header is offset from the raw block start address to ensure proper alignment of Header if  raw block start address is not a Header aligned address.
	This offset is indicated by the rawOffset field in the header.
	ie. The address of Header may not be same as the start address of the raw block.

	*******/

	


	TlsfBlockHeader* nextFreeBlock = nullptr; // Pointer to the next block in the free list
	TlsfBlockHeader* prevFreeBlock = nullptr; // Pointer to the previous block in the free list
	
	size_t rawOffset = 0; // Offset from the start of the raw block to the header,this offset is there because of the Tlsf header alignment requirements.

	size_t rawBlockSize = 0; // Size of the block, including header and footer and all padding

	size_t UserAreaSize = 0; //Size of the area usable by the user


	
	bool isFree = false; 

};




struct TlsfFooter
{

	size_t rawBlocksize = 0; //including header and footer and all padding jsut like in TlsfBlockHeader

};


struct Layout
{
	size_t rawStartAddress = 0;

	size_t paddingHeader = 0;
	size_t HeaderStartAddress = 0; // Address of the TlsfBlockHeader(INCLUSIVE)

	size_t userAreaStartAddress = 0; // Start address of the user area(INCLUSIVE)
	size_t userAreaExclusiveEnd = 0; 

	size_t paddingFooter = 0; // Padding after the user area, if any
	size_t footerStartAddress = 0; // Address of the TlsfBlockFooter(INCLUSIVE)

	size_t rawExclusiveEndAddress = 0;
};




struct TlsfBlock
{
	

	size_t rawStartAddress = 0;
	TlsfBlockHeader* header = nullptr; 

	void* userArea = nullptr; 

	TlsfFooter* footer = nullptr; 
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







class TlsfAllocatorInternalTest;//for testing






class TlsfAllocator
{
protected:


	
	friend TlsfAllocatorInternalTest; // for testing

	const size_t BLOCK_HEADER_SIZE = sizeof(TlsfBlockHeader);
	const uint8_t ALIGNMENT_REQ_BLOCK_HEADER = alignof(TlsfBlockHeader);

	const uint8_t FOOTER_SIZE = sizeof(TlsfFooter);
	const uint8_t ALIGNMENT_REQ_FOOTER = alignof(TlsfFooter);



	const size_t MINIMUM_VALID_TLSF_BLOCK_RAW_SIZE = (ALIGNMENT_REQ_BLOCK_HEADER - 1) + BLOCK_HEADER_SIZE + 4 /*USER BYTES*/ + (ALIGNMENT_REQ_FOOTER - 1) + FOOTER_SIZE;




	TlsfBlock m_startTlsfBlock ;
	TlsfBlock m_endTlsfBlock ;



	//currently second level is 8 sub-bins;

	uint32_t m_firstLevelBitmap = 0; // Bitmap for the first level of free blocks
	uint8_t m_secondLevelBitmap[32] = { 0 }; // Bitmap for the second level of free blocks,

	const size_t m_subBinCount = sizeof(m_secondLevelBitmap[0]) * 8;

	TlsfBlockHeader* m_freeList[32][8] = { nullptr }; // Free list for managing free blocks


	Layout calculateLayout(const size_t startAddress, const size_t size) const;

	//this method assumes that  (rawEndAddress - FOOTER_SIZE) is aligned for footer else we will have problem finding footer and header
	TlsfBlockHeader* createTlsfBlock(const size_t rawStartAddress, const size_t rawEndAddress) const;

	bool checkIfSecondLevelEmpty(size_t firstLevelIndex) const;

	size_t getFirstLevelIndex(size_t size) const;
	size_t getSecondLevelIndex(size_t size) const;




	void storeInFreeList(TlsfBlockHeader* header);
	void removeFromFreeList(TlsfBlockHeader* CurrHeader);


	TlsfBlock getNextTlsfBlock(TlsfBlockHeader* header) const;//rename to getNextMemoryBlock?
	TlsfBlock getPreviousTlsfBlock(TlsfBlockHeader* header) const;


	bool checkForwardMerge(TlsfBlockHeader* header) const;
	bool checkBackwardMerge(TlsfBlockHeader* header) const;

	TlsfBlockHeader* mergeForward(TlsfBlockHeader* header);
	TlsfBlockHeader* mergeBackward(TlsfBlockHeader* header);

	TlsfBlockHeader* coalesceBlocks(TlsfBlockHeader* currHeader);

	TlsfBlockHeader* getFreeBlock(const size_t requiredSize);


public:

	TlsfAllocator(const size_t memoryPoolSize);

	~TlsfAllocator();

	void* allocate(size_t size);
	void deallocate(void* ptr);
};
