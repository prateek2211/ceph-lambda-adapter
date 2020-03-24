# Ceph Lambda Adapter
Simple Adapter that invokes AWS lambda on recieving ceph bucket notification

## Prerequisites
* CMake
* AWS SDK for C++

## Build Instructions
```shell script
$ mkdir "build"
$ cd build
$ export CMAKE_PREFIX_PATH=<Path to SDK build directory>
$ cmake ../
$ make
$ ./ceph-lambda-adapter
```