#!/bin/bash


mkdir -p $TUCUXI_ROOT/bin

RESULT=0

if test "$1" = "clean"; then
MAKECMD=clean
fi

for MODULE in tiny-js-master-20170629
do
   cd $TUCUXI_ROOT/libs/$MODULE
   mkdir -p objs
   make TARGET=LINUX $MAKECMD $@ 2>&1 | tee objs/build.log
   if [ ${PIPESTATUS[0]} -eq 2 ]
   then
      RESULT=1
   fi
done

for MODULE in botan-2.1.0
do
   cd $TUCUXI_ROOT/libs/$MODULE
   mkdir -p objs
   make TARGET=LINUX $MAKECMD $@ 2>&1 | tee objs/build.log
   if [ ${PIPESTATUS[0]} -eq 2 ]
   then
      RESULT=1
   fi
done

for MODULE in tucucommon tucucore tucucli
do
   cd $TUCUXI_ROOT/src/$MODULE
   mkdir -p objs
   make TARGET=LINUX $MAKECMD $@ 2>&1 | tee objs/build.log
   if [ ${PIPESTATUS[0]} -eq 2 ]
   then
      RESULT=1
   fi
done

for MODULE in tucucommon tucucore
do
   cd $TUCUXI_ROOT/test/$MODULE
   mkdir -p objs
   make TARGET=LINUX $MAKECMD $@ 2>&1 | tee objs/build.log
   if [ ${PIPESTATUS[0]} -eq 2 ]
   then
      RESULT=1
   fi
done

doxygen $TUCUXI_ROOT/src/doxyfile $@ 2>&1 | tee objs/doxigen.log
#if [ ${PIPESTATUS[0]} -ne 0 ]
#then
#  RESULT=1
#fi

exit $RESULT
