#!/bin/bash -xe

BUILD_FOLDER=build_ut_thread_sanitizer
HALT_ON_ERROR=1

export TSAN_OPTIONS=print_stacktrace=true:halt_on_error=${HALT_ON_ERROR}:second_deadlock_stack=1
export CFLAGS='-fsanitize=thread'
export CXXFLAGS='-fsanitize=thread'
export LDFLAGS='-pthread'

. ./run_ut.sh