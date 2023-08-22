#!/bin/bash -e

./build_app_release.sh

if [[ $? -eq 0 ]];
then
   echo " Build correctly, preparing deployment package"
   mkdir -p deploy
   pushd deploy
   cp ../build_release/bin/UltimateTerminal .
   cp ../build_release/bin/.system_config.json .
   cp ../icon.png .
   echo "=============================================="
   echo "========= Project deployed correctly ========="
   echo "=============================================="
   popd
else
   echo "=========================="
   echo "====== Build error! ======"
   echo "=========================="
fi

