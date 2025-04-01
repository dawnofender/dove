#!/bin/bash

ninja -C build &&
mv build/reality src &&
cd src &&
./reality 
