#!/usr/bin/env bash

TARGET_NAME=CST8703Lab2-FreshThreads
SUBFOLDER=FreshThreads
DATAFILE_PREFIX=data/desirabilityLog

mkdir -p data

rm -rf ./build
cmake -DCMAKE_INSTALL_PREFIX=${HOME}/.local -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++ -S. -B./build
cmake --build ./build --target ${TARGET_NAME} -j 4 --

./build/${SUBFOLDER}/${TARGET_NAME} -f ${DATAFILE_PREFIX}.dat |& tee data/output.txt
python3 ./scripts/plot_datalog.py --datafile ${DATAFILE_PREFIX}.dat --output ${DATAFILE_PREFIX}.png --user "${AC_USERNAME}"
