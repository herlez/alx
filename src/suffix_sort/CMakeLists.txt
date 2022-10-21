add_executable(benchmark_ss benchmark.cpp)
target_link_libraries(benchmark_ss PRIVATE alx_lce tlx_clp fmt::fmt-header-only)

find_package(OpenMP)

if(ALX_BENCHMARK_PARALLEL)
  target_compile_definitions(benchmark_ss PRIVATE -DALX_BENCHMARK_PARALLEL)
  target_link_libraries(benchmark_ss PRIVATE OpenMP::OpenMP_CXX)
else()
  target_link_libraries(benchmark_ss PRIVATE malloc_count)
endif()
