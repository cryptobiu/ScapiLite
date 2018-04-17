cmake_minimum_required(VERSION 3.4.1)

set(CMAKE_C_COMPILER gcc)
set(CMAKE_CXX_COMPILER g++)

PROJECT(protocol)

set(distribution_DIR ${CMAKE_SOURCE_DIR}/../lib)

add_library(lib_ntl STATIC IMPORTED)
set_target_properties(lib_ntl PROPERTIES IMPORTED_LOCATION
    ${distribution_DIR}/libntl.a)

add_library(lib_gmp STATIC IMPORTED)
set_target_properties(lib_gmp PROPERTIES IMPORTED_LOCATION
    ${distribution_DIR}/libgmp.a)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g -O1")

add_executable(protocol Prg.cpp aes.cpp Common.cpp ArithmeticCircuit.cpp
CommBF.cpp MPCCommunicationBF.cpp Protocol.cpp ConfigFile.cpp Mersenne.h Matrix.h
TemplateField.cpp GF2_8LookupTable.cpp ProtocolParty.h main.cpp)

target_include_directories(protocol PRIVATE
                           ${distribution_DIR}/../include)

target_link_libraries(protocol
                      gmp
                      ntl
		      pthread)
