#!/bin/bash

ENGINE_DIR=src/engine

set -Eeuo pipefail

cd emsdk 
source ./emsdk_env.sh 
cd .. 

emcc $ENGINE_DIR/*.cpp -lembind -o $ENGINE_DIR/engine.mjs -s ALLOW_MEMORY_GROWTH=1 -s MODULARIZE=1 -s EXPORT_ES6=1
