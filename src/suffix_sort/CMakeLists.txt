add_executable(benchmark benchmark.cpp)
target_link_libraries(benchmark PRIVATE alx_lce tlx_clp fmt::fmt-header-only)

find_package(OpenMP)

if(ALX_BENCHMARK_PARALLEL)
  target_compile_definitions(benchmark PRIVATE -DALX_BENCHMARK_PARALLEL)
  target_link_libraries(benchmark PRIVATE OpenMP::OpenMP_CXX)
else()
  target_link_libraries(benchmark PRIVATE malloc_count)
endif()
