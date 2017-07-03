#!/bin/bash

RESULT=0

for MODULE in tucucommon
do
   cd $TUCUXI_ROOT/test/$MODULE
   objs/$MODULE-test
   if [ $? -ne 0 ]
   then
      RESULT=1
   fi
done

exit $RESULT
