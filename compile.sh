 #!/bin/bash 
function createDirs
{
    mkdir -p buildUbuntu
    mkdir -p buildRaspberry
    mkdir -p buildAndroid
}

function build
{
    rm -rf CMakeFiles CMackeCache.txt cmake_install.cmake
    cmake . -DARCH=${1} -Bbuild${1}
    cd build${1} && make
    cd ..
}

createDirs
build "Ubuntu"
build "Raspberry"
build "Android"

