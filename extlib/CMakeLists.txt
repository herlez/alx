set(ALX_EXTLIB_DIR ${CMAKE_CURRENT_SOURCE_DIR})
message(STATUS "alx extlib path: ${ALX_EXTLIB_DIR}")

add_library(rk-lce INTERFACE)
target_include_directories(rk-lce INTERFACE rk-lce/internal) 

add_library(tlx_clp tlx/tlx/cmdline_parser.cpp tlx/tlx/string/parse_si_iec_units.cpp)
target_include_directories(tlx_clp PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/tlx)

add_subdirectory(fmt)

add_library(malloc_count malloc_count/malloc_count.c)
target_include_directories(malloc_count PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(malloc_count ${CMAKE_DL_LIBS})

add_library(libsais libsais/src/libsais.c)
target_include_directories(libsais PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/libsais ${CMAKE_CURRENT_SOURCE_DIR}/libsais/src)

add_library(libsais64 libsais/src/libsais64.c)
target_include_directories(libsais64 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/libsais ${CMAKE_CURRENT_SOURCE_DIR}/libsais/src)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/modular_arithmetic)
#add_library(hurchalla_modular_arithmetic hurchalla_modular_arithmetic)

add_library(parallel-hashmap INTERFACE)
target_include_directories(parallel-hashmap INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/parallel-hashmap/)

add_library(pgm INTERFACE)
target_link_directories(pgm INTERFACE  OpenMP::OpenMP_CXX)
target_include_directories(pgm INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/PGM-index/include/pgm)