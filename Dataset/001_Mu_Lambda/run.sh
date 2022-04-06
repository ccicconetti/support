#!/bin/bash

cost_warm_mus="6.3e-3 6.3e-6"

mkdir cost 2> /dev/null

for cwm in $cost_warm_mus ; do
  cmd="./afdb-cost \
    --output-dir cost \
    --cost-exec-mu 0 \
    --cost-exec-lambda 0.6 \
    --cost-read-lambda 0.4 \
    --cost-write-lambda 5 \
    --cost-warm-mu $cwm \
    --cost-warm-lambda 0 \
    --cost-migrate-mu 12 \
    --cost-migrate-lambda 12 \
    --input ~/Data/AzureFunctionsBlobDataset2020/data/azurefunctions-accesses-2020-sorted.csv \
    --append"

  if [ "$DRY" == "" ] ; then
    GLOG_v=1 $cmd
  else
    echo $cmd
  fi
done
