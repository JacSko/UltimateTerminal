#!/bin/bash -xe

BUILD_FOLDER=build_sim
EXECUTOR_NUMBER=1
CMAKE_PARAMETERS="-DSIMULATION=On -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=On"
CTEST_PARAMETERS="--output-on-failure --schedule-random --no-compress-output"

mkdir -p ${BUILD_FOLDER}
pushd ${BUILD_FOLDER}

cmake .. ${CMAKE_PARAMETERS}

make -j${EXECUTOR_NUMBER}
ctest -j${EXECUTOR_NUMBER} ${CTEST_PARAMETERS}

popd
