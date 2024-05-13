## Pre-requisites

On Ubuntu you need the following packages installed:
_build-essential_, _cmake_, _doxygen_, and _git_.

```shell
sudo apt install build-essential cmake doxygen git libboost-all-dev libgtest-dev
```

In other Linux distributions you need equivalent packages installed.

**Attention:**  The tests were develelopped using google test version 1.11.0

## Compiling

### Preparing the compilation environment

From the command line navigate to the directory of the source code tree and create a build directory:

```shell
mkdir build
```

Next, navigate to that build directory and run [`cmake`](https://cmake.org/cmake/help/latest/manual/cmake.1.html#manual:cmake(1)) to configure the project and generate a native build system

```shell
cd build
cmake ..
```

### Compiling the code

Execute the call for the build system to actually compile/link the project:

```shell
cmake --build .
```

The resulting executables will be located in the projects `bin` folder.

## Testing the code

The tests were constructed using the [Ctest](https://cmake.org/cmake/help/book/mastering-cmake/chapter/Testing%20With%20CMake%20and%20CTest.html) and [Google Test](https://google.github.io/googletest/) tools. To run them, you first need to compile the project, using the steps above. After that, from the project directory, execute the following commands:

```shell
cd bin/tests
./<test_to_run>
```

### Types of tests developed

There were two types of tests developed:

* **Unit** Tests: found inside the folders `tests`, but not sub-folders
* **End-to-End** Tests: found inside the folder `tests/test_complete_system`

# Bibliography

Google Test guide [here](https://matgomes.com/integrate-google-test-into-cmake/)

CTest with CMake documentation [here](https://cmake.org/cmake/help/book/mastering-cmake/chapter/Testing%20With%20CMake%20and%20CTest.html)
