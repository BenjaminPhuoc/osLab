#!/bin/bash

echo "Cleaning Previous Build ..."
make clean

echo "Building ..."
make

echo "Running Program..."
./main

echo "Program finished running!"
read -p "Result is stored in output.txt! Show result? (y/N): " answer
if [[ "$answer" =~ ^[Yy]$ ]]; then
    cat output.txt
fi
