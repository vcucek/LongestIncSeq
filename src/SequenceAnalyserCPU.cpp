
#include "SequenceAnalyser.hpp"

Interval
SequenceAnalyserCPU::calcLongestIncInterval(FloatSequence &seq)
{
    float *buffer = seq.getBuffer();
    long bufferSize = seq.getBufferSize();

    Interval candidate = {-1, -1};
    bool hasCandidate = false;

    Interval longest = {0, 1};

    float value = buffer[0];
    float valueLast = value;

    for (int i = 0; i < bufferSize / sizeof(float); i++)
    {
        value = buffer[i];
        if (value > valueLast && !hasCandidate)
        {
            candidate.startIndex = i - 1;
            hasCandidate = true;
        }
        if (value <= valueLast && hasCandidate)
        {
            candidate.endIndex = i;
            hasCandidate = false;

            int longestLength = longest.endIndex - longest.startIndex;
            int candidateLength = candidate.endIndex - candidate.startIndex;
            if (candidateLength > longestLength)
            {
                longest = candidate;
            }
        }
        valueLast = value;
    }

    if (hasCandidate)
    {
        candidate.endIndex = bufferSize / sizeof(float);
        int longestLength = longest.endIndex - longest.startIndex;
        int candidateLength = candidate.endIndex - candidate.startIndex;
        if (candidateLength > longestLength)
        {
            longest = candidate;
        }
    }
    return longest;
}