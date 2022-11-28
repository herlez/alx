add_library(alx_rmq INTERFACE)

add_library(alx_rmq_nlgn INTERFACE)
target_include_directories(alx_rmq_nlgn INTERFACE ${ALX_INCLUDE_DIR})
target_link_libraries(alx_rmq_nlgn INTERFACE OpenMP::OpenMP_CXX)
target_link_libraries(alx_rmq INTERFACE alx_rmq_nlgn)

add_library(alx_rmq_naive INTERFACE)
target_include_directories(alx_rmq_naive INTERFACE ${ALX_INCLUDE_DIR})
target_link_libraries(alx_rmq_naive INTERFACE OpenMP::OpenMP_CXX)
target_link_libraries(alx_rmq INTERFACE alx_rmq_naive)

add_library(alx_rmq_n INTERFACE)
target_include_directories(alx_rmq_n INTERFACE ${ALX_INCLUDE_DIR})
target_link_libraries(alx_rmq_n INTERFACE OpenMP::OpenMP_CXX)
target_link_libraries(alx_rmq INTERFACE alx_rmq_n)