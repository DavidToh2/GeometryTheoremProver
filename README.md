Project structure (including `.gitignore`-d files):

```
.cache          (for clangd)
.vscode         (VSCode dev-environment files)
build           (CMake build directory)
    /bin             (contains executables)
lib             (header and object files for external libraries)
notes           (documentation)
problems        (problem banks for deployment testing)
src             (source code)
    /main.cpp        (main program entrypoint)
tests           (unit testing code)
    /test_main.cpp   (unit testing entrypoint)
.clangd
CMakeLists.txt
```

Read [Outline.md](notes/Outline.md) for technical specifications, as well as [the other project notes](notes/).

Current code length: 11053 lines