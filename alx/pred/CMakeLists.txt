add_library(alx_pred INTERFACE)

add_library(alx_pred_binsearch_std INTERFACE)
target_include_directories(alx_pred_binsearch_std INTERFACE ${ALX_INCLUDE_DIR})
target_link_libraries(alx_pred INTERFACE alx_pred_binsearch_std)

add_library(alx_pred_index INTERFACE)
target_include_directories(alx_pred_index INTERFACE ${ALX_INCLUDE_DIR})
target_link_libraries(alx_pred_index INTERFACE OpenMP::OpenMP_CXX)
target_link_libraries(alx_pred INTERFACE alx_pred_index)

#find_package(OpenMP)
#add_library(alx_lce_fp INTERFACE)
#target_include_directories(alx_lce_fp INTERFACE ${ALX_INCLUDE_DIR})
#target_link_libraries(alx_lce_fp INTERFACE alx_rolling_hash OpenMP::OpenMP_CXX)
#target_link_libraries(alx_lce INTERFACE alx_lce_fp)

