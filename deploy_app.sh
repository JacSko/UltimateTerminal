#!/bin/bash -e

./build_app.sh

if [[ $? -eq 0 ]];
then
   echo " Build correctly, preparing deployment package"
   mkdir -p deploy
   pushd deploy
   cp ../build/bin/UltimateTerminal .
   cp ../build/bin/.system_config.json .
   cp /usr/lib/x86_64-linux-gnu/libQt5Widgets.so.5 .
   cp /usr/lib/x86_64-linux-gnu/libQt5Gui.so.5 .
   cp /usr/lib/x86_64-linux-gnu/libQt5Core.so.5 .
   echo "=============================================="
   echo "========= Project deployed correctly ========="
   echo "=============================================="
   popd
else
   echo "=========================="
   echo "====== Build error! ======"
   echo "=========================="
fi

