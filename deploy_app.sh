#!/bin/bash -e

if [ ! ${CUSTOM_EXECUTOR_NUMBER+x} ]
then
   CUSTOM_EXECUTOR_NUMBER=8
fi

if [ ! ${CUSTOM_CMAKE_PARAMETERS+x} ]
then
   CUSTOM_CMAKE_PARAMETERS="-DCMAKE_BUILD_TYPE=Release"
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
      echo " Build correctly, preparing deployment package"
      popd
      mkdir -p deploy
      pushd deploy
      cp ../build/bin/UltimateTerminal .
      cp /usr/lib/x86_64-linux-gnu/libQt5Widgets.so.5 .
      cp /usr/lib/x86_64-linux-gnu/libQt5Gui.so.5 .
      cp /usr/lib/x86_64-linux-gnu/libQt5Core.so.5 .
      cp ../config/system_config.json ./.system_config.json
      echo "=============================================="
      echo "========= Project deployed correctly ========="
      echo "=============================================="
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
