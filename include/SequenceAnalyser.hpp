#ifndef __sequenceanalyser_h__
#define __sequenceanalyser_h__

#include "FloatSequence.hpp"

/**
 * Interval defining subsequence
 * with strat (inclusive) and end (exclusive) indexes.
 */
typedef struct
{
    int startIndex = -1;
    int endIndex = -1;
} Interval;

/**
 * Common interface class for processing.
 */
class SequenceAnalyser
{
public:

    /**
     * Caclulate longest continuous interval where values in the provided sequence strictly increase.
     * The returned interval contains start (inclusive) and end (exclusive) indexes.
     * Consequently {0,1} is an empty interval returned when providing sequence with zero size.
     *
     * @param seq is a sequnce of float values.
     * @return interval defining subsequence with strictly increasing values.
     */
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

/**
 * CUDA implementation of the SequenceAnalyser.
 */
class SequenceAnalyserCUDA : public SequenceAnalyser
{
public:
    int batchSize = 10000;
    virtual Interval calcLongestIncInterval(FloatSequence &seq);
};

/**
 * Basic CPU implementation of the SequenceAnalyser.
 */
class SequenceAnalyserCPU : public SequenceAnalyser
{
public:
    virtual Interval calcLongestIncInterval(FloatSequence &seq);
};

#endif
