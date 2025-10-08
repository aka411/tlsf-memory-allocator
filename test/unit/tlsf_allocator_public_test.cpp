#include "gtest/gtest.h"
#include "tlsf.h"
#include <vector>        // For std::vector
#include <algorithm>     // For std::shuffle
#include <random>        // For std::default_random_engine
#include <chrono>        // For seeding the random engine

class TlsfAllocatorTestPublic : public ::testing::Test
{
protected: 
	TlsfAllocator* tlsfAllocatorInstance = nullptr;

	const size_t ALLOCATION_SIZE = 1024 * 1024; //1 MiB

	void SetUp() override
	{

		tlsfAllocatorInstance = new TlsfAllocator(ALLOCATION_SIZE);

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





};



TEST_F(TlsfAllocatorTestPublic, AllocationGracefullFail)
{

	const size_t userAreaSizeEqualToTotalAllocatorPool = ALLOCATION_SIZE;

	TlsfAllocator* tlsfAllocator = getTlsfAllocator();

	





	//pressure tesst without deallocation
	void* ptr = tlsfAllocator->allocate(userAreaSizeEqualToTotalAllocatorPool);

	bool allocationFailed = false;

	if (ptr == nullptr) allocationFailed = true;

	// Assert allocation  failed
	ASSERT_TRUE(allocationFailed);

}




TEST_F(TlsfAllocatorTestPublic, ContinousAllocationDeallocation)
{
	const size_t requestSizeLessThanAllocatorPool = ALLOCATION_SIZE / 100;

	TlsfAllocator* tlsfAllocator = getTlsfAllocator();

	bool allocationFailed = false;
	void* ptr = nullptr;
	for (size_t i = 0; i < 1000; ++i)
	{
		ptr = tlsfAllocator->allocate(requestSizeLessThanAllocatorPool);
		
		if (ptr == nullptr)
		{

			allocationFailed = true;
			break; // Stop once the expected failure occurs
		}
		tlsfAllocatorInstance->deallocate(ptr);
	}

	ASSERT_FALSE(allocationFailed);
}











TEST_F(TlsfAllocatorTestPublic, ContinousAllocationDeallocationVaryingSize)
{
	const size_t baseRequestSize = ALLOCATION_SIZE / 100;
	const size_t variationMax = (baseRequestSize * 0.3);

	TlsfAllocator* tlsfAllocator = getTlsfAllocator();
	std::vector<void*> allocatedPointers;


	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	std::default_random_engine rng(seed);
	std::uniform_int_distribution<size_t> dist(0, variationMax);



	// Phase 0: Try to allocate a large block to see if before fragmentation test
	void* largePtrPreFragmentation = tlsfAllocator->allocate(ALLOCATION_SIZE / 2);
	ASSERT_NE(largePtrPreFragmentation, nullptr) << "Failed to allocate a large block of size "<< ALLOCATION_SIZE /(2*1024.0) <<" kB before fragmentation test.";
	if (largePtrPreFragmentation)
	{
		tlsfAllocator->deallocate(largePtrPreFragmentation);
	}


	// Phase 1: Allocate varying-sized blocks
	for (size_t i = 0; i < 39; ++i)
	{
		size_t varyingSize = baseRequestSize + dist(rng);


		// Print the iteration and size before the allocation attempt
		std::cout << "Attempting to allocate " << varyingSize << " bytes on iteration " << i << std::endl;
		
		void* ptr = tlsfAllocator->allocate(varyingSize);
		if (ptr == nullptr)
		{
			FAIL() << "Failed to allocate block of size " << varyingSize
				<< " on iteration " << i << ".";
		}
		allocatedPointers.push_back(ptr);
	}

	// Phase 2: Deallocate in a non-sequential, random order to induce fragmentation
	std::shuffle(allocatedPointers.begin(), allocatedPointers.end(), rng);
	for (void* ptr : allocatedPointers)
	{
		tlsfAllocator->deallocate(ptr);
	}

	// Phase 3: Try to allocate a large block to see if fragmentation occurred
	void* largePtr = tlsfAllocator->allocate(ALLOCATION_SIZE / 2);
	ASSERT_NE(largePtr, nullptr) << "Failed to allocate a large block after fragmentation test.";
	if (largePtr)
	{
		tlsfAllocator->deallocate(largePtr);
	}
}







TEST_F(TlsfAllocatorTestPublic, DataIntegrityTest)
{

	TlsfAllocator* tlsfAllocator = getTlsfAllocator();

	const size_t blockSize1 = 900;//in number of bytes
	const size_t blockSize2 = 1500;//in number of bytes

	void* dataBlockPtr1 = tlsfAllocator->allocate(blockSize1);

	void* dataBlockPtr2 = tlsfAllocator->allocate(blockSize2);


	const uint8_t pattern1 = 34;
	const uint8_t pattern2 = 130;
	
	memset(dataBlockPtr1, pattern1, blockSize1);
	memset(dataBlockPtr2, pattern2, blockSize2);


	for (int i = 0; i <1000 ; ++i)
	{
		void* tempPtr1 = tlsfAllocator->allocate(i+50);
		memset(tempPtr1, i, i + 50);
		tlsfAllocator->deallocate(tempPtr1);
	}



	for (size_t i = 0; i < blockSize1; ++i)
	{
		ASSERT_EQ(((uint8_t*)dataBlockPtr1)[i], pattern1);
	}

	for (size_t i = 0; i < blockSize2; ++i)
	{
		ASSERT_EQ(((uint8_t*)dataBlockPtr2)[i], pattern2);
	}

	// Clean up: Deallocate the original blocks.
	tlsfAllocator->deallocate(dataBlockPtr1);
	tlsfAllocator->deallocate(dataBlockPtr2);

}

