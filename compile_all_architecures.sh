#!/bin/bash

rm -rf build_*
unset CC
unset CXX

apt update


# Controlla se è stato fornito almeno un argomento
if [ $# -lt 1 ]; then
    echo "Usage: $0 <version>"
    exit 1
fi

# Ottieni la versione dall'argomento
version="$1"

# Costruisci il contenuto da scrivere nel file R5560_SDKLib/version.h
versionContent="#ifndef __R5560_VERSION_H
#define __R5560_VERSION_H

#define R5560_VERSION \"$version\"
#endif"

# Scrivi il contenuto nel file version.h
echo "$versionContent" > "R5560_SDKLib/version.h"

echo "Version updated in R5560_SDKLib/version.h to $version"


autoreconf -if
BUILD="$(echo $MACHTYPE | sed "s/$(echo $MACHTYPE | cut -d- -f2)/cross/")"

if [ "$ARCHITECTURE" = "x64" ]; then
    #compile x64 (native)
    apt install -y libzmq3-dev
    rm -rf build_x64
    mkdir -p build_x64
    cd build_x64
    ../configure --prefix=$(pwd)/output CFLAGS="-O3" CXXFLAGS="-O3"
    make -j4 && make install
    cd output
    fpm \
    -s dir -t deb \
    -p ../../debs/libr5560_${CI_COMMIT_TAG}_${LINUXRELEASE}_amd64.deb \
    --name libr5560 \
    --license agpl3 \
    --version $CI_COMMIT_TAG \
    --architecture x86_64 \
    --description "Nuclear Instruments libR5560" \
    --url "https://www.sci-compiler.com/" \
    --maintainer "Andrea Abba <abba@nuclearinstruments.eu>" \
    .    
    tar zcvf ../../tars/libr5560-$DOCKER_IMAGE-x64.tar.gz .
    cd ../..
elif [ "$ARCHITECTURE" = "i386" ]; then
    #compile x86 
    apt install -y libzmq3-dev:i386
    rm -rf build_i386
    mkdir -p build_i386
    cd build_i386
    ../configure --host=i686-linux-gnu CFLAGS="-m32 -O3" CXXFLAGS="-m32 -O3" LDFLAGS=-m32 --prefix=$(pwd)/output
    make -j4 && make install
    cd output
    fpm \
    -s dir -t deb \
    -p ../../debs/libr5560_${CI_COMMIT_TAG}_${LINUXRELEASE}_i386.deb \
    --name libr5560 \
    --license agpl3 \
    --version $CI_COMMIT_TAG \
    --architecture i386 \
    --description "Nuclear Instruments libR5560" \
    --url "https://www.sci-compiler.com/" \
    --maintainer "Andrea Abba <abba@nuclearinstruments.eu>" \
    .    
    tar zcvf ../../tars/libr5560-$DOCKER_IMAGE-i386.tar.gz .
    cd ../..
elif [ "$ARCHITECTURE" = "arm64" ]; then
    #compile arm64
    apt install -y libzmq3-dev:arm64
    rm -rf build_arm64
    mkdir -p build_arm64
    cd build_arm64
    export CC=aarch64-linux-gnu-gcc
    export CXX=aarch64-linux-gnu-g++
    ../configure --host=aarch64-linux-gnu --prefix=$(pwd)/output --build=$BUILD CFLAGS="-O3" CXXFLAGS="-O3"
    make -j4 && make install
    cd output
    fpm \
    -s dir -t deb \
    -p ../../debs/libr5560_${CI_COMMIT_TAG}_${LINUXRELEASE}_arm64.deb \
    --name libr5560 \
    --license agpl3 \
    --version $CI_COMMIT_TAG \
    --architecture arm64 \
    --description "Nuclear Instruments libR5560" \
    --url "https://www.sci-compiler.com/" \
    --maintainer "Andrea Abba <abba@nuclearinstruments.eu>" \
    .    
    tar zcvf ../../tars/libr5560-$DOCKER_IMAGE-arm64.tar.gz .
    cd ../..
elif [ "$ARCHITECTURE" = "armhf" ]; then
    #compile armhf
    apt install -y libzmq3-dev:armhf
    rm -rf build_armhf
    mkdir -p build_armhf
    cd build_armhf
    export CC=arm-linux-gnueabihf-gcc
    export CXX=arm-linux-gnueabihf-g++
    ../configure --host=arm-linux-gnueabihf --prefix=$(pwd)/output --build=$BUILD CFLAGS="-O3" CXXFLAGS="-O3"
    make -j4 && make install
    cd output
    fpm \
    -s dir -t deb \
    -p ../../debs/libr5560_${CI_COMMIT_TAG}_${LINUXRELEASE}_armhf.deb \
    --name libr5560 \
    --license agpl3 \
    --version $CI_COMMIT_TAG \
    --architecture armhf \
    --description "Nuclear Instruments libR5560" \
    --url "https://www.sci-compiler.com/" \
    --maintainer "Andrea Abba <abba@nuclearinstruments.eu>" \
    .    
    tar zcvf ../../tars/libr5560-$DOCKER_IMAGE-armhf.tar.gz .
    cd ../..
else
    echo "Invalid architecture specified."
    exit 1
fi

