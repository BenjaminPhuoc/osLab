#!/bin/bash

echo "Cleaning Previous Build ..."
make clean

echo "Building ..."
make

echo "Running Program..."
./seqlock

echo "Program finished running!"