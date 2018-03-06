cmake_minimum_required(VERSION 3.5)

if(${ARCH} MATCHES "Raspberry")
    set(CMAKE_C_COMPILER /usr/bin/arm-linux-gnueabi-gcc)
    set(CMAKE_CXX_COMPILER /usr/bin/arm-linux-gnueabi-g++)
elseif(${ARCH} MATCHES "Android")
    # Compile only with clang - android-arm not recognize iostream!!!
    set(CMAKE_ANDROID_NDK $ENV{HOME}/Android/Sdk/ndk-bundle)
    set(CMAKE_C_COMPILER /home/liork/Android/Sdk/ndk-bundle/toolchains/llvm/prebuilt/linux-x86_64/bin/clang)
    set(CMAKE_CXX_COMPILER /home/liork/Android/Sdk/ndk-bundle/toolchains/llvm/prebuilt/linux-x86_64/bin/clang++)
endif()

project(ScapiLite)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++11 -Wno-narrowing")

set(SOURCES src/primitives/aes.c src/primitives/common.cpp src/primitives/Prg.cpp)

add_library(ScapiLite SHARED ${SOURCES})

if(${ARCH} MATCHES "Ubuntu")
    set_target_properties(ScapiLite PROPERTIES PREFIX "libUbuntu")
elseif(${ARCH} MATCHES "Raspberry")
    set_target_properties(ScapiLite PROPERTIES PREFIX "libRaspberry")
elseif(${ARCH} MATCHES "Android")
    set_target_properties(ScapiLite PROPERTIES PREFIX "libAndroid")
endif()
