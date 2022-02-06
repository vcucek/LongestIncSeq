
#include "SequenceAnalyser.hpp"
#include <iostream>

static int THREADS_PER_BLOCK = 256;

typedef struct
{
    Interval incr[3];
} BatchResult;

static __device__ int getGlobalComputeIndex()
{
    return blockIdx.x * blockDim.x + threadIdx.x;
}

static __global__ void findIntervalsKernel(
    const float *data,
    long dataSize,
    long batchSize,
    BatchResult *results)
{

    int computeIndex = getGlobalComputeIndex();

    // Clear result struct
    BatchResult *res = &results[computeIndex];
    res->incr[0].startIndex = -1;
    res->incr[0].endIndex = -1;
    res->incr[1].startIndex = -1;
    res->incr[1].endIndex = -1;
    res->incr[2].startIndex = -1;
    res->incr[2].endIndex = -1;

    int dataBatchStartIndex = computeIndex * batchSize;
    if (dataBatchStartIndex > dataSize - 1)
    {
        return;
    }
    int dataBatchEndIndex = dataBatchStartIndex + batchSize;
    if (dataBatchEndIndex > dataSize)
    {
        dataBatchEndIndex = dataSize;
    }

    // Clear result struct
    float last_value = data[dataBatchStartIndex];
    float value;

    // Interval potentially crossing batch start boundary
    res->incr[0].startIndex = dataBatchStartIndex;
    res->incr[2].startIndex = dataBatchEndIndex - 1;
    res->incr[2].endIndex = dataBatchEndIndex;
    Interval current = res->incr[0];
    bool has_current = true;

    for (int i = dataBatchStartIndex + 1; i < dataBatchEndIndex; i++)
    {
        value = data[i];
        if (value > last_value && !has_current)
        {
            current.startIndex = i - 1;
            current.endIndex = -1;
            has_current = true;
        }
        if (value <= last_value && has_current)
        {
            current.endIndex = i;
            // Reevaluate longest interval in this batch
            int longestLength = res->incr[1].endIndex - res->incr[1].startIndex;
            int currentLength = current.endIndex - current.startIndex;
            if (currentLength > longestLength)
            {
                res->incr[1] = current;
            }
            if (current.startIndex == dataBatchStartIndex)
            {
                res->incr[0] = current;
            }
            has_current = false;
        }
        last_value = value;
    }

    // Interval potentially crossing batch end boundary
    if (has_current)
    {
        current.endIndex = dataBatchEndIndex;

        res->incr[2].startIndex = current.startIndex;
        res->incr[2].endIndex = current.endIndex;

        // Reevaluate longest interval in this batch
        int longestLength = res->incr[1].endIndex - res->incr[1].startIndex;
        int currentLength = current.endIndex - current.startIndex;
        if (currentLength > longestLength)
        {
            res->incr[1] = current;
        }
        if (current.startIndex == dataBatchStartIndex)
        {
            res->incr[0] = current;
        }
    }
}

Interval
SequenceAnalyserCUDA::calcLongestIncInterval(FloatSequence &seq)
{

    // Check CUDA
    cudaError_t error;
    error = cudaGetLastError();
    if (error != cudaSuccess)
    {
        std::cerr 
            << "ERROR: Initializing CUDA: " << cudaGetErrorString(error) 
            << ", Exiting...(code 1)" << std::endl;
        exit(1);
    }

    // Allocate device input data
    float *devFloatArray = NULL;
    long devFloatArraySize = seq.getBufferSize() / sizeof(float);
    cudaMalloc(&devFloatArray, seq.getBufferSize());
    cudaDeviceSynchronize();
    error = cudaGetLastError();
    if (error != cudaSuccess)
    {
        std::cerr 
            << "ERROR: Allocating device input data: " << cudaGetErrorString(error) 
            << ", Exiting...(code 1)" << std::endl;
        exit(1);
    }
    // Copy input data from host to device
    cudaMemcpy(devFloatArray, seq.getBuffer(), seq.getBufferSize(), cudaMemcpyHostToDevice);
    cudaDeviceSynchronize();
    error = cudaGetLastError();
    if (error != cudaSuccess)
    {
        std::cerr 
            << "ERROR: Copying input data from host to device: " << cudaGetErrorString(error) 
            << ", Exiting...(code 1)" << std::endl;
        exit(1);
    }

    int tasks = (int)(seq.getSize() / batchSize) + 1;
    int blocks = (int)(tasks / THREADS_PER_BLOCK) + 1;

    // Allocate device output data
    int batchResultArraySize = tasks;
    BatchResult *batchResultArray = NULL;
    batchResultArray = (BatchResult *)calloc(batchResultArraySize, sizeof(BatchResult));
    BatchResult *devBatchResultArray = NULL;
    cudaMalloc(&devBatchResultArray, batchResultArraySize * sizeof(BatchResult));
    cudaDeviceSynchronize();
    error = cudaGetLastError();
    if (error != cudaSuccess)
    {
        std::cerr 
            << "ERROR: Allocating device output data memory block: " << cudaGetErrorString(error) 
            << ", Exiting...(code 1)" << std::endl;
        exit(1);
    }

    // Run CUDA kernel
    findIntervalsKernel<<<blocks, THREADS_PER_BLOCK>>>(
        devFloatArray, devFloatArraySize,
        batchSize,
        devBatchResultArray);

    error = cudaGetLastError();
    if (error != cudaSuccess)
    {
        std::cerr 
            << "ERROR: Running findIntervalsKernel: " << cudaGetErrorString(error) 
            << ", Exiting...(code 1)" << std::endl;
        exit(1);
    }

    // Copy output data from device to host
    cudaMemcpy(batchResultArray, devBatchResultArray,
               batchResultArraySize * sizeof(BatchResult),
               cudaMemcpyDeviceToHost);
    cudaDeviceSynchronize();
    error = cudaGetLastError();
    if (error != cudaSuccess)
    {
        std::cerr 
            << "ERROR: Copying findIntervalsKernel results from device to host: " << cudaGetErrorString(error) 
            << ", Exiting...(code 1)" << std::endl;
        exit(1);
    }

    // Filter longest interval from candidates
    Interval interCurrent = {0, 1};
    Interval interLongest = {0, 1};

    for (int i = 0; i < batchResultArraySize; i++)
    {
        for (int s = 0; s < 3; s++)
        {
            Interval inter = batchResultArray[i].incr[s];
            if (inter.startIndex >= 0 && inter.endIndex >= 1)
            {

                if (s == 0 && interCurrent.endIndex == inter.startIndex)
                {
                    float endValue = seq.getValue(interCurrent.endIndex - 1);
                    float startValue = seq.getValue(inter.startIndex);
                    if (startValue > endValue)
                    {
                        // Interval can be extended
                        interCurrent.endIndex = inter.endIndex;
                        if (interCurrent.endIndex - interCurrent.startIndex > interLongest.endIndex - interLongest.startIndex)
                        {
                            interLongest = interCurrent;
                        }
                    }
                }
                else if (inter.endIndex > interCurrent.endIndex)
                {
                    interCurrent = inter;
                    // Check and set if current interval is longer
                    if (interCurrent.endIndex - interCurrent.startIndex > interLongest.endIndex - interLongest.startIndex)
                    {
                        interLongest = interCurrent;
                    }
                }
            }
        }
    }

    // Dispose allocated device and host memory
    cudaFree(devBatchResultArray);
    cudaFree(devFloatArray);
    free(batchResultArray);

    return interLongest;
}