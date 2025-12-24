## Project Setup

We use the following development tools:

```shell
clang++20 -std=gnu++23
clangd
clang-tidy
cmake v3.27.0-rc3
```

To build `clang`, the sanitisers and the utilities `clangd, clang-tidy` from source, perform the following steps:

```shell
git clone https://github.com/llvm/llvm-project.git

# from llvm-project/
mkdir build-llvm

cmake -G Ninja -S ./llvm -B ./build-llvm -DLLVM_ENABLE_PROJECTS="clang;clang-tools-extra;compiler-rt;" -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=g++

cmake --build ./build-llvm --target clang
cmake --build ./build-llvm --target clangd
cmake --build ./build-llvm --target clang-tidy

cd build-llvm

# from llvm-project/build/llvm
sudo ninja install-clang
sudo ninja install-clangd
sudo ninja install-clang-tidy
# installed to /usr/local
```

A `.clangd` file is added at the project root. This passes instructions to `clangd` to ensure it looks in the correct places for our system libraries. In our case, these were installed at `/usr/local/lib/clang/20`.

## CMake

The project's build instructions are specified in the `CMakeLists.txt` file at root. The important things to note are:
- The C++ standard used is `gnu++23` and is set by the line `set(CMAKE_CXX_STANDARD 23)`;
- After building, the binaries are stored at `build/bin`.

Note that to switch to the `clang` compiler we need to add `-DCMAKE_C_COMPILER=/path/to/clang -DCMAKE_CXX_COMPILER=/path/to/clang++` to every command-line invocation of `cmake`. For me, my path is `/usr/local/bin/clang-20`. This is already done by the `CMake Tools` extension (by using the VSCode interface to select the appropriate "CMake Kits").

## External Libraries

All external libraries are installed to `lib/`. Each library needs to be added to the list of directories searched for both `include`s and linkage by CMake. In general, this is done by:
- adding the parent folder(s) of the header file(s) we want to access;
- adding the parent folder(s) of the built object files for linking, if any.

We use the [**HiGHS**](https://highs.dev/) linear programming library. Build instructions can be found [here](https://github.com/ERGO-Code/HiGHS/blob/master/cmake/README.md). Example program found [here](https://github.com/ERGO-Code/HiGHS/blob/master/examples/call_highs_from_cpp.cpp).

Build steps:
1. Clone from the github repo
2. `cmake -S . -B build -DCMAKE_INSTALL_PREFIX=$ROOT/lib/HiGHS`
3. `cmake --build build`
4. `cmake --install build`

To configure for use, add the following to our root `CMakeLists.txt` file:

```cmake
include_directories("lib/HiGHS/include/highs")
link_directories("lib/HiGHS/lib")
```

For `src/CMakeLists.txt`, add

```cmake
target_link_libraries(main PRIVATE highs)
```

Our library can now be imported with:

```cpp
#include <Highs.h>
```

## Unit Testing

We use the [**doctest**](https://github.com/doctest/doctest?tab=readme-ov-file) unit testing library. The `doctest.h` header file is installed under `lib/doctest`.

To configure for use, add the following to our root `CMakeLists.txt` file:

```cmake
include_directories("lib/doctest")
```

Since there are no `cpp` or object files to be built, no linking is necessary.

The unit testing library can now be imported with the following line in every test file:

```cpp
#include <doctest.h>
```

The testing entrypoint is `tests/test_main.cpp` and is as minimal as `doctest` allows, using their pre-built `main()` macro. Individual files then define `TEST_SUITE`s grouping `TEST_CASE`s by the classes they are testing.

To allow for the inclusion of all test files, it suffices to add them via `tests/CMakeLists.txt`:

```cmake
add_executable(tests ${entry_test} ${sources_test} ${sources})
```

The test executable is compiled to `build/bin/tests`.

## Development Environment

To debug `src/main.cpp`, run the default `VSCode-cpptools` debugger with the following `launch.json` configuration:

```json
    {
        "name": "Debug main",
        "type": "cppdbg",
        "request": "launch",
        // Resolved by CMake Tools. See comment below
        "program": "${command:cmake.launchTargetDirectory}/main",
        "args": ["..."],
        "stopAtEntry": false,
        "cwd": "${workspaceFolder}",
        "environment": [
            {
                // add the directory where our target was built to the PATHs
                // it gets resolved by CMake Tools:
                "name": "PATH",
                "value": "${env:PATH}:${command:cmake.launchTargetDirectory}"
            }
        ],
        "externalConsole": false,
        "MIMode": "gdb",
        "setupCommands": [
            {
                "description": "Enable pretty-printing for gdb",
                "text": "-enable-pretty-printing",
                "ignoreFailures": true
            }
        ]
    }
```

Comment: It is a limitation of both the `VSCode-cpptools` debugger and the `CMakeTools` extension that we cannot define individual running and debugging targets for each CMake target. As a result,
- `CMakeTools` introduces the concept of a "launch target" - which refers to the default CMake target that the debugger will run. This "launch target" is set on build.
- The "launch target" is stored in `cmake.launchTargetPath` and subsequently accessible by `cpptools`. 
- Furthermore, `cpptools` does NOT support "run without debugging". This is a [long-standing issue](https://github.com/microsoft/vscode-cpptools/issues/5680).
- Consequently, for our use case where we want to `run tests` without debugging, and `debug main`, we would have to constantly re-build and switch between the two tools and menus.

To work around this limitation, we can define two separate configurations in `launch.json`, one with `{cmake.launchTargetDirectory}/main` and the other with `.../tests`. These will launch the C/C++ debugger. We also configure `CMakeTools` to permanently use the `tests` launch target, so we can just press play to run all tests without debugging.