add_library(alx_util INTERFACE)
target_include_directories(alx_util INTERFACE ${ALX_INCLUDE_DIR})
target_link_libraries(alx_util INTERFACE fmt::fmt-header-only)