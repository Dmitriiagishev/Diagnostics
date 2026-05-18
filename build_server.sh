#!/bin/sh
rm -rf build
mkdir build
cd build
cmake ..
make
cd ..
echo "all done, now start server with ./start.sh"
