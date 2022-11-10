#!/bin/bash -e

echo "=============================================="
echo "======== Verifying application build ========="
echo "=============================================="

echo "Building target"
./build_app.sh

if [[ $? -eq 0 ]];
then
   echo "Application build OK, building Unit Tests"
   ./run_ut.sh
   if [[ $? -eq 0 ]];
   then
      echo "=============================================="
      echo "========== Project build correctly ==========="
      echo "=============================================="
   else
      echo "======================================"
      echo "====== Unit tests build error! ======"
      echo "======================================"
   fi
else
   echo "======================================"
   echo "====== Application build error! ======"
   echo "======================================"
fi
