```
Milestone "perp-para-coll-eqangle"
tests/perp-para-coll-eqangle/problems.txt
tests/perp-para-coll-eqangle/rules.txt

Solving problem p1
Conclusion reached at iteration 0!
Total time: 3855 us
Solving problem p2
Conclusion reached at iteration 2!
Total time: 93257 us
Solving problem p3
Conclusion reached at iteration 3!
Total time: 1134923 us
Solving problem p4
Conclusion reached at iteration 1!
Total time: 81654 us
Solving problem p5
Conclusion reached at iteration 3!
Total time: 381820 us
Solving problem p6
Conclusion reached at iteration 2!
Total time: 126827 us
```

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

Current code length: 9042 lines