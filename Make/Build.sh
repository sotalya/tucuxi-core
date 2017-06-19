#!/bin/bash

TUCUXI_ROOT=/srv/TucuData/jenkins/Jobs/Tucuxi/workspace
export TUCUXI_ROOT=/srv/TucuData/jenkins/Jobs/Tucuxi/workspace

mkdir -p $TUCUXI_ROOT/bin

if test "$1" = "clean"; then
MAKECMD=clean
fi

for MODULE in TucuCommon TucuCore TucuCli
do
   cd $TUCUXI_ROOT/Src/$MODULE
   make TARGET=LINUX $MAKECMD > Build.log 2>&1
done



