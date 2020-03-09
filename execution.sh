#!/usr/bin/env bash
echo "Profiling atomics"
cmake ./
make
./openCLDispatcher $1