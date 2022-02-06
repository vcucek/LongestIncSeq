#include <string>
#include <string.h>
#include <chrono>
#include <iostream>

#include "FloatSequence.hpp"
#include "SequenceAnalyser.hpp"

static SequenceAnalyserCUDA analyserCuda;
static SequenceAnalyserCPU analyserCpu;

static void findLongestIncInterval(SequenceAnalyser &analyeser, FloatSequence &seq)
{
    using std::chrono::duration;
    using std::chrono::duration_cast;
    using std::chrono::high_resolution_clock;
    using std::chrono::milliseconds;

    auto t1 = high_resolution_clock::now();
    Interval result = analyeser.calcLongestIncInterval(seq);
    auto t2 = high_resolution_clock::now();
    std::cout
        << "Longest continous increasing sequence:" << std::endl
        << "	Start index: " << result.startIndex << std::endl
        << "	End index: " << result.endIndex << std::endl
        << "	Interval length: " << result.endIndex - result.startIndex << std::endl
        << "Compute time: " << duration_cast<milliseconds>(t2 - t1).count() << " ms" << std::endl;
}

static void findLongestIncInterval(SequenceAnalyser &analyeser, char *url)
{
    // Read and initialize input data from file
    FloatSequence seq;
    if (!seq.read(std::string(url)))
    {
        std::cout << "ERROR: Reading input file data! Exiting...(1)" << std::endl;
        exit(1);
    }
    findLongestIncInterval(analyeser, seq);
}

static void printHelp()
{
    std::cout
        << "Find longest continuous increasing sequence." << std::endl
        << "Usages:" << std::endl
        << "    LongestSqc [FILE]" << std::endl
        << "    LongestSqc cpu [FILE]" << std::endl
        << "    LongestSqc cuda [FILE]" << std::endl
        << "" << std::endl
        << "The specified FILE must contain tightly packed 32bit little endian float values." << std::endl;
}

static void argErrorExit()
{
    std::cerr << "Invalid arguments!" << std::endl;
    exit(1);
}

int main(int argc, char **argv)
{
    if (argc <= 1)
    {
        printHelp();
        exit(1);
    }

    if (argc == 2)
    {
        findLongestIncInterval(analyserCuda, argv[1]);
    }
    else if (argc >= 3)
    {
        if (!strcmp(argv[1], "cpu"))
        {
            findLongestIncInterval(analyserCpu, argv[2]);
        }
        else if (!strcmp(argv[1], "cuda"))
        {
            findLongestIncInterval(analyserCuda, argv[2]);
        }
        else
        {
            argErrorExit();
        }
    }
    else
    {
        argErrorExit();
    }
    return 0;
}
