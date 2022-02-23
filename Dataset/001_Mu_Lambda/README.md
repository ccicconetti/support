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

Create a symbolic link of the executable file in the current directory:

```
ln -s ../../release/Dataset/afdb-cost .
```

Analyze the trace with default cost values:

```
./afdb-cost --input azurefunctions-accesses-2020-sorted.csv
```

This will produce the file `azurefunctions-accesses-2020-sorted.csv-cost.dat`, where the meaning of the columns is explained by:

```
./afdb-cost --explain
```

## Reproduce experiments

If you want to reproduce the results obtained in the paper execute:

```
./run.sh
```

This will produce the results in a newly created directory `cost/`.
You can plot some statistics with Gnuplot:

```
cd graph
for i in *.plt ; do gnuplot -persist $i ; done
```
