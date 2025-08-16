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

	const size_t ALLOCATION_SIZE = 1024 * 5; //5K KB

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









