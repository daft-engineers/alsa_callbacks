#! /bin/bash

set -e

SRC=$(find src -name "*.cc")
WRONG_EXT=$(find src \( -name "*.cpp" -o -name "*.hpp" \))

if [[ $1 == "--static-analysis" ]]; then
    clang-tidy -p build --warnings-as-errors='clang-diagnostic-*,clang-analyzer-*,clang-analyzer-*,concurrency-*,cppcoreguidelines-*,llvm-*,modernize-*,-modernize-use-trailing-return-type,performance-*' $SRC
elif [[ $1 == "--formatting" ]]; then
    clang-format -n -Werror -style=file $SRC
    if [[ -n $WRONG_EXT ]]; then 
        echo "Files found ending in .cpp: $WRONG_EXT"
        exit 1
    fi
else
    clang-tidy -p build $SRC
    clang-format -i -style=file $SRC
    if [[ -n $WRONG_EXT ]]; then
        echo "Files found ending in .cpp: $WRONG_EXT"
    fi
fi
