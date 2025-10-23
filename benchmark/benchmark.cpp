
#include <iostream>
#include <string>
#include <iomanip>
#include <fstream>
#include <functional>
#include <chrono>
#include "tlsf.h"
#include <allocators/tlsf_allocator.hpp>
#include <matt_conte_tlsf.h>
#include <rpmalloc.h>


class Timer
{
private:

    std::chrono::time_point<std::chrono::high_resolution_clock>  startTime;
    std::chrono::time_point<std::chrono::high_resolution_clock>  endTime;

public:
    void start()
    {
        startTime = std::chrono::high_resolution_clock::now();
    };
    void stop()
    {
        endTime = std::chrono::high_resolution_clock::now();

    };

    double getTime()
    {
        return std::chrono::duration_cast<std::chrono::nanoseconds>(endTime - startTime).count();
    };
};





// Global or class member
std::ofstream dataFile;

void initializeCSVFile(const std::string& filename)
{
    dataFile.open(filename, std::ios::out | std::ios::trunc);
    if (!dataFile.is_open())
    {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        return;
    }
    // Write the CSV Header
    dataFile << "Allocator,Operation,RequestedSize,Duration_ns\n";
    // Ensure all subsequent outputs use high precision for timing data
    dataFile << std::fixed << std::setprecision(6);
}

void finalizeCSVFile()
{
    if (dataFile.is_open())
    {
        dataFile.close();
    }
}

void logBenchmarkResult(
    const std::string& allocatorName,
    const std::string& operation,
    size_t requestedSize,
    double duration_ns)
{
    if (dataFile.is_open())
    {
        dataFile << allocatorName << ","
            << operation << ","
            << requestedSize << ","
            << duration_ns << "\n";
    }
}




const size_t POOL_SIZE = 1024 * 1024 * 500;      // 500MiB Pool (As requested)
//const int ALLOCATION_COUNT = 5000;            // Number of allocations to perform
const size_t ALLOCATION_SIZE = 1024 * 40;

const unsigned int MAX_RUNS = 10000;



 void runBenchmark(
    const std::string& name,
    std::function<void* (size_t)> alloc_func,
    std::function<void(void*)> dealloc_func)
{



    struct BenchMarkResult
    {
        std::string name;
        std::string operation;

        size_t requestedSize = 0;

        double nsDuration = 0;

    };



    auto outputToFile = [](BenchMarkResult* benchMarkResults, unsigned int count)
        {
            for (unsigned int i = 0; i < count; ++i)
            {
                auto& benchMarkResult = benchMarkResults[i];
                logBenchmarkResult(benchMarkResult.name, benchMarkResult.operation, benchMarkResult.requestedSize, benchMarkResult.nsDuration);
            }
        };





   


   const size_t ALLOCATION_COUNT = MAX_RUNS;

   std::unique_ptr<BenchMarkResult[]> benchmarkResultArray =
       std::make_unique<BenchMarkResult[]>(ALLOCATION_COUNT);

   // --- FIX 2: Allocate pointers array on the HEAP ---
   std::unique_ptr<void* []> pointers =
       std::make_unique<void* []>(ALLOCATION_COUNT);

  //  std::cout << "\n--- Starting " << name << " Benchmark ---" << std::endl;

    // --- Phase 1: Allocation ---
    Timer timer{};
    int lastRunI = 0;
    for (int i = 0; i < ALLOCATION_COUNT; ++i)
    {
        lastRunI = i;
        timer.start();
        pointers[i] = alloc_func(ALLOCATION_SIZE);
        timer.stop();


        benchmarkResultArray[i].name = name;
        benchmarkResultArray[i].operation = "allocate";

        benchmarkResultArray[i].requestedSize = ALLOCATION_SIZE;

        benchmarkResultArray[i].nsDuration = timer.getTime();

        // Check for allocation failure (especially crucial for native/real custom allocators)
        if (!pointers[i])
        {
            std::cerr << "Error: Allocation failed at index " << i << std::endl;
            // Clear successful allocations to avoid leaks
            for (int j = 0; j < i; ++j)
            {
                dealloc_func(pointers[j]);
            }
            return;
        }
    }


    outputToFile(benchmarkResultArray.get(), lastRunI);


    // --- Phase 2: Deallocation ---


    for (int i = 0; i < ALLOCATION_COUNT; ++i)
    {
        if (pointers[i])
        {
            dealloc_func(pointers[i]);
        }
    }



}


// Custom function wrappers for std::malloc/std::free to fit the std::function signature
void* native_alloc(size_t size)
{
    return std::malloc(size);
}

void native_dealloc(void* ptr)
{
    std::free(ptr);
}

int main()
{









    initializeCSVFile("benchmark_result.csv");

    runBenchmark(
        "Malloc",
        native_alloc,
        native_dealloc
    );

    TlsfAllocator tlsfAllocator(POOL_SIZE);

    runBenchmark(
        "Custom Tlsf Allocator",
        [&tlsfAllocator](size_t size) { return tlsfAllocator.allocate(size); },
        [&tlsfAllocator](void* ptr) { tlsfAllocator.deallocate(ptr); }
    );


    finalizeCSVFile();
}
