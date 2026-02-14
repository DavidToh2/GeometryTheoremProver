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

Current code length: 14682 lines

## Milestone contri-simtri

```
Loading problem p1a
Solving problem p1a
-------- Iteration 0: --------
Derived 3 new predicates from DD and 4 new predicates from AR.
SOLVED!! Conclusion reached at iteration 0!
Total time: 5189 us

Loading problem p1b
Solving problem p1b
-------- Iteration 0: --------
Derived 1 new predicates from DD and 0 new predicates from AR.
-------- Iteration 1: --------
Derived 2 new predicates from DD and 4 new predicates from AR.
SOLVED!! Conclusion reached at iteration 1!
Total time: 11423 us

Loading problem p2a
Solving problem p2a
-------- Iteration 0: --------
Derived 3 new predicates from DD and 12 new predicates from AR.
-------- Iteration 1: --------
Derived 7 new predicates from DD and 0 new predicates from AR.
-------- Iteration 2: --------
Derived 9 new predicates from DD and 5 new predicates from AR.
SOLVED!! Conclusion reached at iteration 2!
Total time: 136553 us

Loading problem p2b
Solving problem p2b
-------- Iteration 0: --------
Derived 3 new predicates from DD and 12 new predicates from AR.
-------- Iteration 1: --------
Derived 7 new predicates from DD and 0 new predicates from AR.
SOLVED!! Conclusion reached at iteration 1!
Total time: 63131 us

Loading problem p2c
Solving problem p2c
-------- Iteration 0: --------
Derived 3 new predicates from DD and 12 new predicates from AR.
-------- Iteration 1: --------
Derived 14 new predicates from DD and 30 new predicates from AR.
SOLVED!! Conclusion reached at iteration 1!
Total time: 102964 us

Loading problem 3a
Solving problem 3a
-------- Iteration 0: --------
Derived 12 new predicates from DD and 0 new predicates from AR.
-------- Iteration 1: --------
Derived 2 new predicates from DD and 4 new predicates from AR.
SOLVED!! Conclusion reached at iteration 1!
Total time: 17951 us

Loading problem 3b
Solving problem 3b
-------- Iteration 0: --------
Derived 12 new predicates from DD and 0 new predicates from AR.
-------- Iteration 1: --------
Derived 2 new predicates from DD and 4 new predicates from AR.
SOLVED!! Conclusion reached at iteration 1!
Total time: 21746 us

Loading problem 4a
Solving problem 4a
-------- Iteration 0: --------
Derived 0 new predicates from DD and 4 new predicates from AR.
-------- Iteration 1: --------
Derived 2 new predicates from DD and 0 new predicates from AR.
SOLVED!! Conclusion reached at iteration 1!
Total time: 11228 us

Loading problem 4b
Solving problem 4b
-------- Iteration 0: --------
Derived 0 new predicates from DD and 4 new predicates from AR.
-------- Iteration 1: --------
Derived 2 new predicates from DD and 0 new predicates from AR.
SOLVED!! Conclusion reached at iteration 1!
Total time: 10199 us

Loading problem 5
Solving problem 5
-------- Iteration 0: --------
Derived 6 new predicates from DD and 12 new predicates from AR.
-------- Iteration 1: --------
Derived 33 new predicates from DD and 54 new predicates from AR.
-------- Iteration 2: --------
Derived 12 new predicates from DD and 12 new predicates from AR.
SOLVED!! Conclusion reached at iteration 2!
Total time: 500920 us

Loading problem 6
Solving problem 6
-------- Iteration 0: --------
Derived 10 new predicates from DD and 78 new predicates from AR.
-------- Iteration 1: --------
Derived 15 new predicates from DD and 0 new predicates from AR.
SOLVED!! Conclusion reached at iteration 1!
Total time: 209368 us

Loading problem 7
Solving problem 7
-------- Iteration 0: --------
Derived 0 new predicates from DD and 4 new predicates from AR.
-------- Iteration 1: --------
Derived 1 new predicates from DD and 2 new predicates from AR.
SOLVED!! Conclusion reached at iteration 1!
Total time: 7750 us

Loading problem 8a
Solving problem 8a
-------- Iteration 0: --------
Derived 8 new predicates from DD and 18 new predicates from AR.
-------- Iteration 1: --------
Derived 45 new predicates from DD and 94 new predicates from AR.
-------- Iteration 2: --------
Derived 21 new predicates from DD and 30 new predicates from AR.
-------- Iteration 3: --------
Derived 4 new predicates from DD and 58 new predicates from AR.
SOLVED!! Conclusion reached at iteration 3!
Total time: 1915003 us

Loading problem 8b
Solving problem 8b
-------- Iteration 0: --------
Derived 8 new predicates from DD and 18 new predicates from AR.
-------- Iteration 1: --------
Derived 45 new predicates from DD and 94 new predicates from AR.
-------- Iteration 2: --------
Derived 21 new predicates from DD and 30 new predicates from AR.
-------- Iteration 3: --------
Derived 4 new predicates from DD and 58 new predicates from AR.
SOLVED!! Conclusion reached at iteration 3!
Total time: 1933718 us

Loading problem 9
Solving problem 9
-------- Iteration 0: --------
Derived 2 new predicates from DD and 12 new predicates from AR.
-------- Iteration 1: --------
Derived 3 new predicates from DD and 8 new predicates from AR.
SOLVED!! Conclusion reached at iteration 1!
Total time: 58501 us

Loading problem 10
Solving problem 10
-------- Iteration 0: --------
Derived 24 new predicates from DD and 0 new predicates from AR.
-------- Iteration 1: --------
Derived 4 new predicates from DD and 12 new predicates from AR.
-------- Iteration 2: --------
Derived 1 new predicates from DD and 0 new predicates from AR.
SOLVED!! Conclusion reached at iteration 2!
Total time: 86280 us

Loading problem 11
Solving problem 11
-------- Iteration 0: --------
Derived 15 new predicates from DD and 36 new predicates from AR.
-------- Iteration 1: --------
Derived 12 new predicates from DD and 25 new predicates from AR.
-------- Iteration 2: --------
Derived 12 new predicates from DD and 20 new predicates from AR.
SOLVED!! Conclusion reached at iteration 2!
Total time: 460454 us

Loading problem 12
Solving problem 12
-------- Iteration 0: --------
Derived 44 new predicates from DD and 30 new predicates from AR.
-------- Iteration 1: --------
Derived 17 new predicates from DD and 44 new predicates from AR.
-------- Iteration 2: --------
Derived 17 new predicates from DD and 33 new predicates from AR.
SOLVED!! Conclusion reached at iteration 2!
Total time: 536357 us

Loading problem 13a
Solving problem 13a
-------- Iteration 0: --------
Derived 2 new predicates from DD and 6 new predicates from AR.
-------- Iteration 1: --------
Derived 12 new predicates from DD and 1 new predicates from AR.
-------- Iteration 2: --------
Derived 7 new predicates from DD and 2 new predicates from AR.
SOLVED!! Conclusion reached at iteration 2!
Total time: 86493 us

Loading problem 13b
Solving problem 13b
-------- Iteration 0: --------
Derived 2 new predicates from DD and 6 new predicates from AR.
-------- Iteration 1: --------
Derived 12 new predicates from DD and 1 new predicates from AR.
-------- Iteration 2: --------
Derived 7 new predicates from DD and 2 new predicates from AR.
SOLVED!! Conclusion reached at iteration 2!
Total time: 84440 us

Loading problem 14a
Solving problem 14a
-------- Iteration 0: --------
Derived 22 new predicates from DD and 109 new predicates from AR.
-------- Iteration 1: --------
Derived 22 new predicates from DD and 8 new predicates from AR.
-------- Iteration 2: --------
Derived 11 new predicates from DD and 43 new predicates from AR.
-------- Iteration 3: --------
Derived 6 new predicates from DD and 22 new predicates from AR.
SOLVED!! Conclusion reached at iteration 3!
Total time: 2888181 us

Loading problem 14b
Solving problem 14b
-------- Iteration 0: --------
Derived 22 new predicates from DD and 109 new predicates from AR.
-------- Iteration 1: --------
Derived 22 new predicates from DD and 8 new predicates from AR.
-------- Iteration 2: --------
Derived 11 new predicates from DD and 43 new predicates from AR.
-------- Iteration 3: --------
Derived 6 new predicates from DD and 22 new predicates from AR.
SOLVED!! Conclusion reached at iteration 3!
Total time: 2934456 us

Loading problem 15
Solving problem 15
-------- Iteration 0: --------
Derived 15 new predicates from DD and 36 new predicates from AR.
-------- Iteration 1: --------
Derived 10 new predicates from DD and 21 new predicates from AR.
SOLVED!! Conclusion reached at iteration 1!
Total time: 132653 us

Loading problem 16a
Solving problem 16a
-------- Iteration 0: --------
Derived 26 new predicates from DD and 10 new predicates from AR.
-------- Iteration 1: --------
Derived 23 new predicates from DD and 15 new predicates from AR.
-------- Iteration 2: --------
Derived 10 new predicates from DD and 6 new predicates from AR.
-------- Iteration 3: --------
Derived 3 new predicates from DD and 0 new predicates from AR.
SOLVED!! Conclusion reached at iteration 3!
Total time: 517536 us

Loading problem 16b
Solving problem 16b
-------- Iteration 0: --------
Derived 26 new predicates from DD and 10 new predicates from AR.
-------- Iteration 1: --------
Derived 23 new predicates from DD and 15 new predicates from AR.
-------- Iteration 2: --------
Derived 10 new predicates from DD and 6 new predicates from AR.
-------- Iteration 3: --------
Derived 3 new predicates from DD and 0 new predicates from AR.
-------- Iteration 4: --------
Derived 12 new predicates from DD and 26 new predicates from AR.
-------- Iteration 5: --------
Derived 8 new predicates from DD and 6 new predicates from AR.
SOLVED!! Conclusion reached at iteration 5!
Total time: 1074660 us

Loading problem 17
Solving problem 17
-------- Iteration 0: --------
Derived 10 new predicates from DD and 84 new predicates from AR.
-------- Iteration 1: --------
Derived 36 new predicates from DD and 60 new predicates from AR.
-------- Iteration 2: --------
Derived 16 new predicates from DD and 118 new predicates from AR.
SOLVED!! Conclusion reached at iteration 2!
Total time: 1592135 us

Solved 26 out of 26 problems.
```