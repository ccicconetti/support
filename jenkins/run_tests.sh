#!/bin/bash

if [ "$WORKSPACE" == "" ] ; then
  WORKSPACE=$PWD
fi

if [ "$COMPILER" == "" ] ; then
  COMPILER=g++
fi

if [ "$CONCURRENCY" == "" ] ; then
  CONCURRENCY=5
fi

echo "WORKSPACE:   ${WORKSPACE}"
echo "COMPILER:    ${COMPILER}"
echo "CONCURRENCY: ${CONCURRENCY}"

cd ${WORKSPACE}/build/debug

if [ -z ${NOBUILD} ] ; then

  echo "cleaning previous compilation"
  rm -rf * >& /dev/null

  ../build.sh ${COMPILER}

  if [ $? -ne 0 ]; then
    # exit immediately if cmake fails
    echo "$module: cmake failed"
    exit 1
  fi

  make -j${CONCURRENCY}

  if [ $? -ne 0 ] ; then
    # exit immediately if make fails
    echo "$module: make failed"
    exit 1
  fi

fi

ret=0
for testunit in $(find . | grep 'Test/test' | grep -v ".cmake") ; do

  report=$(basename $testunit)

  $testunit \
    --gtest_shuffle \
    --gtest_output=xml:${WORKSPACE}/$report.xml

  if [ $? -ne 0 ] ; then
    # if a test unit fails, mark the whole execution as failed but go on
    ret=1
  fi

done

exit $ret
