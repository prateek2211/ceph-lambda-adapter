# Ceph Lambda Adapter
Simple Adapter that invokes AWS lambda on recieving ceph bucket notification

## Prerequisites
* CMake
* AWS SDK for C++

## Build Instructions
* Make sure that your aws credentials are stored in ~/aws/credentials or are present in the environment variables.  
```shell script
$ mkdir "build"
$ cd build
$ export CMAKE_PREFIX_PATH=<Path to SDK build directory>
$ cmake ../
$ make
$ ./ceph-lambda-adapter
```
