#!/user/bin/env bash

rm -rf build
mkdir build
cd build || exit 1
cmake ..
make
cd .. || exit 1
