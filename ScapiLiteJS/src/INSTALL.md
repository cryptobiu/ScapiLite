## ScapiLite JS

ScapiLite JS is a port of [libscapi](https://github.com/cryptobiu/libscapi) library for browsers, built with JavaScript using [emscripten](https://github.com/kripken/emscripten).


### Libraries used by ScapiLite JS

#### emscripten

- mkdir emscripten
- sudo apt-get install debootstrap
- sudo debootstrap --arch i386 stretch emscripten
- sudo chroot emscripten /bin/bash
- apt-get install python cmake g++ nodejs git lzip
- echo "none /dev/shm tmpfs rw,nosuid,nodev,noexec 0 0" >> /etc/fstab
- mount /dev/shm
- adduser emscripten
- su - emscripten
- wget https://s3.amazonaws.com/mozilla-games/emscripten/releases/emsdk-portable.tar.gz
- tar xf emsdk-portable.tar.gz
- cd emsdk-portable
- ./emsdk update
- ./emsdk install latest-32bit - **this process can take between 4-5 hours**
- ./emsdk activate latest-32bit
- source ./emsdk_env.sh
- mkdir -p ${HOME}/opt/src
- cd ${HOME}/opt/src

#### gmp 6.1.2

We need to build gmp twice. One for C++ and the other for emcc.
wget https://gmplib.org/download/gmp/gmp-6.1.2.tar.lz
tar xf gmp-6.1.2.tar.lz
cd gmp-6.1.2
CC_FOR_BUILD=/usr/bin/gcc ABI=standard emconfigure ./configure \
  --build i686-pc-linux-gnu --disable-assembly --enable-cxx
make -j 6
sudo make install
cd ..

#### ntl 10.5.0

wget http://www.shoup.net/ntl/ntl-10.5.0.tar.gz
tar xf ntl-10.5.0.tar.gz
cd ntl-10.5.0

* Option 1: without GMP
  To compile NTL without GMP use : `emconfigure ./configure DEF_PREFIX=${HOME}/opt NTL_GMP_LIP=off`
  To compile it with em++, edit the generated makefile:
  change `CXX=g++`to `CXX=em++`, `WIZARD=on` to `WIZARD=off`. In addition compile NTL `setup1, setup2 , setup3` programs with `g++` and not with `em++`
  After you edited the makefile run : `make -j 6; make install`

* Option 2: with GMP (optimal - choose this one)

    * Compile GMP with gcc:
      * `mkdir -p gmp-6.1.2-for-ntl`
      * `tar xf gmp-6.1.2.tar.lz -C gmp-6.1.2-for-ntl`
      * `cd gmp-6.1.2-for-ntl`
      * `./configure --prefix=$HOME/optnojs --disable-assembly --build=none`
      * Edit config.h:
        * `comment: #define HAVE_OBSTACK_VPRINTF 1`
        * `comment: #define HAVE_QUAD_T 1`
      * `make`
      * `make install`

  After you install gmp with gcc we will compile ntl-10.5.0 by running: `emconfigure ./configure DEF_PREFIX=${HOME}/opt NTL_GMP_LIP=on`

  After the configuration file was created, a makefile was generated. In order to compile with em++ we need to change this lines:
    
    * `CXX=em++`
    * `WIZARD=off`
    * `GMP_PREFIX=$HOME/optnojs`
    * setup 1-4: use `g++` instead of `em++` - a sample makefile is at the repo.
  After the makefile was edditted run: `make -j 6; make install`

* OPTIONAL: run the following test to check NTL library was installed correctly:
  emcc tests_ntl.cpp ~/opt/lib/libntl.a ~/opt/lib/libgmp.a -o tests_ntl.js -I${HOME}/opt/include

#################################
Preperations - NodeJS installaion
#################################
* open a new tab *
sudo chroot emscripten /bin/bash
curl -sL https://deb.nodesource.com/setup_9.x | bash -
sudo apt-get install -y nodejs
Edit '/usr/lib/node_modules/npm/node_modules/worker-farm/lib/farm.js':
  set maxConcurrentWorkers to 1
apt get install npm
npm install ws@2.3.1
* IMPORTANT: 
  1. don't run 'su - emscripten' for this stage - remain as root
  2. you must install ws version 2.3.1 or below (from version 3.0.0 till 5.1.0 [the last version according to this moment], there is a bug which cause the nodejs server to crash. It can be fixed by manipulating the generated javascript: see https://github.com/kripken/emscripten/issues/5971, but it is much easier to install earlier version as I mentioned)

################################
Running stage
################################
Open 2 new tabs and in each of them run:
sudo chroot emscripten /bin/bash
su - emscripten
cd emsdk-portable
source ./emsdk_env.sh
cd ~/opt/src
alias ll='ls -l'

Clone ScapiLite project and go into 'ScapiLiteJS/src' folder.

The 2 tabs represent participants 2 & 3 (the browser is participant 1) and you MUST run the participants in descending order (from the third one to the first) and to wait ~5 seconds between each run.
Means:

Run participant 3:
nodejs protocol.js 2 3 assets/inputs333.txt output.txt assets/1000000G_1000000MG_333In_50Out_20D_OutputOne3P.txt assets/parties.conf GF2_8LookupTable 1

* Wait ~5 seconds

Run participant 2:
nodejs protocol.js 1 3 assets/inputs333.txt output.txt assets/1000000G_1000000MG_333In_50Out_20D_OutputOne3P.txt assets/parties.conf GF2_8LookupTable 

* Wait ~5 seconds

Run participant 1:
Edit procotol.html by simply add the following line underneath the line 'var Module':
"arguments: ['0','3','assets/inputs333.txt','output.txt','assets/1000000G_1000000MG_333In_50Out_20D_OutputOne3P.txt','assets/parties.conf','GF2_8LookupTable','1'],"
and load page to the browser



In order to install libevent add int32_t arc4random(void); in the begining of evutil_rand.c