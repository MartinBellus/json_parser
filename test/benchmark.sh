#!/bin/bash

set -e

TESTS_DIR="test/bench"
JSON_PATH="test/big.json"

make

for test in $(ls $TESTS_DIR); do
    echo "Running $test"
    QUERY=$(<"$TESTS_DIR/$test")
    START_TIME=$(date +%s.%N)
    ./parser "$JSON_PATH" "$QUERY" > /dev/null
    END_TIME=$(date +%s.%N)

    ELAPSED_TIME=$(echo "$END_TIME - $START_TIME" | bc)
    echo "Elapsed time for $file: ${ELAPSED_TIME} seconds"
    echo
done
