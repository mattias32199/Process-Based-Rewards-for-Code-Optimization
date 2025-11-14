#!/bin/bash
# chmod +x run.sh
FILE_PATH=$1
FILE_NAME=$(basename "$FILE_PATH")
if [ -z "$FILE_PATH" ]; then
    echo "Please provide the path to a cpp file."
    exit 1
fi

SCRIPT_PATH="$(realpath "$0")"
SCRIPT_DIR="$(dirname "$SCRIPT_PATH")"
benchmark_src="${SCRIPT_DIR%/}/benchmark/build/src"
benchmark_include="${SCRIPT_DIR%/}/benchmark/include"
utils="${SCRIPT_DIR%/}/simdbench/utils"

run_cpp_file() {
    g++ "$1" -mavx -mavx2 -std=c++17 -L$benchmark_src -isystem $benchmark_include -lbenchmark -lpthread -I$utils -o ./a.out \
    && ./a.out \
    && rm ./a.out
}
run_cpp_file "$FILE_PATH"