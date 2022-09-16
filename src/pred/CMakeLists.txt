add_executable(alx_benchmark_pred benchmark.cpp)
target_link_libraries(alx_benchmark_pred PRIVATE alx_pred tlx_clp fmt::fmt-header-only)

find_package(OpenMP)

#if(ALX_BENCHMARK_PARALLEL)
  target_compile_definitions(alx_benchmark_pred PRIVATE -DALX_BENCHMARK_PARALLEL)
  target_link_libraries(alx_benchmark_pred PRIVATE OpenMP::OpenMP_CXX)
#else()
  target_link_libraries(alx_benchmark_pred PRIVATE malloc_count)
#endif()
