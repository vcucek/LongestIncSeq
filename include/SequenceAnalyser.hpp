#ifndef __sequenceanalyser_h__
#define __sequenceanalyser_h__

#include "FloatSequence.hpp"

typedef struct
{
    int startIndex = -1;
    int endIndex = -1;
} Interval;

class SequenceAnalyser
{
public:
    virtual Interval calcLongestIncInterval(FloatSequence &seq) = 0;

    static bool intervalEquals(Interval &a, Interval &b)
    {
        if (a.startIndex == b.startIndex && a.endIndex == b.endIndex)
        {
            return true;
        }
        return false;
    }
};

class SequenceAnalyserCUDA : public SequenceAnalyser
{
public:
    int batchSize = 10000;
    virtual Interval calcLongestIncInterval(FloatSequence &seq);
};

class SequenceAnalyserCPU : public SequenceAnalyser
{
public:
    virtual Interval calcLongestIncInterval(FloatSequence &seq);
};

#endif
