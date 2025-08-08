
#include "tlsf.h"



void* TlsfAllocator::allocate(size_t size)
{

}

void TlsfAllocator::deallocate(void* ptr)
{
	//get header
	//see if it can be collased
	//if not store in free list
}


Layout TlsfAllocator::calculateLayout(void* ptr, size_t size) const
{
	Layout layout;

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
	layout.paddingFooter = (ALIGNMENT_REQ_FOOTER - ((layout.userAreaExclusiveEnd + FOOTER_SIZE) % ALIGNMENT_REQ_FOOTER)) % ALIGNMENT_REQ_FOOTER;

	// Footer start address
	layout.FooterStartAddress = layout.userAreaExclusiveEnd + layout.paddingFooter;

	// Raw end address
	layout.rawExclusiveEndAddress = layout.FooterStartAddress + FOOTER_SIZE;

	return layout;
}