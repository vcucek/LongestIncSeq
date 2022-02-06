#ifndef __floatsequence_h__
#define __floatsequence_h__

#include <string>
#include <vector>

class FloatSequence
{

private:
    float *buffer;
    long buffer_size;

public:
    FloatSequence();
    FloatSequence(std::vector<float> &data);
    ~FloatSequence();

    /**
    * Get internal float array buffer.
    * @return float array
    */
    float *getBuffer();

    /**
    * Get internal float array buffer size in bytes.
    * @return buffer size in bytes
    */
    long getBufferSize();

    /**
    * Get number of float elements.
    * @return number of float elements.
    */
    long getSize();

    /**
    * Get value at the specifed index
    * @return value
    */
    float getValue(long index);

    /**
    * Read float values from the specified file url.
    * @param url: Representing filepath on the filesystem.
    * @return true when operation succeeds
    */
    bool read(std::string url);

    /**
    * Allocates internal buffer with the specified number of randomized float values.
    * @param size: Number of float values generated for this sequence.
    * @return true when operation succeeds
    */
    bool randomize(long size);

    /**
    * Print float sequence to stdout.
    * @param count: Max number of printed sequece values.
    */
    void print(int count);
};

#endif
