#!/bin/bash -xe

BUILD_FOLDER=build_ut
EXECUTOR_NUMBER=8
CMAKE_PARAMETERS="-DUNIT_TESTS=On -DCMAKE_BUILD_TYPE=Debug"
CTEST_PARAMETERS="--output-on-failure --schedule-random --no-compress-output"

mkdir -p ${BUILD_FOLDER}
pushd ${BUILD_FOLDER}

cmake .. ${CMAKE_PARAMETERS}

make -j${EXECUTOR_NUMBER}
ctest -j${EXECUTOR_NUMBER} ${CTEST_PARAMETERS}

popd
