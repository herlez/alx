add_library(tlx_clp tlx/tlx/cmdline_parser.cpp tlx/tlx/string/parse_si_iec_units.cpp)
target_include_directories(tlx_clp PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/tlx)

add_subdirectory(fmt)
