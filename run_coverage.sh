export CFLAGS='-fprofile-arcs -ftest-coverage'
export CXXFLAGS='-fprofile-arcs -ftest-coverage'
export CMAKE_PARAMETERS="-DUNIT_TESTS=On -DUT_NO_DEBUG_OUTPUT=On"
BUILD_FOLDER=build_ut_coverage

. ./run_ut.sh

cd build_ut_coverage

if [[ ! -d "html/" ]]
then
    echo "creating html dir"
    mkdir html
fi

gcovr -f ../sw/ -s --root=. -e /.*/test/.* -e /.*/tests/.* --html --html-details --output=html/coverage.html --exclude-unreachable-branches
