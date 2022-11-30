#!/bin/bash -xe

if [ ! ${BUILD_FOLDER+x} ]
then
   BUILD_FOLDER=build_ut
fi

EXECUTOR_NUMBER=8

if [ ! ${CMAKE_PARAMETERS+x} ]
then
   CMAKE_PARAMETERS="-DUNIT_TESTS=On -DCMAKE_BUILD_TYPE=Debug"
fi

CTEST_PARAMETERS="--output-on-failure --schedule-random --no-compress-output"

mkdir -p ${BUILD_FOLDER}
pushd ${BUILD_FOLDER}

cmake .. ${CMAKE_PARAMETERS}

make -j${EXECUTOR_NUMBER}
ctest -j${EXECUTOR_NUMBER} ${CTEST_PARAMETERS}

popd
