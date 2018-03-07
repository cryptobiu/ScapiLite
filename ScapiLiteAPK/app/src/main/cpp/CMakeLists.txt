cmake_minimum_required(VERSION 3.4.1)

set(distribution_DIR ${CMAKE_SOURCE_DIR}/../../../../distribution)

add_library(lib_gmp SHARED IMPORTED)
set_target_properties(lib_gmp PROPERTIES IMPORTED_LOCATION
    ${distribution_DIR}/GMP/lib/${ANDROID_ABI}/libgmp.so)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

add_library(primitives SHARED native-lib.cpp Prg.cpp aes.cpp common.cpp)

target_include_directories(primitives PRIVATE
                           ${distribution_DIR}/GMP/include)

target_link_libraries(primitives lib_gmp log)