#!/bin/bash -xe

if [ ! ${CUSTOM_EXECUTOR_NUMBER+x} ]
then
   CUSTOM_EXECUTOR_NUMBER=8
fi

if [ ! ${CUSTOM_CMAKE_PARAMETERS+x} ]
then
   CUSTOM_CMAKE_PARAMETERS="-DUNIT_TESTS=On -DCMAKE_BUILD_TYPE=Debug"
fi

mkdir -p build_ut
pushd build_ut

cmake .. ${CUSTOM_CMAKE_PARAMETERS}

make -j${CUSTOM_EXECUTOR_NUMBER}
ctest -j${CUSTOM_EXECUTOR_NUMBER}

popd
