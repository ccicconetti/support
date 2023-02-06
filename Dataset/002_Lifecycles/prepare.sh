#!/bin/bash

dirs="graph lifecycles"
for d in $dirs ; do
  if [ ! -d $d ] ; then
    echo "directory '$d' is missing, bailing out"
    exit 1
  fi
done

if [ -a graph/lifecycles.dat ] ; then
  echo "output file exists, will not overwrite"
  exit 1
fi

cat $(ls lifecycles/*.dat | grep -v concurrent.dat) | grep -v ',0,0,' > graph/lifecycles.dat
