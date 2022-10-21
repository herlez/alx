add_library(alx_lce INTERFACE)

add_library(alx_lce_naive INTERFACE)
target_include_directories(alx_lce_naive INTERFACE ${ALX_INCLUDE_DIR})
target_link_libraries(alx_lce INTERFACE alx_lce_naive)

add_library(alx_lce_naive_wordwise INTERFACE)
target_include_directories(alx_lce_naive_wordwise INTERFACE ${ALX_INCLUDE_DIR})
target_link_libraries(alx_lce INTERFACE alx_lce_naive_wordwise)

add_library(alx_lce_std INTERFACE)
target_include_directories(alx_lce_std INTERFACE ${ALX_INCLUDE_DIR})
target_link_libraries(alx_lce INTERFACE alx_lce_std)

add_library(alx_memcmp INTERFACE)
target_include_directories(alx_memcmp INTERFACE ${ALX_INCLUDE_DIR})
target_link_libraries(alx_lce INTERFACE alx_memcmp)

find_package(OpenMP)
add_library(alx_lce_fp INTERFACE)
target_include_directories(alx_lce_fp INTERFACE ${ALX_INCLUDE_DIR})
target_link_libraries(alx_lce_fp INTERFACE alx_rolling_hash OpenMP::OpenMP_CXX)
target_link_libraries(alx_lce INTERFACE alx_lce_fp)


add_library(alx_lce_sss INTERFACE)
target_include_directories(alx_lce_sss INTERFACE ${ALX_INCLUDE_DIR})
target_link_libraries(alx_lce_sss INTERFACE alx_rolling_hash OpenMP::OpenMP_CXX)
target_link_libraries(alx_lce INTERFACE alx_lce_sss)

#TODO: 
# Comments & Structure
# Build data structures so that they handle wider text symbols
