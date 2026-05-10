Project structure (including `.gitignore`-d files):

```
.cache          (for clangd)
.venv           (for profiler.ipynb)
.vscode         (VSCode dev-environment files)
build           (CMake build directory)
    /bin             (contains executables)
lib             (header and object files for external libraries)
notes           (development notes)
problems        (problem banks for deployment testing)
src             (source code)
    /main.cpp        (main program entrypoint)
tests           (unit testing code)
    /test_main.cpp   (unit testing entrypoint)
.clangd
CMakeLists.txt
```

Read [Dev Setup.md](notes/Dev%20Setup.md) for setup instructions, as well as [the other project notes](notes/).

On successful setup and compilation, the program may be called from `./build/bin/main`. The following command-line options are available:

```
-f, --problem_file          NECESSARY
-p, --problem_name          OPTIONAL    If not passed, runs all problems in problem_file 
                                        sequentially
-r, --rule_file             OPTIONAL    Defaults to ./problems/rules.txt
-c, --construction_file     OPTIONAL    Defaults to ./problems/constructions.txt
-o, --output_file           NECESSARY   
-g, --profiler_output_file  OPTIONAL    If not passed, profiler does not run
```

Current code length: 21133 lines
