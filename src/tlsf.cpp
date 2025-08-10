
#include "tlsf.h"



void* TlsfAllocator::allocate(size_t size)
{

}

void TlsfAllocator::deallocate(void* ptr)
{
	if (ptr == nullptr)
	{
		return; // Nothing to deallocate
	}
	
	const const size_t headerRawAddress = reinterpret_cast<size_t>(ptr) - BLOCK_HEADER_SIZE;

	TlsfBlockHeader* header = reinterpret_cast<TlsfBlockHeader*>(headerRawAddress);

	while (checkForwardMerge(header))
	{
		// Merge with the next block
		header = mergeForward(header);
	}

	while (checkBackwardMerge(header))
	{
		// Merge with the previous block
		header = mergeBackward(header);
	}

	// At this point, the header is merged with both forward and backward blocks if possible
	 
	//store in free list
	storeInFreeList(header);

}


Layout TlsfAllocator::calculateLayout(void* ptr, size_t size) const
{
	Layout layout;

	if (ptr == nullptr || size == 0)
	{
		// Invalid input, return an empty layout
		return layout;
	}
	// range is always defined as [start, end)


	// Calculate the raw start address
	layout.rawStartAddress = reinterpret_cast<size_t>(ptr);

	// Calculate padding for header
	layout.paddingHeader = (ALIGNMENT_REQ_BLOCK_HEADER - (layout.rawStartAddress % ALIGNMENT_REQ_BLOCK_HEADER)) % ALIGNMENT_REQ_BLOCK_HEADER;

	// Header start address
	layout.HeaderStartAddress = layout.rawStartAddress + layout.paddingHeader;

	// User area start address
	layout.userAreaStartAddress = layout.HeaderStartAddress + BLOCK_HEADER_SIZE;

	// User area end address
	layout.userAreaExclusiveEnd = layout.userAreaStartAddress + size;

	// Padding after user area
	layout.paddingFooter = (ALIGNMENT_REQ_FOOTER - ((layout.userAreaExclusiveEnd ) % ALIGNMENT_REQ_FOOTER)) % ALIGNMENT_REQ_FOOTER;

	// Footer start address
	layout.FooterStartAddress = layout.userAreaExclusiveEnd + layout.paddingFooter;

	// Raw end address
	layout.rawExclusiveEndAddress = layout.FooterStartAddress + FOOTER_SIZE;

	return layout;
}



void TlsfAllocator::storeInFreeList(TlsfBlockHeader* header)
{
	if (header == nullptr)
	{
		return; 
	}




	size_t firstLevelIndex = getFirstLevelIndex(header->UserAreaSize);
	size_t secondLevelIndex = getSecondLevelIndex(header->UserAreaSize);

	//ToDO : check inavlid index




	


	if (m_freeList[firstLevelIndex][secondLevelIndex] == nullptr) // If the sub-bin is empty
	{

		//Case 1: Second level sub-bin is empty
			// Add this header as first in sublevel index, check if bitmaps are correctly set and set it
			// or avoid check and set it, can say some if statements

		m_freeList[firstLevelIndex][secondLevelIndex] = header; // Set the header as the first block in the sub-bin

		header->nextFreeBlock = nullptr; // No next block
		header->prevFreeBlock = nullptr; // No previous block

		m_secondLevelBitmap[firstLevelIndex] |= (1 << secondLevelIndex); // Set the second level bitmap for this size
		m_firstLevelBitmap |= (1 << firstLevelIndex);
	}
	else // If the sub-bin is not empty
	{

		//Case 2: Sub-bin is not empty ,add this block to first in sub-bin and update the next and previous pointers 

		TlsfBlockHeader* firstHeader = m_freeList[firstLevelIndex][secondLevelIndex];
		header->nextFreeBlock = firstHeader; // Set the new header as the first block in the sub-bin
		firstHeader->prevFreeBlock = header; // Update the previous pointer of the first block
		m_freeList[firstLevelIndex][secondLevelIndex] = header; // Update the free list to point to the new header
	}

	return;


}



TlsfBlock TlsfAllocator::getNextTlsfBlock(TlsfBlockHeader* header) const
{

	TlsfBlock nextTlsfBlock;

	if (header == nullptr)
	{
		return nextTlsfBlock; // Return an empty block if the header is null
	}

	const size_t nextBlockRawAddress = (reinterpret_cast<size_t>(header) - (header->rawOffset)) + header->rawBlockSize;


	const size_t nextBlockHeaderAddress = nextBlockRawAddress + ((ALIGNMENT_REQ_BLOCK_HEADER - (nextBlockRawAddress % ALIGNMENT_REQ_BLOCK_HEADER)) % ALIGNMENT_REQ_BLOCK_HEADER);

	const TlsfBlockHeader* nextHeader = reinterpret_cast<TlsfBlockHeader*>(nextBlockHeaderAddress);

	const size_t nextBlockFooterAddress = nextBlockRawAddress + header->rawBlockSize - FOOTER_SIZE;

	nextTlsfBlock.header = nextHeader;
	nextTlsfBlock.userArea = reinterpret_cast<void*>(nextBlockHeaderAddress + BLOCK_HEADER_SIZE);
	nextTlsfBlock.footer = reinterpret_cast<TlsfFooter*>(nextBlockFooterAddress);

	return nextTlsfBlock;
}

TlsfBlock TlsfAllocator::getPreviousTlsfBlock(TlsfBlockHeader* header) const
{

	TlsfBlock prevTlsfBlock;
	if (header == nullptr)
	{
		return prevTlsfBlock; // Return an empty block if the header is null
	}

	const size_t blockRawAddress = reinterpret_cast<size_t>(header) - header->rawOffset;

	const size_t prevBlockFooterAddress = blockRawAddress - FOOTER_SIZE;

	const TlsfFooter* prevFooter = reinterpret_cast<TlsfFooter*>(prevBlockFooterAddress);



	const size_t prevBlockRawSize = prevFooter->RawBlocksize;

	const size_t prevBlockRawAddress = blockRawAddress - prevBlockRawSize;

	const size_t prevBlockHeaderAddress = prevBlockRawAddress + ((ALIGNMENT_REQ_BLOCK_HEADER - (prevBlockRawAddress % ALIGNMENT_REQ_BLOCK_HEADER)) % ALIGNMENT_REQ_BLOCK_HEADER);


	const TlsfBlockHeader* prevHeader = reinterpret_cast<TlsfBlockHeader*>(prevBlockHeaderAddress);

	prevTlsfBlock.header = prevHeader;
	prevTlsfBlock.userArea = reinterpret_cast<void*>(prevBlockHeaderAddress + BLOCK_HEADER_SIZE);
	prevTlsfBlock.footer = reinterpret_cast<TlsfFooter*>(prevBlockFooterAddress);

	return prevTlsfBlock;


}




bool TlsfAllocator::checkForwardMerge(TlsfBlockHeader* header) const
{
	if (header == nullptr)
	{
		return false; // No block to merge with
	}


	TlsfBlock nextTlsfBlock = getNextTlsfBlock(header);

	const TlsfBlockHeader* nextHeader = nextTlsfBlock.header;

	if (nextHeader->isFree)
	{
		// Check if the next block is free and can be merged
		return true;
	}

	return false;

}


bool TlsfAllocator::checkBackwardMerge(TlsfBlockHeader* header) const
{
	if (header == nullptr)
	{
		return false; // No block to merge with
	}

	TlsfBlock prevTlsfBlock = getPreviousTlsfBlock(header);
	const TlsfBlockHeader* prevHeader = prevTlsfBlock.header;

	if (prevHeader->isFree)
	{
		// Check if the previous block is free and can be merged
		return true;
	}

	return false;
}












TlsfBlockHeader* TlsfAllocator::mergeForward(TlsfBlockHeader* header)
{
	if (header == nullptr)
	{
		return nullptr; // Nothing to merge
	}
	TlsfBlock nextTlsfBlock = getNextTlsfBlock(header);

	if (nextTlsfBlock.header == nullptr)
	{
		return nullptr; // No next block to merge with
	}

	//ToDo: Implement merge logic


	 

}


TlsfBlockHeader* TlsfAllocator::mergeBackward(TlsfBlockHeader* header)
{
	if (header == nullptr)
	{
		return nullptr; // Nothing to merge
	}

	TlsfBlock prevTlsfBlock = getPreviousTlsfBlock(header);

	if (prevTlsfBlock.header == nullptr)
	{
		return nullptr; // No previous block to merge with
	}
	//ToDo: Implement merge logic





}