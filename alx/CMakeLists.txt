set(ALX_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
message(STATUS "alx path: ${ALX_INCLUDE_DIR}")


add_library(alx INTERFACE)

add_subdirectory(lce)
target_link_libraries(alx INTERFACE alx_lce)

add_subdirectory(rolling_hash)
target_link_libraries(alx INTERFACE alx_rolling_hash)

add_subdirectory(pred)
target_link_libraries(alx INTERFACE alx_pred)
