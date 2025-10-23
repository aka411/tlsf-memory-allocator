#include <benchmark/benchmark.h>
#include <tlsf.h>
#include <iostream>





class AllocatorFixture : public benchmark::Fixture
{
public:
    TlsfAllocator* tlsf;
	const size_t POOL_SIZE = 1024 * 1024 * 1;// 1 MiB pool size
    void SetUp(const ::benchmark::State& state) 
    {
        tlsf = new TlsfAllocator(POOL_SIZE);
    }
    void TearDown(const ::benchmark::State& state) 
    {
        delete tlsf;
    }
};


static void BM_System_Malloc(benchmark::State& state)
{
    const size_t ALLOCATION_SIZE = state.range(0);
    for (auto _ : state)
    {
        void* p = std::malloc(ALLOCATION_SIZE);
 
        benchmark::DoNotOptimize(p);
        std::free(p);
    }
}

BENCHMARK(BM_System_Malloc)->Arg( 1024 * 40 );//40 KiB



BENCHMARK_DEFINE_F(AllocatorFixture, BM_Tlsf_AllocateAndFree) (benchmark::State& state)
{
    const size_t ALLOCATION_SIZE = state.range(0);
    for (auto _ : state)
    {
        void* p = tlsf->allocate(ALLOCATION_SIZE);
        benchmark::DoNotOptimize(p);
        tlsf->deallocate(p);
    }
}
BENCHMARK_REGISTER_F(AllocatorFixture, BM_Tlsf_AllocateAndFree)->Arg(1024 * 40);// 40 KiB


//BENCHMARK_MAIN();



int main(int argc, char** argv)
{
    // 1. Initialize and run the benchmarks
    benchmark::Initialize(&argc, argv);
    if (benchmark::ReportUnrecognizedArguments(argc, argv)) return 1;
    benchmark::RunSpecifiedBenchmarks();

    
    std::cout << "\n\n--- Benchmarks Finished ---\nPress ENTER to exit...";



    // Wait for the user to press ENTER
    std::cin.get();

    return 0;
}

