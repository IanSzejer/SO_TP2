#!/bin/bash

cd Toolchain
make --no-print-directory clean
make --no-print-directory all
cd ..
make --no-print-directory clean
make --no-print-directory all