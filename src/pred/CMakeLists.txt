add_executable(benchmark_pred benchmark.cpp)
target_link_libraries(benchmark_pred PRIVATE alx_pred tlx_clp fmt::fmt-header-only alx_util)

find_package(OpenMP)

if(ALX_BENCHMARK_PARALLEL)
  target_compile_definitions(benchmark_pred PRIVATE -DALX_BENCHMARK_PARALLEL)
  target_link_libraries(benchmark_pred PRIVATE OpenMP::OpenMP_CXX)
else()
  target_link_libraries(benchmark_pred PRIVATE malloc_count)
endif()

add_executable(gen_sss gen_sss.cpp)
target_link_libraries(gen_sss PRIVATE tlx_clp alx_string_synchronizing_set fmt::fmt-header-only alx_util)