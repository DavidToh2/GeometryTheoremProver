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

Current code length: 12438 lines

## Current milestone: cyclic-ar-angles

```
Loading problem p1
Solving problem p1
-------- Iteration 0: --------
Derived 3 new predicates from DD and 8 new predicates from AR.
SOLVED!! Conclusion reached at iteration 0!
Total time: 8390 us

Loading problem p2
Solving problem p2
-------- Iteration 0: --------
Derived 5 new predicates from DD and 5 new predicates from AR.
SOLVED!! Conclusion reached at iteration 0!
Total time: 24119 us

Loading problem p3
Solving problem p3
-------- Iteration 0: --------
Derived 14 new predicates from DD and 5 new predicates from AR.
-------- Iteration 1: --------
Derived 3 new predicates from DD and 2 new predicates from AR.
SOLVED!! Conclusion reached at iteration 1!
Total time: 481891 us

Loading problem p4
Solving problem p4
-------- Iteration 0: --------
Derived 4 new predicates from DD and 6 new predicates from AR.
-------- Iteration 1: --------
Derived 2 new predicates from DD and 0 new predicates from AR.
-------- Iteration 2: --------
Derived 6 new predicates from DD and 4 new predicates from AR.
SOLVED!! Conclusion reached at iteration 2!
Total time: 73785 us

Loading problem p5
Solving problem p5
-------- Iteration 0: --------
Derived 18 new predicates from DD and 38 new predicates from AR.
-------- Iteration 1: --------
Derived 4 new predicates from DD and 0 new predicates from AR.
SOLVED!! Conclusion reached at iteration 1!
Total time: 274177 us

Loading problem p6
Solving problem p6
-------- Iteration 0: --------
Derived 24 new predicates from DD and 5 new predicates from AR.
SOLVED!! Conclusion reached at iteration 0!
Total time: 6144 us

Loading problem p7
Solving problem p7
-------- Iteration 0: --------
Derived 12 new predicates from DD and 13 new predicates from AR.
SOLVED!! Conclusion reached at iteration 0!
Total time: 12451 us

Loading problem p8
Solving problem p8
-------- Iteration 0: --------
Derived 24 new predicates from DD and 4 new predicates from AR.
-------- Iteration 1: --------
Derived 2 new predicates from DD and 0 new predicates from AR.
SOLVED!! Conclusion reached at iteration 1!
Total time: 162473 us
```