#!/bin/bash

if test -d ./modules/; then
	exit 0
fi

# setup cmocka

mkdir modules
cd modules
git clone https://gitlab.com/cmocka/cmocka.git
mv cmocka cmocka-build
mkdir cmocka-build/build

# install cmake
if  test ! -f "/usr/bin/cmake"; then
	sudo apt-get install cmake
fi
# build cmocka
cd cmocka-build/build
git checkout "stable-1.1"
cmake -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_BUILD_TYPE=Debug ..
make

# copy library files
cd ../..
mkdir -p ./cmocka/include
cp ./cmocka-build/include/cmocka.h ./cmocka/include/
cp ./cmocka-build/include/cmocka_pbc.h ./cmocka/include/
mkdir ./cmocka/lib
cp ./cmocka-build/build/src/libcmocka* ./cmocka/lib/

# cleanup
rm -rf cmocka-build
