#!/bin/bash

TUCUXI_ROOT=/srv/TucuData/jenkins/Jobs/Tucuxi/workspace
export TUCUXI_ROOT=/srv/TucuData/jenkins/Jobs/Tucuxi/workspace

if [ "$1" == "clean" ]; then
makecmd=clean
fi

for MODULE in TucuCommon TucuCore TucuCli
do
   cd $TUCUXI_ROOT/Src/$MODULE
#   make TARGET=LINUX 
   ls -la 
done



