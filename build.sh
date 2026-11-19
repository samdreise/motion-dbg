#!/usr/bin/env bash

set -eou pipefail
cd "$(dirname "$0")" 

cmake -S . -B build/debug -DCMAKE_BUILD_TYPE=Debug
cmake --build build/debug -j "$(nproc)"
ctest --test-dir build/debug --output-on-failure
./build/debug/motiondbg


