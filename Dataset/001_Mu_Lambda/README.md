# Reproducibility

## Building instructions

Dependencies:

- cmake >= 3.2
- recent C++ compiler (GNU gcc >= 7 or LLVM clang >= 10)
- Google's glog library
- non-ancient Boost libraries

Clone repository:

```
git clone https://github.com/ccicconetti/support.git
cd support
```

To build with optimizations:

```
mkdir release
cd release
cmake -DCMAKE_BUILD_TYPE=release ../
```

## Analyze dataset

azurefunctions-accesses-2020.csv

Download and decompress the dataset:

```
wget https://azurecloudpublicdataset2.blob.core.windows.net/azurepublicdatasetv2/azurefunctions_dataset2020/azurefunctions-accesses-2020.csv.bz2
bunzip2 azurefunctions-accesses-2020.csv.bz2
```

Sort the dataset by timestamp:

```
sort -n azurefunctions-accesses-2020.csv > azurefunctions-accesses-2020-sorted.csv
```

Analyze the trace:

```
mkdir cost
./afdb-cost --output-dir cost --input azurefunctions-accesses-2020-sorted.csv
```

This will produce the file `cost/azurefunctions-accesses-2020-sorted.csv-cost.dat`.
The meaning of the columns is explained by:

```
./afdb-cost --explain
```

You can plot some statistics with, e.g.:

```
cd graph
gnuplot -persist cost-comparison.plt
```
