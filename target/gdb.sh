#!/usr/bin/env bash

set -eou pipefail
cd "$(dirname "$0")" 


gdb -q --interpreter=mi3 \
    -iex "set style emoji off" \
    -iex "set confirm off" \
    -iex "set pagination off" \
    -iex "set mi-async on" \
    --args target

