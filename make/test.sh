#!/bin/bash

RESULT=0

for MODULE in tucucommon
do
   cd $TUCUXI_ROOT/test/$MODULE
   objs/$MODULE-test $@ 2>&1 | tee objs/$MODULE-test.log
   if [ ${PIPESTATUS[0]} -ne 0 ]
   then
      RESULT=1
   fi
done

exit $RESULT
