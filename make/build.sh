#!/bin/bash

TUCUXI_MAKE=$( cd $( dirname ${BASH_SOURCE[0]} ) && pwd )
export TUCUXI_ROOT=$(dirname ${TUCUXI_MAKE})
echo $TUCUXI_ROOT

mkdir -p $TUCUXI_ROOT/bin

RESULT=0
BUILD_LIBS=1
BUILD_DOC=1

for i in "$@" ; do
   if [ $i == "clean" ] ; then
      MAKECMD=clean
   fi
   if [ $i == "nolib" ] ; then
      BUILD_LIBS=0
   fi
   if [ $i == "nodoc" ] ; then
      BUILD_DOC=0
   fi
done

if [ $BUILD_LIBS -eq 1 ] ; then
   for MODULE in tiny-js-master-20170629 botan-2.1.0
   do
      cd $TUCUXI_ROOT/libs/$MODULE
      mkdir -p objs
      make TARGET=LINUX $MAKECMD 2>&1 | tee objs/build.log
      if [ ${PIPESTATUS[0]} -eq 2 ] ; then
         RESULT=1
     fi
   done
fi

for MODULE in tucucommon tucucore tucuquery tucucli tucuvalidator tuculicense
do
   cd $TUCUXI_ROOT/src/$MODULE
   mkdir -p objs
   make TARGET=LINUX $MAKECMD 2>&1 | tee objs/build.log
   if [ ${PIPESTATUS[0]} -eq 2 ] ; then
      RESULT=1
   fi
done

for MODULE in tucucommon tucucore # tuculicense
do
   cd $TUCUXI_ROOT/test/$MODULE
   mkdir -p objs
   make TARGET=LINUX $MAKECMD 2>&1 | tee objs/build.log
   if [ ${PIPESTATUS[0]} -eq 2 ] ; then
      RESULT=1
   fi
done

if [ $BUILD_DOC -eq 1 ] ; then
   doxygen $TUCUXI_ROOT/src/doxyfile 2>&1 | tee objs/doxygen.log
   if [ ${PIPESTATUS[0]} -ne 0 ] ; then
     RESULT=1
   fi
fi

exit $RESULT
