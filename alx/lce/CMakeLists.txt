add_library(alx_lce INTERFACE)

add_library(alx_lce_naive INTERFACE)
target_include_directories(alx_lce_naive INTERFACE ${ALX_INCLUDE_DIR})
target_link_libraries(alx_lce INTERFACE alx_lce_naive)

add_library(alx_lce_naive_block INTERFACE)
target_include_directories(alx_lce_naive_block INTERFACE ${ALX_INCLUDE_DIR})
target_link_libraries(alx_lce INTERFACE alx_lce_naive_block)

add_library(alx_lce_std INTERFACE)
target_include_directories(alx_lce_std INTERFACE ${ALX_INCLUDE_DIR})
target_link_libraries(alx_lce INTERFACE alx_lce_std)

add_library(alx_lce_fp INTERFACE)
target_include_directories(alx_lce_fp INTERFACE ${ALX_INCLUDE_DIR})
target_link_libraries(alx_lce_fp INTERFACE alx_rolling_hash)
target_link_libraries(alx_lce INTERFACE alx_lce_fp)



