## Introduction
Simple demo project for finding longest increasing interval in a sequence of float values.<br>
It provides two implementations optimized for CPU and CUDA compute units.

The CUDA implementation utilizes the reduction algorithm.
First, the input sequence is segmented into smaller batches for optimized concurrent execution on CUDA compute units.
Then the algorithm runs in parallel on each batch, producing three interval candidates.

Let us consider the following example sequence split into three batches.

|1,0,1,0,1|0,1,0,**1,2|3,4**,0,1,2|

The CUDA kernel produces 9 interval candidates.
The first and last candidate in each batch represents two bordering intervals, and the third one represent the longes increasing interval in a batch.

| [0,1),[1,2),[3,5) | [5,7),[5,7),**[7,10) | [10,12)**,[10,12),[12,15) |

In the final process, the algorithm sequentially iterates through all interval candidates, potentially merges bordering ones, and determines the longest increasing interval.

The application has been developed and tested on ArchLinux using CUDA 11.6.0-1 and GCC compiler 11.1.0-3.

## Build
```Bash
cmake -Bbuild
cd build
make all test
```

## Usage
Run algorithm using single CPU core:
```Bash
cd build
./LongestIncSeq cpu <fileUrl>
```
Run algorithm using CUDA compute units:
```Bash
cd build
./LongestIncSeq cuda <fileUrl>
```

The build process also creates some utility executables primarly used for testing purpose, but they can also be utilized for a quick comaprisson between CPU and GPU performance.

```Bash
cd build/tests
./SequenceAnalyserTest random <random_sequence_size> <batch_size>
```

Where a random sequence size is a total number of random float values generated as the algorithm input sequence.
The Batch size determines the number of float values processed by one CUDA thread. The recommanded value is ~5000.
