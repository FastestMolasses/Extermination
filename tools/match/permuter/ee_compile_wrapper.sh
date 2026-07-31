#!/bin/bash
# permuter compile wrapper for ee-gcc 2.9 units: invoked as `compile.sh <in.c> -o <out.o>`
cd /work
tools/eegcc/ee-compile.sh "$1" "$3" -O2 2>/dev/null
