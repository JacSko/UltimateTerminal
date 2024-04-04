#!/bin/bash -xe

if [ ! ${UT_BUILD_FOLDER+x} ]
then
   UT_BUILD_FOLDER=build_ut
fi
EXECUTOR_NUMBER=8
CMAKE_PARAMETERS="-DUNIT_TESTS=On -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=On"
CTEST_PARAMETERS="--output-on-failure --schedule-random --no-compress-output"

mkdir -p ${UT_BUILD_FOLDER}
pushd ${UT_BUILD_FOLDER}

cmake .. ${CMAKE_PARAMETERS}

make -j${EXECUTOR_NUMBER}
ctest -j${EXECUTOR_NUMBER} ${CTEST_PARAMETERS}

popd
