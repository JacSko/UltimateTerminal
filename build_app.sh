#!/bin/bash -e

if [ ! ${CUSTOM_EXECUTOR_NUMBER+x} ]
then
   CUSTOM_EXECUTOR_NUMBER=8
fi

if [ ! ${CUSTOM_CMAKE_PARAMETERS+x} ]
then
   CUSTOM_CMAKE_PARAMETERS="-DCMAKE_BUILD_TYPE=Debug"
fi

mkdir -p build
pushd build

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
