export CFLAGS='-fprofile-arcs -ftest-coverage'
export CXXFLAGS='-fprofile-arcs -ftest-coverage'
BUILD_FOLDER=build_ut_coverage

. ./run_ut.sh

cd build_ut_coverage

if [[ ! -d "html/" ]]
then
    echo "creating html dir"
    mkdir html
fi

gcovr -f ../sw/ --root=. -e /.*/test/.* -e /.*/tests/.* --html --html-details --output=html/coverage.html --exclude-unreachable-branches
firefox html/coverage.html