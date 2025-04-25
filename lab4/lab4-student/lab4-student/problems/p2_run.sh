#!/bin/bash

echo "Cleaning Previous Build ..."
make clean

echo "Building ..."
make p2

echo "Running Program..."
./mypool

echo "Program finished running!"