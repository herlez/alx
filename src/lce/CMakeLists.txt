add_executable(benchmark_lce benchmark.cpp)
target_link_libraries(benchmark_lce PRIVATE alx_lce tlx_clp fmt::fmt-header-only)

find_package(OpenMP)

if(ALX_BENCHMARK_PARALLEL)
  target_compile_definitions(benchmark_lce PRIVATE -DALX_BENCHMARK_PARALLEL)
  target_link_libraries(benchmark_lce PRIVATE OpenMP::OpenMP_CXX)
else()
  target_link_libraries(benchmark_lce PRIVATE malloc_count)
endif()

add_executable(genqueries genqueries.cpp)
target_link_libraries(genqueries PRIVATE tlx_clp)
