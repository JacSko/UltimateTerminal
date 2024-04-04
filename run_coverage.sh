#!/bin/bash -xe

CMAKE_PARAMETERS="-DUNIT_TESTS=On -DUT_NO_DEBUG_OUTPUT=On -DCOVERAGE=On"
UT_BUILD_FOLDER=build_ut_coverage
EXECUTOR_NUMBER=8

mkdir -p ${UT_BUILD_FOLDER}
pushd ${UT_BUILD_FOLDER}

cmake .. ${CMAKE_PARAMETERS}

make -j${EXECUTOR_NUMBER}
ctest -j${EXECUTOR_NUMBER}

if [[ ! -d "html/" ]]
then
    echo "creating html dir"
    mkdir html
fi

gcovr -f ../sw/ -s --root=. -e /.*/test/.* -e /.*/tests/.* --html --html-details --output=html/coverage.html --exclude-unreachable-branches

popd
