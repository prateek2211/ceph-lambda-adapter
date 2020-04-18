# Ceph AWS Adapter
Simple Adapter that invokes AWS lambda or publishes messages to SNS topic (based on program flag provided) on receiving ceph bucket notification(or any http payload in general).

## Prerequisites
AWS adapter uses standard CPP SDK provided by AWS, and lockfree queue available in boost library/
* CMake
* AWS SDK for C++
* Boost

## Build Instructions
* Make sure that your aws credentials are stored in ~/aws/credentials or are present in the environment variables.  
```shell script
$ mkdir "build"
$ cd build
$ export CMAKE_PREFIX_PATH=<Path to SDK build directory>
$ cmake ../
$ make
```

## Usage
Support for both lambda and SNS endpoints are added, which could be switched using appropriate flags.

Provide the mode of adapter as either lambda or sns

```shell script
$ ./ceph-aws-adapter -m <Mode of adapter> -p <Port of the server>
```

Eg
```shell script
$ ./ceph-aws-adapter -m lambda -p 8080
```
After running the binary, it's time to hit the endpoints with some message payload.

```shell script
$ curl -X POST 'http://127.0.0.1:8080' --data '{"payload":"sample data"}'
```