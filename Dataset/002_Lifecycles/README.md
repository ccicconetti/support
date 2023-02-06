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

Download and decompress the dataset (5.1 GB):

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

Create the directory that will store the results:

```
mkdir lifecycles
```

Analyze the trace:

```
./afdb --input azurefunctions-accesses-2020-sorted.csv --output-dir lifecycles --analysis lifecycles
```

You can then plot some statistics with Gnuplot:

```
./prepare.sh
cd graph
for i in *.plt ; do gnuplot -persist $i ; done
```
