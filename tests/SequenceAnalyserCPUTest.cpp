#include <string>
#include <assert.h>
#include <iostream>

#include "FloatSequence.hpp"
#include "SequenceAnalyser.hpp"

static SequenceAnalyserCPU analyserCpu;

void printResult(const std::string &title, Interval &interval)
{
    printf("%s\n", title.c_str());
    printf("Longest continous increasing sequence:\n");
    printf("	Start index: %d\n", interval.startIndex);
    printf("	End index: %d\n", interval.endIndex);
    printf("	Interval length: %d\n", interval.endIndex - interval.startIndex);
}

void testLongestIncInterval(std::vector<float> data, Interval expected)
{
    FloatSequence seq(data);
    std::cout << "Testing predefined sequence:" << std::endl;
    seq.print(20);

    Interval resultCpu = analyserCpu.calcLongestIncInterval(seq);
    printResult("CPU", resultCpu);
    assert(SequenceAnalyser::intervalEquals(resultCpu, expected));
}

int main(int argc, char ** argv)
{
    // Check multiple results
    testLongestIncInterval(
        {1, 2, 1, 2, 1, 2, 1, 2},
        {0,2});
    testLongestIncInterval(
        {0.58635, 0.24096, 0.780683, 0.0204995, 0.669729, 0.114543, 0.183372, 0.195734},
        {5,8});
    testLongestIncInterval(
        {0.629432, 0.086141, 0.63704, 0.453759, 0.0106824, 0.99078, 0.86038, 0.631292},
        {1,3});

    // Check passing multiple batch boundaries
    testLongestIncInterval(
        {1, 1, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 11, 11},
        {2,13});

    // Check full interval
    testLongestIncInterval(
        {-1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17},
        {0,19});
    // Check empty interval
    testLongestIncInterval(
        {15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, -1, -2},
        {0,1});

    // Check interval at the beginning
    testLongestIncInterval(
        {1, 2, 3, 0, 0, 0, 0, 0},
        {0,3});

    // Check interval at the end
    testLongestIncInterval(
        {0, 0, 0, 0, 0, 1, 2, 3},
        {4,8});

	return 0;
}