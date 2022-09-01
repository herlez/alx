add_executable(benchmark benchmark.cpp)
target_link_libraries(benchmark PRIVATE alx tlx_clp fmt::fmt-header-only)

#if(TRUE)
target_link_libraries(benchmark PRIVATE malloc_count)
#endif()

add_executable(genqueries genqueries.cpp)
target_link_libraries(genqueries PRIVATE tlx_clp)
