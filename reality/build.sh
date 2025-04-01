#!/bin/bash

mkdir -p build
cd build
cmake .. -G Ninja
ninja
cd ..

# should be unnecessary but were doing this for now
mv build/reality src
cd src

./reality
