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

Current code length: 12637 lines

## Current milestone: cyclic-ar-angles

```
Loading problem p1
Solving problem p1
-------- Iteration 0: --------
Derived 3 new predicates from DD and 8 new predicates from AR.
SOLVED!! Conclusion reached at iteration 0!
Total time: 7679 us

Loading problem p2
Solving problem p2
-------- Iteration 0: --------
Derived 5 new predicates from DD and 5 new predicates from AR.
SOLVED!! Conclusion reached at iteration 0!
Total time: 29594 us

Loading problem p3
Solving problem p3
-------- Iteration 0: --------
Derived 14 new predicates from DD and 5 new predicates from AR.
-------- Iteration 1: --------
Derived 3 new predicates from DD and 2 new predicates from AR.
SOLVED!! Conclusion reached at iteration 1!
Total time: 281262 us

Loading problem p4
Solving problem p4
-------- Iteration 0: --------
Derived 4 new predicates from DD and 6 new predicates from AR.
-------- Iteration 1: --------
Derived 2 new predicates from DD and 0 new predicates from AR.
-------- Iteration 2: --------
Derived 6 new predicates from DD and 3 new predicates from AR.
SOLVED!! Conclusion reached at iteration 2!
Total time: 72939 us

Loading problem p5
Solving problem p5
-------- Iteration 0: --------
Derived 16 new predicates from DD and 31 new predicates from AR.
-------- Iteration 1: --------
Derived 5 new predicates from DD and 0 new predicates from AR.
SOLVED!! Conclusion reached at iteration 1!
Total time: 119857 us

Loading problem p6
Solving problem p6
-------- Iteration 0: --------
Derived 24 new predicates from DD and 5 new predicates from AR.
SOLVED!! Conclusion reached at iteration 0!
Total time: 5668 us

Loading problem p7
Solving problem p7
-------- Iteration 0: --------
Derived 12 new predicates from DD and 13 new predicates from AR.
SOLVED!! Conclusion reached at iteration 0!
Total time: 5720 us

Loading problem p8
Solving problem p8
-------- Iteration 0: --------
Derived 24 new predicates from DD and 4 new predicates from AR.
-------- Iteration 1: --------
Derived 2 new predicates from DD and 0 new predicates from AR.
SOLVED!! Conclusion reached at iteration 1!
Total time: 23471 us

Loading problem p9
Solving problem p9
-------- Iteration 0: --------
Derived 8 new predicates from DD and 20 new predicates from AR.
-------- Iteration 1: --------
Derived 8 new predicates from DD and 16 new predicates from AR.
-------- Iteration 2: --------
Derived 13 new predicates from DD and 0 new predicates from AR.
-------- Iteration 3: --------
Derived 49 new predicates from DD and 59 new predicates from AR.
SOLVED!! Conclusion reached at iteration 3!
Total time: 5911884 us

Loading problem p10
Solving problem p10
-------- Iteration 0: --------
Derived 17 new predicates from DD and 45 new predicates from AR.
SOLVED!! Conclusion reached at iteration 0!
Total time: 89845 us
```