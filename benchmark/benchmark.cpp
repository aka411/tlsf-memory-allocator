#include <benchmark/benchmark.h>
#include <tlsf.h>
#include <iostream>





class MyTLSFAllocatorFixture : public benchmark::Fixture
{
public:
    TlsfAllocator* m_tlsf;
	const size_t POOL_SIZE = 1024 * 1024 * 4;// 4 MiB pool size
    void SetUp(const ::benchmark::State& state) 
    {
        m_tlsf = new TlsfAllocator(POOL_SIZE);
    }
    void TearDown(const ::benchmark::State& state) 
    {
        delete m_tlsf;
    }
};


static void SystemMalloc_AllocateAndFree(benchmark::State& state)
{
    const size_t ALLOCATION_SIZE = state.range(0);
    for (auto _ : state)
    {
        void* p = std::malloc(ALLOCATION_SIZE);
 
        benchmark::DoNotOptimize(p);
        std::free(p);
    }
}

BENCHMARK(SystemMalloc_AllocateAndFree)
    ->Args({900})              // 900 Bytes
    ->Args({1024 * 4})        // 4 KiB
    ->Args({1024 * 40})       // 40 KiB
    ->Args({1024 * 512})      // 512 KiB
    ->Args({1024 * 1024});    // 1 MiB



BENCHMARK_DEFINE_F(MyTLSFAllocatorFixture, MyTlsf_AllocateAndFree) (benchmark::State& state)
{
    const size_t ALLOCATION_SIZE = state.range(0);
    for (auto _ : state)
    {
        void* p = m_tlsf->allocate(ALLOCATION_SIZE);
        benchmark::DoNotOptimize(p);
        m_tlsf->deallocate(p);
    }
}
BENCHMARK_REGISTER_F(MyTLSFAllocatorFixture, MyTlsf_AllocateAndFree)
    ->Args({900}) 
    ->Args({1024 * 4})
    ->Args({1024 * 40})
    ->Args({1024 * 512})
    ->Args({1024 * 1024});


//BENCHMARK_MAIN();



int main(int argc, char** argv)
{
    // 1. Initialize and run the benchmarks
    benchmark::Initialize(&argc, argv);
    if (benchmark::ReportUnrecognizedArguments(argc, argv)) return 1;

	
    bool success = benchmark::RunSpecifiedBenchmarks();
    
    std::cout << "\n\n--- Benchmarks Finished ---\n\n";



    // Wait for the user to press ENTER
   // std::cin.get();

   return success ? 0 : 1;
}

