#include "gtest/gtest.h"
#include "tlsf.h"



class TlsfAllocatorInternalTest : public ::testing::Test
{
protected:
	TlsfAllocator* tlsfAllocatorInstance = nullptr;

	const size_t ALLOCATION_SIZE = 1024 * 5; //5K KB



	size_t HEADER_SIZE = 0;
	size_t HEADER_ALIGNMENT = 0;
	size_t FOOTER_SIZE = 0;
	size_t FOOTER_ALIGNMENT = 0;

	void SetUp() override
	{

		tlsfAllocatorInstance = new TlsfAllocator(ALLOCATION_SIZE);//5 KB

		HEADER_SIZE = tlsfAllocatorInstance->BLOCK_HEADER_SIZE;
		HEADER_ALIGNMENT = tlsfAllocatorInstance->ALIGNMENT_REQ_BLOCK_HEADER;
		FOOTER_SIZE = tlsfAllocatorInstance->FOOTER_SIZE;
		FOOTER_ALIGNMENT = tlsfAllocatorInstance->ALIGNMENT_REQ_FOOTER;

	}


	void TearDown() override
	{

		delete tlsfAllocatorInstance;
		tlsfAllocatorInstance = nullptr;
	}

public:

	TlsfAllocator* getTlsfAllocator()
	{
		return tlsfAllocatorInstance;
	}

	void CompareLayout(const Layout calculatedLayout, const Layout expectedLayout)
	{


		EXPECT_EQ(calculatedLayout.rawStartAddress, expectedLayout.rawStartAddress);
		EXPECT_EQ(calculatedLayout.paddingHeader, expectedLayout.paddingHeader);
		EXPECT_EQ(calculatedLayout.HeaderStartAddress, expectedLayout.HeaderStartAddress);
		EXPECT_EQ(calculatedLayout.userAreaStartAddress, expectedLayout.userAreaStartAddress);
		EXPECT_EQ(calculatedLayout.paddingFooter, expectedLayout.paddingFooter);
		EXPECT_EQ(calculatedLayout.footerStartAddress, expectedLayout.footerStartAddress);
		EXPECT_EQ(calculatedLayout.rawExclusiveEndAddress, expectedLayout.rawExclusiveEndAddress);
	}


	Layout CallCalculateLayout(const size_t startAddress, const size_t UserAreaSize) const
	{
		
		return tlsfAllocatorInstance->calculateLayout(startAddress, UserAreaSize);
	}





};



TEST_F(TlsfAllocatorInternalTest, LayoutCalculation)
{








	const size_t startAddress = 1023;
	const size_t UserAreaSize = 70;


	Layout expectedLayout;

	expectedLayout.rawStartAddress = 1023;
	expectedLayout.paddingHeader = ((HEADER_ALIGNMENT - (startAddress % HEADER_ALIGNMENT)) % HEADER_ALIGNMENT);
	expectedLayout.HeaderStartAddress = expectedLayout.paddingHeader + expectedLayout.rawStartAddress;
	expectedLayout.userAreaStartAddress = expectedLayout.HeaderStartAddress + HEADER_SIZE;
	expectedLayout.userAreaExclusiveEnd = expectedLayout.userAreaStartAddress + UserAreaSize;

	expectedLayout.paddingFooter = ((FOOTER_ALIGNMENT - (expectedLayout.userAreaExclusiveEnd % FOOTER_ALIGNMENT)) % FOOTER_ALIGNMENT);
	expectedLayout.footerStartAddress = expectedLayout.userAreaExclusiveEnd + expectedLayout.paddingFooter;

	expectedLayout.rawExclusiveEndAddress = expectedLayout.footerStartAddress + FOOTER_SIZE;

	const Layout calculatedLayout = CallCalculateLayout(startAddress, UserAreaSize);


	CompareLayout(calculatedLayout, expectedLayout);


}


