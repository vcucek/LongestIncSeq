#include "FloatSequence.hpp"

#include <string.h>
#include <random>
#include <cstdlib>
#include <iostream>
#include <fstream>

FloatSequence::FloatSequence()
{
    buffer = NULL;
    buffer_size = 0;
}

FloatSequence::FloatSequence(std::vector<float> &data)
{
    buffer_size = data.size() * sizeof(float);
    buffer = (float *)calloc(buffer_size, 1);
    memcpy(buffer, &data[0], buffer_size);
}

FloatSequence::~FloatSequence()
{
    if (buffer)
    {
        delete buffer;
    }
    buffer = NULL;
    buffer_size = 0;
}

float *
FloatSequence::getBuffer()
{
    return buffer;
}

long FloatSequence::getBufferSize()
{
    return buffer_size;
}

long FloatSequence::getSize()
{
    return buffer_size / 4;
}

float FloatSequence::getValue(long index)
{
    return buffer[index];
}

bool FloatSequence::read(std::string url)
{
    std::ifstream is;
    is.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try
    {
        is.open(url, std::ios::binary);
        is.seekg(0, std::ios::end);
        // Read file length in bytes
        buffer_size = is.tellg();
        if (buffer_size % 4 != 0)
        {
            std::cerr
                << "ERROR: Expecting tightly packed array of 32 bit little endian float values"
                << " URL: " << url
                << " size: " << buffer_size << std::endl;

            is.close();
            return false;
        }
        std::cout
            << "Reading file: " << url
            << ", byte size: " << buffer_size << std::endl;

        is.seekg(0, std::ios::beg);
        buffer = (float *)calloc(buffer_size, 1);
        // Read file data
        is.read((char *)buffer, buffer_size);
        is.close();
        return true;
    }
    catch (std::system_error &e)
    {
        std::cerr << "ERROR: Unable to open file: " << url << std::endl;
        return false;
    }
}

bool FloatSequence::randomize(long size)
{
    std::random_device r;
    std::default_random_engine randomEngine(r());
    std::uniform_int_distribution<int> uniform_dist(0, 1000);

    srand(time(NULL));
    buffer = (float *)calloc(size, sizeof(float));
    for (int i = 0; i < size; i++)
    {
        buffer[i] = (float)uniform_dist(randomEngine);
    }
    buffer_size = size * 4;
    return true;
}

void FloatSequence::print(int max_elements)
{
    if (buffer && buffer_size)
    {
        long elements_count = buffer_size / 4;
        std::cout << "FloatSequence: [";
        for (long i = 0; i < elements_count && i < max_elements; i++)
        {
            if (i != 0)
            {
                std::cout << ", ";
            }
            std::cout << buffer[i];
        }
        if (elements_count > max_elements)
        {
            std::cout << "...]";
        }
        else
        {
            std::cout << "]";
        }

        std::cout << ", elements count: " << elements_count << std::endl;
    }
}