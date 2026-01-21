#!/bin/bash

set -Eeuo pipefail

git clone https://github.com/emscripten-core/emsdk.git

cd emsdk

./emsdk install latest
./emsdk activate latest
