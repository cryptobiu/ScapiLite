cmake_minimum_required(VERSION 3.4.1)

set(distribution_DIR ${CMAKE_SOURCE_DIR}/../../../../distribution)

add_library(lib_ntl STATIC IMPORTED)
set_target_properties(lib_ntl PROPERTIES IMPORTED_LOCATION
    ${distribution_DIR}/NTL/lib/${ANDROID_ABI}/libntl.a)

add_library(lib_boost_thread STATIC IMPORTED)
set_target_properties(lib_boost_thread PROPERTIES IMPORTED_LOCATION
    ${distribution_DIR}/Boost/lib/${ANDROID_ABI}/libboost_thread.a)

add_library(lib_boost_system STATIC IMPORTED)
set_target_properties(lib_boost_system PROPERTIES IMPORTED_LOCATION
    ${distribution_DIR}/Boost/lib/${ANDROID_ABI}/libboost_system.a)

add_library(lib_gmp SHARED IMPORTED)
set_target_properties(lib_gmp PROPERTIES IMPORTED_LOCATION
    ${distribution_DIR}/GMP/lib/${ANDROID_ABI}/libgmp.so)

find_library(android-lib
          android)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g")

add_library(primitives SHARED native-lib.cpp Prg.cpp aes.cpp Common.cpp ArithmeticCircuit.cpp
Comm.cpp MPCCommunication.cpp Protocol.cpp ConfigFile.cpp Measurement.cpp Mersenne.h Matrix.h
TemplateField.cpp GF2_8LookupTable.cpp ProtocolParty.h CommBF.cpp MPCCommunicationBF.cpp)

target_include_directories(primitives PRIVATE
                           ${distribution_DIR}/GMP/include
                           ${distribution_DIR}/NTL/include
                           ${distribution_DIR}/Boost/include)

target_link_libraries(primitives
                      lib_gmp
                      lib_boost_thread
                      lib_boost_system
                      lib_ntl
                      log
                      ${android-lib}
                      ${android})