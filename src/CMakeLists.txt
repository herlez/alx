option(ALX_BENCHMARK_PARALLEL "Run benchmarks with parallel data structures" OFF)
option(ALX_BENCHMARK_INTERNAL "Also benchmark internal data structures" OFF)
option(ALX_BENCHMARK_SPACE "Also benchmark memory and memory peak" OFF)

if(ALX_BENCHMARK_SPACE AND ALX_BENCHMARK_PARALLEL)
  error("Can't measure space for parallel algorithms. Disable ALX_BENCHMARK_SPACE or ALX_BENCHMARK_PARALLEL")
endif()

add_subdirectory(lce)
add_subdirectory(pred)
add_subdirectory(suffix_sort)