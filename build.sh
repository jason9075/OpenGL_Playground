#!/user/bin/env bash

rm -rf build
mkdir build
cd build || exit 1
cmake ..
make || exit 1
cd .. || exit 1
