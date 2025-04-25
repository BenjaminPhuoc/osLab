#!/bin/bash

echo "Cleaning Previous Build ..."
make clean

echo "Building ..."
make p1

echo "Running Program..."
./mypool

echo "Program finished running!"