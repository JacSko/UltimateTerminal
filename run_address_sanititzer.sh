#!/bin/bash -xe

BUILD_FOLDER=build_ut_address_sanitizer
HALT_ON_ERROR=1

export ASAN_OPTIONS=print_stacktrace=true:halt_on_error=${HALT_ON_ERROR}:detect_stack_use_after_return=1
export UBSAN_OPTIONS=print_stacktrace=true:halt_on_error=${HALT_ON_ERROR}
export CFLAGS='-fsanitize=address,undefined'
export CXXFLAGS='-fsanitize=address,undefined'
export LDFLAGS='-pthread'

. ./run_ut.sh