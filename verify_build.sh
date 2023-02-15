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
      echo "Building simulation and executing tests"
      ./run_sim_tests.sh
      if [[ $? -eq 0 ]];
      then
         echo "=============================================="
         echo "==========  Project verified: OK   ==========="
         echo "=============================================="
      else
         echo "=================================================="
         echo "====== Verification with simulation failed! ======"
         echo "=================================================="
      fi
   else
      echo "============================================="
      echo "====== Unit tests verification failed! ======"
      echo "============================================="
   fi
else
   echo "======================================="
   echo "====== Application build falied! ======"
   echo "======================================="
fi
