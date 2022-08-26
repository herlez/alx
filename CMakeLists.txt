cmake_minimum_required(VERSION 3.14)
project(alx)

set(CMAKE_CXX_STANDARD 20)

# GoogleTest
include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG release-1.12.1
)
FetchContent_MakeAvailable(googletest)
enable_testing()

add_subdirectory(alx)
add_subdirectory(extlib)
add_subdirectory(tests)
add_subdirectory(src)
