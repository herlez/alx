add_library(alx_rmq INTERFACE)

add_library(alx_rmq_nlgn INTERFACE)
target_include_directories(alx_rmq_nlgn INTERFACE ${ALX_INCLUDE_DIR})
target_link_libraries(alx_rmq_nlgn INTERFACE OpenMP::OpenMP_CXX)
target_link_libraries(alx_rmq INTERFACE alx_rmq_nlgn)