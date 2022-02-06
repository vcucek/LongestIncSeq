#include <string>
#include <string.h>
#include <assert.h>
#include <chrono>
#include <iostream>

#include "FloatSequence.hpp"
#include "SequenceAnalyser.hpp"

static SequenceAnalyserCUDA analyserCuda;
static SequenceAnalyserCPU analyserCpu;

void printResult(const std::string &title, Interval &interval)
{
    printf("%s\n", title.c_str());
    printf("Longest continous increasing sequence:\n");
    printf("	Start index: %d\n", interval.startIndex);
    printf("	End index: %d\n", interval.endIndex);
    printf("	Interval length: %d\n", interval.endIndex - interval.startIndex);
}

void testSeq(FloatSequence &seq)
{
    Interval resultCpu = analyserCpu.calcLongestIncInterval(seq);
    printResult("CPU", resultCpu);
    Interval resultCuda = analyserCuda.calcLongestIncInterval(seq);
    printResult("CUDA", resultCuda);
    assert(SequenceAnalyser::intervalEquals(resultCuda, resultCpu));
}

void testArray(std::vector<float> data)
{
    FloatSequence seq(data);
    std::cout << "Testing predefined sequence:" << std::endl;
    seq.print(20);
    testSeq(seq);
}

void testPredefinedData()
{
    // Check multiple results
    testArray({1, 2, 1, 2, 1, 2, 1, 2});
    testArray({0.58635, 0.24096, 0.780683, 0.0204995, 0.669729, 0.114543, 0.183372, 0.195734});
    testArray({0.629432, 0.086141, 0.63704, 0.453759, 0.0106824, 0.99078, 0.86038, 0.631292});

    // Check batch boundary
    analyserCuda.batchSize = 5;
    testArray({1, 1, 1, 1, 1, 2, 1, 2, 3, 4, 5, 5, 5, 5, 5});
    testArray({1, 1, 1, 1, 1, 2, 3, 4, 5, 6, 5, 5, 5, 5, 5});
    testArray({1, 1, 1, 1, 1, 2, 1, 2, 3, 4, 4, 4, 4, 4, 4});
    testArray({1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 2, 3, 4, 5, 6});
    testArray({1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 2, 2, 2, 2, 2});
    testArray({1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 2, 2, 2, 2, 2});
    testArray({1, 1, 1, 1, 2, 1, 2, 3, 4, 5, 1, 2, 2, 2, 2});
    testArray({1, 1, 1, 1, 2, 1, 2, 3, 4, 5, 1, 2, 3, 4, 5});
    testArray({1, 1, 1, 1, 2, 3, 4, 5, 1, 2, 3, 4, 5, 2, 2});
    testArray({1, 1, 1, 1, 1, 3, 4, 5, 1, 2, 3, 4, 5, 2, 2});

    // Check passing multiple batch boundaries
    analyserCuda.batchSize = 5;
    testArray({1, 1, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 11, 11});

    // Check full interval
    testArray({-1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17});
    // Check empty interval
    testArray({15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, -1, -2});

    // Check batch size larger then input array
    analyserCuda.batchSize = 1024;
    testArray({1, 2, 1, 2, 1, 2, 1, 2});

    // Check interval at the beginning
    testArray({1, 2, 3, 0, 0, 0, 0, 0});

    // Check interval at the end
    testArray({0, 0, 0, 0, 0, 1, 2, 3});
}

void testRandomData(long size)
{
    FloatSequence seq;
    if (!seq.randomize(size))
    {
        std::cerr << "ERROR: Generating random sequence data! Exiting..." << std::endl;
        exit(1);
    }

    std::cout
        << "Testing random data sequence with "
        << size << " elements." << std::endl;
    seq.print(20);

    using std::chrono::duration;
    using std::chrono::duration_cast;
    using std::chrono::high_resolution_clock;
    using std::chrono::milliseconds;

    Interval resultCpu;
    Interval resultCuda;
    {
        auto t1 = high_resolution_clock::now();
        resultCpu = analyserCpu.calcLongestIncInterval(seq);
        auto t2 = high_resolution_clock::now();
        printf("\n");
        printResult("Using CPU:", resultCpu);
        std::cout << "Compute time: " << duration_cast<milliseconds>(t2 - t1).count() << std::endl;
    }

    {
        auto t1 = high_resolution_clock::now();
        resultCuda = analyserCuda.calcLongestIncInterval(seq);
        auto t2 = high_resolution_clock::now();
        printf("\n");
        printResult("Using CUDA:", resultCuda);
        std::cout << "Compute time: " << duration_cast<milliseconds>(t2 - t1).count() << std::endl;
    }
    assert(SequenceAnalyser::intervalEquals(resultCuda, resultCpu));
}

int main(int argc, char ** argv)
{
    if (argc == 4 && !strcmp(argv[1], "random"))
    {
        long randomSequenceSize = atoi(argv[2]);
        long batchSize = atoi(argv[3]);
        analyserCuda.batchSize = batchSize;
        testRandomData(randomSequenceSize);
    }
    else
    {
        testPredefinedData();
    }
	return 0;
}