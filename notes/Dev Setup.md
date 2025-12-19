### Project Setup

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

The project's build instructions are specified in the `CMakeLists.txt` file at root. The important things to note are:
- The C++ standard used is `gnu++23` and is set by the line `set(CMAKE_CXX_STANDARD 23)`
- After building, the binaries are stored at `build/bin`.
- To switch to the `clang` compiler we need to add `-DCMAKE_C_COMPILER=/path/to/clang -DCMAKE_CXX_COMPILER=/path/to/clang++` to every invocation of `cmake`. For me, my path is `/usr/local/bin/clang-20`. This is already done by VSCode's "CMake kits", but is important to note for command-line invocations.

### External Libraries

We use the **Eigen** linear algebra library. Instructions coming soon.

### Development Environment

To debug `main.cpp`, run the default VSCode (c_cpp_tools) debugger with the following `launch.json` configuration:

```json
        {
            "name": "Debug main",
            "type": "cppdbg",
            "request": "launch",
            // Resolved by CMake Tools:
            "program": "${command:cmake.launchTargetPath}",
            "args": ["-f", "problem.txt", "-p", "problem1", "-o", "out.txt"],
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