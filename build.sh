#!/bin/bash

mkdir -p build &&
cd build &&
# temporary, but lets delete modules folder every time just to be sure that it resets properly
rm -rf modules && 
cmake .. -G Ninja -DCMAKE_POLICY_VERSION_MINIMUM=3.5 &&
ninja &&
# ninja > build.log 2>&1 &&

./dream
