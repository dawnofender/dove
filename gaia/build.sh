#!/bin/bash

mkdir -p build &&
cd build &&
cmake .. -G Ninja -DCMAKE_POLICY_VERSION_MINIMUM=3.5 &&
ninja &&
# ninja > build.log 2>&1 &&
cd .. &&

# should be unnecessary but were doing this for now
mv build/reality src &&
cd src &&

./reality
