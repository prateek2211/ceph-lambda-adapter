# Minimal CMakeLists.txt for the AWS SDK for C++.
cmake_minimum_required(VERSION 3.5)

set(BOOST_ROOT /opt/ceph)
set(BUILD_SHARED_LIBS OFF)
set(CMAKE_PREFIX_PATH ~/install)
project(ceph-lambda-adapter)

# Locate the AWS SDK for C++ package.
# "s3" and "cloudtrail" are just example values.
find_package(AWSSDK REQUIRED COMPONENTS s3 sns lambda)
find_package(Boost 1.72.0 REQUIRED COMPONENTS)
include_directories(${Boost_INCLUDE_DIRS})

# The executable name and its sourcefiles.
add_executable(${PROJECT_NAME} main.cpp aws/sns.cpp aws/lambda.cpp)

# Build using the C++ standard version 11.
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_11)

# The libraries used by your executable.
target_link_libraries(${PROJECT_NAME} ${AWSSDK_LINK_LIBRARIES} ${Boost_LIBRARIES})

