# uiiit::support library

General-purpose C++ utility classes used in other projects within the [Ubiquitous Internet research group at IIT-CNR](http://cnd.iit.cnr.it/) (uiiit).

## Build instructions

First, make sure you have [CMake](https://cmake.org/) (version >= 3.2), a recent C++ compiler and all the dependencies installed:

- [glog](https://github.com/google/glog)
- [Boost](https://www.boost.org/)
- [gRPC](https://grpc.io/)
- [protobuf](https://developers.google.com/protocol-buffers/)

Very likely you can find packaged versions of the first two (glog and Boost) in your system's package repository.
Make sure you are installing the development version of the packages, which also include header files.
gRPC is better built from source, and it may automatically download and compile protobuf too (recommended).

Note that [gmock](https://github.com/google/googlemock) is also needed to compile the unit tests but, if everything goes right, it will be downloaded automatically by CMake (needless to say: you do need a working Internet connection for this step).

```
git clone git@github.com:ccicconetti/support.git
cd support
```

Once everything is ready (assuming `clang++` is your compiler):

```
cd build/debug
../build.sh clang++
make
```

This will compile the full build tree consisting of:

1. `RpcSupport`: library with simple wrappers of gRPC client/server
2. `Support: generic support library
3. `Test`: unit tests, which you can execute with `Test/testsupport`

If you want to compile with compiler optimisations and no assertions:

```
cd build/release
../build.sh clang++
make
```

The unit tests will not be compiled (gmock will not be even downloaded).

## How to use in your code

At least two ways:

1. [recommended] Include the full git source tree in your project as a submodule (e.g., under `support`), add `add_subdirectory(support/Support)` and `add_subdirectory(support/RpcSupport)` directives to your `CMakeLists.txt` file, fix the header inclusion path so that is also searches in there, and finally in all your targets that use the library add `uiiitsupport` and `uiiitrpcsupport`

2. Copy the header files in your header inclusion path, copy the compiled libraries in your library inclusion path.

Method 1 is preferrable because: the dependency stays together with your code; if improvements/fixes are made to the library you just have to update the submodule.

## List of modules

### uiiit::rpc

- `SimpleServer`, `SimpleStreamingServer`: gRPC server wrapper classes
- `SimpleClient`: gRPC client wrapper class

### uiiit::support

- `Chrono`: chronometer
- `CliOptions`: wrapper of `boost::program_options`
- `Conf`: key/value parser
- `GlogRaii`: clear start-up/tear-down of the glog sub-system
- `Histogram`: binned histogram
- `LinearEstimation`: linear regression
- `MmTable`: formats string as a [Mattermost](https://mattermost.com/) table
- `MovingAvg`, `MovingVariance`: average, variance over a moving window
- `PeriodicTask`: execute a task periodically in a dedicated thread
- `Process`: query the user/system load of the current process
- `Queue`: blocking thread-safe queue
- `Random`: wrapper of some `std::random` r.v.'s
- `SignalHandlerFlag`, `SignalHandlerWait`: captures SIGINT and sets a flag when received or waits until received
- `Stat`: wrapper of `boost::accumulators`
- `System`: basic system information
- `ThreadPool`: pool of thread doing something
- `Thrower`: wrapper to check/format C++ exceptions
- `Uuid`: wrapper of `boost::uuids::uiiid`
