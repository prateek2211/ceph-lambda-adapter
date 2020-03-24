# Minimal CMakeLists.txt for the AWS SDK for C++.
cmake_minimum_required(VERSION 3.5)

set(BUILD_SHARED_LIBS OFF)

project(ceph-lambda-adapter)

# Locate the AWS SDK for C++ package.
# "s3" and "cloudtrail" are just example values.
find_package(AWSSDK REQUIRED COMPONENTS s3)

# The executable name and its sourcefiles.
add_executable(${PROJECT_NAME} main.cpp)

# Build using the C++ standard version 11.
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_11)

# The libraries used by your executable.
target_link_libraries(${PROJECT_NAME} ${AWSSDK_LINK_LIBRARIES})

