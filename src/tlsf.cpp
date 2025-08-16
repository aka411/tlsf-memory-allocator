
#include "tlsf.h"
#include <cstdlib>
#include <iostream>

bool TlsfAllocator::checkIfSecondLevelEmpty(size_t firstLevelIndex) const
{
	//or i could just check the bitset

	if (firstLevelIndex >= sizeof(m_firstLevelBitmap)*8) //bits
	{
		return true;
	}

	// Check if any second level sub-bin is non-empty
	for (size_t subBinIndex = 0; subBinIndex < sizeof(m_secondLevelBitmap[firstLevelIndex]) * 8; ++subBinIndex)//bits
	{
		if (m_freeList[firstLevelIndex][subBinIndex] != nullptr)
		{
			return false; 
		}
	}
	return true; 
}





size_t TlsfAllocator::getFirstLevelIndex(size_t size) const
{

	//Windows/MSVC:

//_BitScanForward()


//GCC:

//__builtin_ffs()


	size_t potentialFirstBitSetIndex = (sizeof(size) * 8) - 1;//8 bits * number of bytes ,since zero based index we minus 1


	while (true)
	{
		if (size & (1ULL << potentialFirstBitSetIndex)) return potentialFirstBitSetIndex;

		if (potentialFirstBitSetIndex == 0) break;

		--potentialFirstBitSetIndex;

		
	}



	return 0;
}

size_t TlsfAllocator::getSecondLevelIndex(size_t size) const
{
	const size_t flI = getFirstLevelIndex(size);
	



	const size_t firstLevelBinRangeSize = (1ULL << flI);

	const size_t subBinIntervalSize = firstLevelBinRangeSize / m_subBinCount;



	const size_t subBinIndex = (size - firstLevelBinRangeSize) / subBinIntervalSize;


	return subBinIndex;
}






Layout TlsfAllocator::calculateLayout(const size_t startAddress,const size_t size) const
{
	Layout layout;


	// range is always defined as [start, end)


	layout.rawStartAddress = startAddress;

	// amount of padding not address
	layout.paddingHeader = (ALIGNMENT_REQ_BLOCK_HEADER - (layout.rawStartAddress % ALIGNMENT_REQ_BLOCK_HEADER)) % ALIGNMENT_REQ_BLOCK_HEADER;


	layout.HeaderStartAddress = layout.rawStartAddress + layout.paddingHeader;


	layout.userAreaStartAddress = layout.HeaderStartAddress + BLOCK_HEADER_SIZE;


	layout.userAreaExclusiveEnd = layout.userAreaStartAddress + size;


	layout.paddingFooter = (ALIGNMENT_REQ_FOOTER - (layout.userAreaExclusiveEnd % ALIGNMENT_REQ_FOOTER)) % ALIGNMENT_REQ_FOOTER;


	layout.footerStartAddress = layout.userAreaExclusiveEnd + layout.paddingFooter;


	layout.rawExclusiveEndAddress = layout.footerStartAddress + FOOTER_SIZE;

	return layout;
}

TlsfBlockHeader* TlsfAllocator::createTlsfBlock(const size_t rawStartAddress, const size_t rawEndAddress) const
{

	const size_t headerPadding = (ALIGNMENT_REQ_BLOCK_HEADER - (rawStartAddress % ALIGNMENT_REQ_BLOCK_HEADER) )% ALIGNMENT_REQ_BLOCK_HEADER;

	const size_t headerStartAddress = rawStartAddress + headerPadding;

	const size_t userAreaStartAddress = headerStartAddress + BLOCK_HEADER_SIZE;

	
	const size_t footerStartAddress = rawEndAddress - FOOTER_SIZE;

	if (footerStartAddress % ALIGNMENT_REQ_FOOTER)
	{
		//we have a problem
		return nullptr;
	}

	const size_t userAreaEndAddress = footerStartAddress; // no footer padding ,so they overlap

	
	
	const size_t userAreaSize = userAreaEndAddress - userAreaStartAddress;




	TlsfBlockHeader* header = reinterpret_cast<TlsfBlockHeader*>(headerStartAddress);

	TlsfFooter* footer = reinterpret_cast<TlsfFooter*>(footerStartAddress);


	header->rawBlockSize = (rawEndAddress - rawStartAddress);
	header->rawOffset = headerPadding ;
	header->UserAreaSize = userAreaSize;
	header->isFree = true;

	footer->rawBlocksize = header->rawBlockSize;

	return header;

}



void TlsfAllocator::storeInFreeList(TlsfBlockHeader* header)
{
	if (header == nullptr)
	{
		return; 
	}




	size_t firstLevelIndex = getFirstLevelIndex(header->UserAreaSize);
	size_t secondLevelIndex = getSecondLevelIndex(header->UserAreaSize);

	//ToDo : check inavlid index



	header->isFree = true;
	

	
	if (m_freeList[firstLevelIndex][secondLevelIndex] == nullptr) // If the sub-bin is empty
	{

		//Case 1: Second level sub-bin is empty
			// Add this header as first in sublevel index, check if bitmaps are correctly set and set it
			// or avoid check and set it, can say some if statements

		m_freeList[firstLevelIndex][secondLevelIndex] = header; // Set the header as the first block in the sub-bin

		header->nextFreeBlock = nullptr; // No next block
		header->prevFreeBlock = nullptr; // No previous block

	
		m_secondLevelBitmap[firstLevelIndex] |= (1ULL << secondLevelIndex); // Set the second level bitmap for this size

		
		
		m_firstLevelBitmap |= (1ULL << firstLevelIndex);
		
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





void TlsfAllocator::removeFromFreeList(TlsfBlockHeader* currHeader)
{

	if (currHeader == nullptr)
	{
		return;
	}


	TlsfBlockHeader* nextHeader = currHeader->nextFreeBlock;
	TlsfBlockHeader* prevHeader = currHeader->prevFreeBlock;

	if (  prevHeader != nullptr && nextHeader != nullptr ) // Case 1: Both next and previous headers exist in the sub-bin
	{
		prevHeader->nextFreeBlock = nextHeader;
		nextHeader->prevFreeBlock = prevHeader;


	}
	else if (prevHeader == nullptr && nextHeader != nullptr) // case 2: Only next header exsits and currHeader is the first in the second level sub-bin
	{
		

		size_t firstLevelIndex = getFirstLevelIndex(currHeader->UserAreaSize);
		size_t secondLevelIndex = getSecondLevelIndex(currHeader->UserAreaSize);

		m_freeList[firstLevelIndex][secondLevelIndex] = nextHeader; // Update the free list to point to the next header

		nextHeader->prevFreeBlock = nullptr; // Clear the previous pointer of the next header


	}
	else if (prevHeader == nullptr && nextHeader == nullptr) // case 3 : currHeader is the only free block in second Level sub-bin
	{



		size_t firstLevelIndex = getFirstLevelIndex(currHeader->UserAreaSize);
		size_t secondLevelIndex = getSecondLevelIndex(currHeader->UserAreaSize);

		//set Second Level sub-bin to nullptr and second level bitmap to 0

		m_freeList[firstLevelIndex][secondLevelIndex] = nullptr; // Update the free list to point to nullptr, since the list is now empty
		m_secondLevelBitmap[firstLevelIndex] &= ~(1 << secondLevelIndex); // Clear the second level bitmap for this size


		if (checkIfSecondLevelEmpty(firstLevelIndex)) // Check if the second level is empty and update the first level bitmap accordingly
		{
			m_firstLevelBitmap &= ~(1 << firstLevelIndex); // Clear the first level bitmap for this size
		}

	}

	//common for all conditions
	currHeader->nextFreeBlock = nullptr; 
	currHeader->prevFreeBlock = nullptr; 
	currHeader->isFree = false;




	return;
}



TlsfBlock TlsfAllocator::getNextTlsfBlock(TlsfBlockHeader* header) const
{

	TlsfBlock nextTlsfBlock;

	if (header == nullptr)
	{
		return nextTlsfBlock; 
	}

	const size_t nextBlockRawAddress = (reinterpret_cast<size_t>(header) - (header->rawOffset)) + header->rawBlockSize;


	const size_t nextBlockHeaderAddress = nextBlockRawAddress + ((ALIGNMENT_REQ_BLOCK_HEADER - (nextBlockRawAddress % ALIGNMENT_REQ_BLOCK_HEADER)) % ALIGNMENT_REQ_BLOCK_HEADER);

	 TlsfBlockHeader* nextHeader = reinterpret_cast<TlsfBlockHeader*>(nextBlockHeaderAddress);

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
		return prevTlsfBlock; 
	}

	const size_t blockRawAddress = reinterpret_cast<size_t>(header) - header->rawOffset;

	const size_t prevBlockFooterAddress = blockRawAddress - FOOTER_SIZE;

	const TlsfFooter* prevFooter = reinterpret_cast<TlsfFooter*>(prevBlockFooterAddress);



	const size_t prevBlockRawSize = prevFooter->rawBlocksize;

	const size_t prevBlockRawAddress = blockRawAddress - prevBlockRawSize;

	const size_t prevBlockHeaderAddress = prevBlockRawAddress + ((ALIGNMENT_REQ_BLOCK_HEADER - (prevBlockRawAddress % ALIGNMENT_REQ_BLOCK_HEADER)) % ALIGNMENT_REQ_BLOCK_HEADER);


    TlsfBlockHeader* prevHeader = reinterpret_cast<TlsfBlockHeader*>(prevBlockHeaderAddress);

	prevTlsfBlock.header = prevHeader;
	prevTlsfBlock.userArea = reinterpret_cast<void*>(prevBlockHeaderAddress + BLOCK_HEADER_SIZE);
	prevTlsfBlock.footer = reinterpret_cast<TlsfFooter*>(prevBlockFooterAddress);

	return prevTlsfBlock;


}




bool TlsfAllocator::checkForwardMerge(TlsfBlockHeader* header) const
{
	if (header == nullptr)
	{
		return false; 
	}


	TlsfBlock nextTlsfBlock = getNextTlsfBlock(header);

	const TlsfBlockHeader* nextHeader = nextTlsfBlock.header;

	if (nextHeader->isFree)
	{

		return true;
	}

	return false;

}


bool TlsfAllocator::checkBackwardMerge(TlsfBlockHeader* header) const
{
	if (header == nullptr)
	{
		return false; 
	}

	TlsfBlock prevTlsfBlock = getPreviousTlsfBlock(header);
	const TlsfBlockHeader* prevHeader = prevTlsfBlock.header;

	if (prevHeader->isFree)
	{
		
		return true;
	}

	return false;


}












TlsfBlockHeader* TlsfAllocator::mergeForward(TlsfBlockHeader* currHeader)
{
	if (currHeader == nullptr)
	{
		return nullptr;
	}



	TlsfBlock nextTlsfBlock = getNextTlsfBlock(currHeader);

	if (nextTlsfBlock.header == nullptr)
	{
		return nullptr; // No next block to merge with
	}

	TlsfBlockHeader* nextHeader = nextTlsfBlock.header;
	if (!nextHeader->isFree)
	{ //need more thought here
		return nullptr; // Next block is not free, cannot merge
	}

	removeFromFreeList(nextHeader);


	Layout nextBlockLayout = calculateLayout(reinterpret_cast<size_t>(nextHeader)-nextHeader->rawOffset, nextHeader->UserAreaSize);


	const size_t newRawBlockSize = currHeader->rawBlockSize + nextHeader->rawBlockSize;

	const size_t newUserAreaSize = nextBlockLayout.userAreaExclusiveEnd - (reinterpret_cast<size_t>(currHeader) + BLOCK_HEADER_SIZE);


	TlsfFooter* newFooter = reinterpret_cast<TlsfFooter*>(nextBlockLayout.footerStartAddress);

	TlsfBlockHeader* newHeader = currHeader;

	newHeader->rawBlockSize = newRawBlockSize;
	newHeader->UserAreaSize = newUserAreaSize;

	newHeader->nextFreeBlock = nullptr; // i still need to hash out who is responsible for setting this
	newHeader->prevFreeBlock = nullptr;

	newHeader->isFree = true; 


	newFooter->rawBlocksize = newRawBlockSize; 

	return newHeader; // Return the new merged header
}


TlsfBlockHeader* TlsfAllocator::mergeBackward(TlsfBlockHeader* currHeader)
{
	if (currHeader == nullptr)
	{
		return nullptr;
	}



	TlsfBlock prevTlsfBlock = getPreviousTlsfBlock(currHeader);

	TlsfBlockHeader* prevHeader = prevTlsfBlock.header;

	if (prevHeader == nullptr)
	{
		return nullptr; 
	}



	//Need more thought here
	if (!prevHeader->isFree)
	{
		
		return nullptr; 
	}


	//remove prevBlock from free list
	removeFromFreeList(prevHeader);


	Layout currBlockLayout = calculateLayout(reinterpret_cast<size_t>(currHeader) - currHeader->rawOffset, currHeader->UserAreaSize);

	const size_t newRawBlockSize = prevHeader->rawBlockSize + currHeader->rawBlockSize;
	const size_t newUserAreaSize = currBlockLayout.userAreaExclusiveEnd - (reinterpret_cast<size_t>(prevHeader)+ BLOCK_HEADER_SIZE);

	TlsfBlockHeader* newHeader = prevHeader;
	TlsfFooter* newFooter = reinterpret_cast<TlsfFooter*>(currBlockLayout.footerStartAddress);

	newHeader->rawBlockSize = newRawBlockSize;
	newHeader->UserAreaSize = newUserAreaSize;

	newHeader->nextFreeBlock = nullptr; // i still need to hash out who is responsible for setting this
	newHeader->prevFreeBlock = nullptr; 

	newHeader->isFree = true; 
	newFooter->rawBlocksize = newRawBlockSize; 

	return newHeader; // Return the new merged header
}

TlsfBlockHeader* TlsfAllocator::coalesceBlocks(TlsfBlockHeader* currHeader)
{ 

	//check for nullptr
	if (currHeader == nullptr)
	{
		return nullptr;
	}

	//check forward merge
	 // merge
	while (checkForwardMerge(currHeader))
	{
		currHeader = mergeForward(currHeader);
		if (currHeader == nullptr) break;
	}



	//check backward merge
		  //merge
	while (checkBackwardMerge(currHeader))
	{
		currHeader = mergeBackward(currHeader);
		if (currHeader == nullptr) break;
	}

	return currHeader;
}

TlsfBlockHeader* TlsfAllocator::getFreeBlock(const size_t requiredSize)
{
	
	const size_t fLIndex = getFirstLevelIndex(requiredSize);//culprit?



	if (m_firstLevelBitmap & (1ULL << fLIndex))// if there is atleast one subBin with a block
	{
		const auto& secondLevelBitMap = m_secondLevelBitmap[fLIndex];

		 size_t sLI = getSecondLevelIndex(requiredSize);
		
		while (true)
		{
			if (secondLevelBitMap & 1ULL << sLI)
			{
				//get the block from that sli
				//remove from freelist

				//need to find the first fit 

				TlsfBlockHeader* header = m_freeList[fLIndex][sLI];

				while (header != nullptr)
				{
					if (header->UserAreaSize >= requiredSize)
					{
						removeFromFreeList(header);
						return header;
					}
					header = header->nextFreeBlock;
				}



			}
			++sLI;
			if (sLI >= sizeof(secondLevelBitMap) * 8) break;

		}
	}



	//if excecution reaches here it means there is no blocks required size or greater in fLI so go to next.

	size_t newFLIndex = fLIndex ;
	while (true)
	{
		newFLIndex++;
		if (newFLIndex >= sizeof(m_firstLevelBitmap) * 8) break;

		if (m_firstLevelBitmap & (1ULL << newFLIndex))
		{
			const auto& secondLevelBitMap = m_secondLevelBitmap[newFLIndex];
			size_t sLI = 0;
			while (true)
			{
				
				if (secondLevelBitMap & 1ULL << sLI)
				{
					//get the block from that sli
					//remove from freelist
	
					TlsfBlockHeader* header = m_freeList[newFLIndex][sLI];
					

					while (header != nullptr)
					{
						if (header->UserAreaSize >= requiredSize)
						{
					removeFromFreeList(header);
					return header;
				}
						header = header->nextFreeBlock;
					}
				}
				sLI++;
				if (sLI >= sizeof(secondLevelBitMap) * 8) break;
			}

		}

	}



	//unfortunately the allocator failed to find a free block that satisfies the requirements

	return nullptr;


}





TlsfAllocator::TlsfAllocator(const size_t memoryPoolSize) // in bytes
{
	//for our first iteration we will use malloc
	
	void* ptr = reinterpret_cast<void*>(malloc(memoryPoolSize));

	if (ptr == nullptr) return; // not pretty 


	//construct start block
	

	const Layout startBlockLayout = calculateLayout(reinterpret_cast<size_t>(ptr), 0);

	
	TlsfBlockHeader* startHeader = reinterpret_cast<TlsfBlockHeader*>(startBlockLayout.HeaderStartAddress);

	startHeader->isFree = false;
	startHeader->nextFreeBlock = nullptr;
	startHeader->prevFreeBlock = nullptr;

	startHeader->rawBlockSize = startBlockLayout.rawExclusiveEndAddress - startBlockLayout.rawStartAddress;

	startHeader->UserAreaSize = 0;

	startHeader->rawOffset = startBlockLayout.paddingHeader;



	TlsfFooter* startFooter = reinterpret_cast<TlsfFooter*>(startBlockLayout.footerStartAddress);
	startFooter->rawBlocksize = startHeader->rawBlockSize;

	TlsfBlock startBlock;

	startBlock.rawStartAddress = startBlockLayout.rawStartAddress;
	startBlock.header = startHeader;
	startBlock.userArea = nullptr;
	startBlock.footer = startFooter;

	m_startTlsfBlock = startBlock;
	//construct end block

	

	const size_t rawBlockEndAddress = reinterpret_cast<size_t>(ptr) + memoryPoolSize ;


	size_t endBlockPotentialStartAdress = rawBlockEndAddress - BLOCK_HEADER_SIZE;

	if (endBlockPotentialStartAdress % ALIGNMENT_REQ_BLOCK_HEADER)
	{
		endBlockPotentialStartAdress -= (endBlockPotentialStartAdress % ALIGNMENT_REQ_BLOCK_HEADER);
	}

	//ToDo
	//check if  endBlockPotentialStartAdress less than start or if overlapping into start block

	TlsfBlockHeader* endBlockHeader = reinterpret_cast<TlsfBlockHeader*>(endBlockPotentialStartAdress);

	
	const size_t rawOffset = (endBlockPotentialStartAdress % ALIGNMENT_REQ_FOOTER);

	endBlockHeader->isFree = false;
	endBlockHeader->nextFreeBlock = nullptr;
	endBlockHeader->prevFreeBlock = nullptr;

	endBlockHeader->rawBlockSize = rawBlockEndAddress -(endBlockPotentialStartAdress - rawOffset) ;
	endBlockHeader->rawOffset = rawOffset;

	TlsfBlock endBlock;

	endBlock.rawStartAddress = (endBlockPotentialStartAdress - rawOffset);
	endBlock.header = endBlockHeader;

	
	m_endTlsfBlock = endBlock;

	

	//now construct the usbale block

	const size_t usableRawStartAddress = reinterpret_cast<size_t>(startBlock.footer) + FOOTER_SIZE;

	//this address is calculated such that it is aligned for Footer 
	const size_t usableRawEndAddress = endBlock.rawStartAddress;



	TlsfBlockHeader* usableHeader = createTlsfBlock(usableRawStartAddress, usableRawEndAddress);
	
	if (usableHeader)
	{
		storeInFreeList(usableHeader);
	}
	
}

TlsfAllocator::~TlsfAllocator()
{

	//return the whole memory  to os or malloc

	void* poolPtr = reinterpret_cast<void*>(m_startTlsfBlock.rawStartAddress);

	free(poolPtr);
}







void* TlsfAllocator::allocate(size_t size)
{
	


	TlsfBlockHeader* allocatedBlock = getFreeBlock(size);
	
	if (allocatedBlock == nullptr) return nullptr;// allocation failed






	const size_t rawStartAddress =( reinterpret_cast<size_t>(allocatedBlock)-(allocatedBlock->rawOffset));
	const size_t rawEndAddress = rawStartAddress + allocatedBlock->rawBlockSize;


	const Layout bestFitLayout = calculateLayout(rawStartAddress, size);

	const size_t remainingBytes = allocatedBlock->rawBlockSize - (bestFitLayout.rawExclusiveEndAddress - bestFitLayout.rawStartAddress);//unsafe,can wrap around


	//check if the difference between best fit calculated and current allocated has greater difference than a predefined minimum allocatable block

	if (MINIMUM_VALID_TLSF_BLOCK_RAW_SIZE > remainingBytes)
	{
		const size_t headerStartAddress = reinterpret_cast<size_t>(allocatedBlock);
		
		return reinterpret_cast<void*> (headerStartAddress + BLOCK_HEADER_SIZE);
	}
	else
	{
		//split 
		TlsfBlockHeader* userBlock = createTlsfBlock(bestFitLayout.rawStartAddress, bestFitLayout.rawExclusiveEndAddress);
		
		TlsfBlockHeader* remainingBlock = createTlsfBlock( bestFitLayout.rawExclusiveEndAddress, rawEndAddress);//culprit
	
		if (userBlock == nullptr) return nullptr; // allocation failed !!

		userBlock->isFree = false;
		//store one in freelist
		storeInFreeList(remainingBlock);//it will check for nullptr 
		
		//return the carved out chunk
		return reinterpret_cast<void*>( reinterpret_cast<size_t>(userBlock)+BLOCK_HEADER_SIZE);
	}


}




void TlsfAllocator::deallocate(void* ptr)
{
	if (ptr == nullptr)
	{
		return; 
	}


	//currently our design does not align user start address and it is intentional
	//hence header will be just just before user area
	const size_t userAreaStartAddress = reinterpret_cast<size_t>(ptr);
	TlsfBlockHeader* header = reinterpret_cast<TlsfBlockHeader*>(userAreaStartAddress - BLOCK_HEADER_SIZE);


	TlsfBlockHeader* mergedBlockHeader = coalesceBlocks(header);



	storeInFreeList(mergedBlockHeader);


}
