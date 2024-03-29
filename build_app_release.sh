#!/bin/bash -e

CUSTOM_EXECUTOR_NUMBER=8
CUSTOM_CMAKE_PARAMETERS="-DCMAKE_BUILD_TYPE=Release"

rm -rf build_release
mkdir -p build_release
pushd build_release

cmake .. ${CUSTOM_CMAKE_PARAMETERS}

if [[ $? -eq 0 ]];
then
   echo "=============================================="
   echo "=== Project configured correctly, building ==="
   echo "=============================================="
   make -j${CUSTOM_EXECUTOR_NUMBER}
   if [[ $? -eq 0 ]];
   then
      echo "=========================="
      echo "=== Build successfully ==="
      echo "=========================="
   else
      echo "=========================="
      echo "====== Build error! ======"
      echo "=========================="
   fi
else
      echo "=========================="
      echo "====== Cmake error! ======"
      echo "=========================="
fi

popd
