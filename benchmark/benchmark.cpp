
#include <benchmark/benchmark.h>
#include <tlsf.h>
#include <iostream>
#include <random>





class MyTLSFAllocatorFixture : public benchmark::Fixture
{
public:
    TlsfAllocator* m_tlsf;
    const size_t POOL_SIZE = 1024 * 1024 *50;// 50 MiB pool size
    void SetUp(const ::benchmark::State& state)
    {
        m_tlsf = new TlsfAllocator(POOL_SIZE);
    }
    void TearDown(const ::benchmark::State& state)
    {
        delete m_tlsf;
    }
};







/******* Mixed Allocation *********/


BENCHMARK_DEFINE_F(MyTLSFAllocatorFixture, MyTlsf_AllocDeallocCycle_MixedSize) (benchmark::State& state)
{
    const size_t NUM_ALLOCATIONS = state.range(0);
    const size_t MAX_ALLOC_SIZE = 1024 * 128; // Max 128 KiB per block

    std::mt19937 gen(28);
	std::uniform_int_distribution<> distrib(20, MAX_ALLOC_SIZE);//fom 20 bytes to 128 KiB
	std::vector<void*> allocated_pointers;
   
    allocated_pointers.reserve(NUM_ALLOCATIONS);

    for (auto _ : state)
    {
        // 1. ALLOCATION PHASE (TIME MEASURED)
        for (size_t i = 0; i < NUM_ALLOCATIONS; ++i)
        {
            size_t size = distrib(gen);
            void* p;

 
            p = m_tlsf->allocate(size);

            if (p == nullptr)
            {
                // Skips this benchmark run iteration gracefully
                state.SkipWithError("TLSF Allocation failed (Pool Exhaustion).");
                return;
            }
            benchmark::DoNotOptimize(p); 
        
           
            allocated_pointers.push_back(p);
        }


        

        for (void* p : allocated_pointers)
        {
            m_tlsf->deallocate(p);
        }
        allocated_pointers.clear();
        
     
    }



}

BENCHMARK_REGISTER_F(MyTLSFAllocatorFixture, MyTlsf_AllocDeallocCycle_MixedSize)
->Args({ 10 })
->Args({ 20 })
->Args({ 30 })
->Args({ 40 })
->Args({ 50 })
->Args({ 60 })
->Args({ 70 })
->Args({ 80 })
->Args({ 90 })
->Args({ 100 })
->Args({ 110 })
->Args({ 120 })
->Args({ 130 })
->Args({ 140 })
->Args({ 150 })
->Args({ 160 })
->Args({ 170 })
->Args({ 180 })
->Args({ 190 })
->Args({ 200 });




static void SystemMalloc_AllocDeallocCycle_MixedSize(benchmark::State& state)
{
    const size_t NUM_ALLOCATIONS = state.range(0);
    const size_t MAX_ALLOC_SIZE = 1024 * 128; // Max 128 KiB per block


    std::vector<void*> pointers;
    pointers.reserve(NUM_ALLOCATIONS);

    // Random number generation setup (fixed seed for stable testing)
    std::mt19937 gen(28);
	std::uniform_int_distribution<> distrib(20, MAX_ALLOC_SIZE);// from 20 bytes to 128 KiB


    for (auto _ : state)
    {
        // 1. ALLOCATION PHASE (TIME MEASURED)
        for (size_t i = 0; i < NUM_ALLOCATIONS; ++i)
        {
            size_t size = distrib(gen);
            void* p;

 
            p = std::malloc(size); // <<<--- MALLOC CALL

            if (p == nullptr)
            {
                state.SkipWithError("System Malloc failed (OOM).");
                return;
            }
            benchmark::DoNotOptimize(p);
            pointers.push_back(p);
        }


       

        for (void* p : pointers)
        {
            std::free(p); // <<<--- FREE CALL
        }
        pointers.clear();


  
    }




}

BENCHMARK(SystemMalloc_AllocDeallocCycle_MixedSize)
->Args({ 10 })
->Args({ 20 })
->Args({ 30 })
->Args({ 40 })
->Args({ 50 })
->Args({ 60 })
->Args({ 70 })
->Args({ 80 })
->Args({ 90 })
->Args({ 100 })
->Args({ 110 })
->Args({ 120 })
->Args({ 130 })
->Args({ 140 })
->Args({ 150 })
->Args({ 160 })
->Args({ 170 })
->Args({ 180 })
->Args({ 190 })
->Args({ 200 });

//BENCHMARK_MAIN();








int main(int argc, char** argv)
{
    // 1. Initialize and run the benchmarks
    benchmark::Initialize(&argc, argv);
    if (benchmark::ReportUnrecognizedArguments(argc, argv)) return 1;
    
    bool success = benchmark::RunSpecifiedBenchmarks();
    
    std::cout << "\n\n--- Benchmarks Finished ---\nPress ENTER to exit...";



    // Wait for the user to press ENTER
    std::cin.get();

    return success ? 0 : 1;
}

