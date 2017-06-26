#!/bin/bash


mkdir -p $TUCUXI_ROOT/bin

if test "$1" = "clean"; then
MAKECMD=clean
fi

for MODULE in tucucommon tucucore tucucli
do
   cd $TUCUXI_ROOT/src/$MODULE
   mkdir -p objs
   make TARGET=LINUX $MAKECMD > objs/build.log 2>&1
done


for MODULE in tucucommon
do
   cd $TUCUXI_ROOT/test/$MODULE
   mkdir -p objs
   make TARGET=LINUX $MAKECMD > objs/build.log 2>&1
done

doxygen $TUCUXI_ROOT/src/doxyfile

