#!/bin/bash

mkdir -p build &&
cd build &&
cmake .. -G Ninja -DCMAKE_POLICY_VERSION_MINIMUM=3.5 &&
ninja &&
# ninja > build.log 2>&1 &&

# should be unnecessary but were doing this for now
# shaders are in src and should be somewhere else, but if we run from the build folder it can't find them
# mv build/dove src &&
# rm -rf src/modules
# mv build/modules src &&

./dove
