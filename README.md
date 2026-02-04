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

Current code length: 12772 lines

## Current progress on 110 problems

```
Loading problem 110-1
Solving problem 110-1
-------- Iteration 0: --------
Derived 2 new predicates from DD and 4 new predicates from AR.
-------- Iteration 1: --------
Derived 13 new predicates from DD and 22 new predicates from AR.
SOLVED!! Conclusion reached at iteration 1!
Total time: 29629 us

Loading problem 110-2
Solving problem 110-2
-------- Iteration 0: --------
Derived 13 new predicates from DD and 30 new predicates from AR.
SOLVED!! Conclusion reached at iteration 0!
Total time: 31373 us

Loading problem 110-3
Solving problem 110-3
-------- Iteration 0: --------
Derived 4 new predicates from DD and 12 new predicates from AR.
-------- Iteration 1: --------
Derived 19 new predicates from DD and 10 new predicates from AR.
SOLVED!! Conclusion reached at iteration 1!
Total time: 143179 us

Loading problem 110-4
Solving problem 110-4
-------- Iteration 0: --------
Derived 22 new predicates from DD and 12 new predicates from AR.
SOLVED!! Conclusion reached at iteration 0!
Total time: 45399 us

Loading problem 110-5
Solving problem 110-5
-------- Iteration 0: --------
Derived 24 new predicates from DD and 30 new predicates from AR.
SOLVED!! Conclusion reached at iteration 0!
Total time: 128023 us

Loading problem 110-6
Solving problem 110-6
-------- Iteration 0: --------
Derived 1 new predicates from DD and 12 new predicates from AR.
SOLVED!! Conclusion reached at iteration 0!
Total time: 10690 us

Loading problem 110-7
Solving problem 110-7
-------- Iteration 0: --------
Derived 12 new predicates from DD and 18 new predicates from AR.
-------- Iteration 1: --------
Derived 3 new predicates from DD and 0 new predicates from AR.
-------- Iteration 2: --------
Derived 28 new predicates from DD and 2 new predicates from AR.
-------- Iteration 3: --------
Derived 1 new predicates from DD and 0 new predicates from AR.
SOLVED!! Conclusion reached at iteration 3!
Total time: 116224 us

Loading problem 110-8
Solving problem 110-8
-------- Iteration 0: --------
Derived 40 new predicates from DD and 8 new predicates from AR.
-------- Iteration 1: --------
Derived 2 new predicates from DD and 0 new predicates from AR.
-------- Iteration 2: --------
Derived 18 new predicates from DD and 16 new predicates from AR.
SOLVED!! Conclusion reached at iteration 2!
Total time: 191235 us

Loading problem 110-9
Solving problem 110-9
-------- Iteration 0: --------
Derived 90 new predicates from DD and 26 new predicates from AR.
-------- Iteration 1: --------
Derived 4 new predicates from DD and 0 new predicates from AR.
-------- Iteration 2: --------
Derived 64 new predicates from DD and 114 new predicates from AR.
SOLVED!! Conclusion reached at iteration 2!
Total time: 547391 us

Loading problem 110-10
Solving problem 110-10
-------- Iteration 0: --------
Derived 24 new predicates from DD and 4 new predicates from AR.
-------- Iteration 1: --------
Derived 2 new predicates from DD and 0 new predicates from AR.
SOLVED!! Conclusion reached at iteration 1!
Total time: 26295 us

Loading problem 110-11
Solving problem 110-11
-------- Iteration 0: --------
Derived 5 new predicates from DD and 0 new predicates from AR.
-------- Iteration 1: --------
Derived 13 new predicates from DD and 4 new predicates from AR.
-------- Iteration 2: --------
Derived 1 new predicates from DD and 0 new predicates from AR.
SOLVED!! Conclusion reached at iteration 2!
Total time: 77130 us

Loading problem 110-12
Solving problem 110-12
-------- Iteration 0: --------
Derived 13 new predicates from DD and 0 new predicates from AR.
-------- Iteration 1: --------
Derived 5 new predicates from DD and 1 new predicates from AR.
SOLVED!! Conclusion reached at iteration 1!
Total time: 20063 us

Loading problem 110-13
Solving problem 110-13
-------- Iteration 0: --------
Derived 0 new predicates from DD and 4 new predicates from AR.
-------- Iteration 1: --------
Derived 4 new predicates from DD and 0 new predicates from AR.
-------- Iteration 2: --------
Derived 34 new predicates from DD and 6 new predicates from AR.
SOLVED!! Conclusion reached at iteration 2!
Total time: 81475 us

Loading problem 110-14
Solving problem 110-14
-------- Iteration 0: --------
Derived 21 new predicates from DD and 73 new predicates from AR.
-------- Iteration 1: --------
Derived 4 new predicates from DD and 0 new predicates from AR.
SOLVED!! Conclusion reached at iteration 1!
Total time: 173537 us

Loading problem 110-15
Solving problem 110-15
-------- Iteration 0: --------
Derived 0 new predicates from DD and 12 new predicates from AR.
-------- Iteration 1: --------
Derived 9 new predicates from DD and 0 new predicates from AR.
-------- Iteration 2: --------
Derived 53 new predicates from DD and 3 new predicates from AR.
-------- Iteration 3: --------
Derived 2 new predicates from DD and 0 new predicates from AR.
SOLVED!! Conclusion reached at iteration 3!
Total time: 1063882 us

Loading problem 110-16
Solving problem 110-16
-------- Iteration 0: --------
Derived 3 new predicates from DD and 12 new predicates from AR.
SOLVED!! Conclusion reached at iteration 0!
Total time: 63146 us

Loading problem 110-17
Solving problem 110-17
-------- Iteration 0: --------
Derived 43 new predicates from DD and 12 new predicates from AR.
-------- Iteration 1: --------
Derived 2 new predicates from DD and 0 new predicates from AR.
-------- Iteration 2: --------
Derived 14 new predicates from DD and 17 new predicates from AR.
-------- Iteration 3: --------
Derived 1 new predicates from DD and 0 new predicates from AR.
SOLVED!! Conclusion reached at iteration 3!
Total time: 156771 us

Loading problem 110-18
Solving problem 110-18
-------- Iteration 0: --------
Derived 43 new predicates from DD and 12 new predicates from AR.
-------- Iteration 1: --------
Derived 2 new predicates from DD and 0 new predicates from AR.
-------- Iteration 2: --------
Derived 14 new predicates from DD and 17 new predicates from AR.
SOLVED!! Conclusion reached at iteration 2!
Total time: 104742 us

Loading problem 110-19
Solving problem 110-19
-------- Iteration 0: --------
Derived 24 new predicates from DD and 5 new predicates from AR.
SOLVED!! Conclusion reached at iteration 0!
Total time: 6089 us

Loading problem 110-20
Solving problem 110-20
-------- Iteration 0: --------
Derived 1 new predicates from DD and 12 new predicates from AR.
-------- Iteration 1: --------
Derived 5 new predicates from DD and 0 new predicates from AR.
-------- Iteration 2: --------
Derived 31 new predicates from DD and 0 new predicates from AR.
SOLVED!! Conclusion reached at iteration 2!
Total time: 73704 us

Loading problem 110-21
Solving problem 110-21
-------- Iteration 0: --------
Derived 7 new predicates from DD and 8 new predicates from AR.
SOLVED!! Conclusion reached at iteration 0!
Total time: 6423 us

Loading problem 110-22
Solving problem 110-22
-------- Iteration 0: --------
Derived 40 new predicates from DD and 16 new predicates from AR.
-------- Iteration 1: --------
Derived 3 new predicates from DD and 0 new predicates from AR.
SOLVED!! Conclusion reached at iteration 1!
Total time: 107781 us

Loading problem 110-23
Solving problem 110-23
-------- Iteration 0: --------
Derived 41 new predicates from DD and 8 new predicates from AR.
-------- Iteration 1: --------
Derived 1 new predicates from DD and 0 new predicates from AR.
SOLVED!! Conclusion reached at iteration 1!
Total time: 72717 us

Loading problem 110-24
Solving problem 110-24
-------- Iteration 0: --------
Derived 31 new predicates from DD and 2 new predicates from AR.
-------- Iteration 1: --------
Derived 1 new predicates from DD and 0 new predicates from AR.
-------- Iteration 2: --------
Derived 9 new predicates from DD and 0 new predicates from AR.
SOLVED!! Conclusion reached at iteration 2!
Total time: 56388 us

Loading problem 110-25
Solving problem 110-25
-------- Iteration 0: --------
Derived 9 new predicates from DD and 12 new predicates from AR.
-------- Iteration 1: --------
Derived 39 new predicates from DD and 96 new predicates from AR.
-------- Iteration 2: --------
Derived 8 new predicates from DD and 0 new predicates from AR.
SOLVED!! Conclusion reached at iteration 2!
Total time: 1010326 us

Loading problem 110-26
Solving problem 110-26
-------- Iteration 0: --------
Derived 24 new predicates from DD and 30 new predicates from AR.
SOLVED!! Conclusion reached at iteration 0!
Total time: 103435 us

Loading problem 110-27
Solving problem 110-27
-------- Iteration 0: --------
Derived 26 new predicates from DD and 30 new predicates from AR.
-------- Iteration 1: --------
Derived 7 new predicates from DD and 0 new predicates from AR.
SOLVED!! Conclusion reached at iteration 1!
Total time: 284345 us

Loading problem 110-28
Solving problem 110-28
-------- Iteration 0: --------
Derived 2 new predicates from DD and 12 new predicates from AR.
-------- Iteration 1: --------
Derived 12 new predicates from DD and 1 new predicates from AR.
-------- Iteration 2: --------
Derived 5 new predicates from DD and 0 new predicates from AR.
SOLVED!! Conclusion reached at iteration 2!
Total time: 521622 us

Loading problem 110-29
Solving problem 110-29
-------- Iteration 0: --------
Derived 0 new predicates from DD and 12 new predicates from AR.
-------- Iteration 1: --------
Derived 1 new predicates from DD and 0 new predicates from AR.
-------- Iteration 2: --------
Derived 10 new predicates from DD and 0 new predicates from AR.
-------- Iteration 3: --------
Derived 1 new predicates from DD and 0 new predicates from AR.
SOLVED!! Conclusion reached at iteration 3!
Total time: 45779 us

Loading problem 110-30
Solving problem 110-30
-------- Iteration 0: --------
Derived 7 new predicates from DD and 18 new predicates from AR.
-------- Iteration 1: --------
Derived 1 new predicates from DD and 0 new predicates from AR.
-------- Iteration 2: --------
Derived 10 new predicates from DD and 1 new predicates from AR.
SOLVED!! Conclusion reached at iteration 2!
Total time: 48777 us

Loading problem 110-31
Solving problem 110-31
-------- Iteration 0: --------
Derived 2 new predicates from DD and 2 new predicates from AR.
-------- Iteration 1: --------
Derived 1 new predicates from DD and 0 new predicates from AR.
SOLVED!! Conclusion reached at iteration 1!
Total time: 24292 us

Loading problem 110-32
Solving problem 110-32
-------- Iteration 0: --------
Derived 12 new predicates from DD and 12 new predicates from AR.
SOLVED!! Conclusion reached at iteration 0!
Total time: 5264 us

Loading problem 110-33
Solving problem 110-33
-------- Iteration 0: --------
Derived 12 new predicates from DD and 12 new predicates from AR.
-------- Iteration 1: --------
Derived 2 new predicates from DD and 0 new predicates from AR.
-------- Iteration 2: --------
Derived 18 new predicates from DD and 10 new predicates from AR.
SOLVED!! Conclusion reached at iteration 2!
Total time: 58835 us

Loading problem 110-34
Solving problem 110-34
-------- Iteration 0: --------
Derived 12 new predicates from DD and 11 new predicates from AR.
SOLVED!! Conclusion reached at iteration 0!
Total time: 7488 us

Loading problem 110-35
Solving problem 110-35
-------- Iteration 0: --------
Derived 12 new predicates from DD and 14 new predicates from AR.
-------- Iteration 1: --------
Derived 8 new predicates from DD and 0 new predicates from AR.
-------- Iteration 2: --------
Derived 30 new predicates from DD and 16 new predicates from AR.
SOLVED!! Conclusion reached at iteration 2!
Total time: 183195 us

Loading problem 110-36
Solving problem 110-36
-------- Iteration 0: --------
Derived 15 new predicates from DD and 29 new predicates from AR.
-------- Iteration 1: --------
Derived 8 new predicates from DD and 0 new predicates from AR.
SOLVED!! Conclusion reached at iteration 1!
Total time: 187524 us

Loading problem 110-37
Solving problem 110-37
-------- Iteration 0: --------
Derived 40 new predicates from DD and 10 new predicates from AR.
-------- Iteration 1: --------
Derived 1 new predicates from DD and 0 new predicates from AR.
SOLVED!! Conclusion reached at iteration 1!
Total time: 30800 us

Loading problem 110-38
Solving problem 110-38
-------- Iteration 0: --------
Derived 20 new predicates from DD and 46 new predicates from AR.
SOLVED!! Conclusion reached at iteration 0!
Total time: 29806 us

Loading problem 110-39
Solving problem 110-39
-------- Iteration 0: --------
Derived 40 new predicates from DD and 19 new predicates from AR.
SOLVED!! Conclusion reached at iteration 0!
Total time: 34269 us

Loading problem 110-40
Solving problem 110-40
-------- Iteration 0: --------
Derived 1 new predicates from DD and 0 new predicates from AR.
SOLVED!! Conclusion reached at iteration 0!
Total time: 2525 us

Loading problem 110-41
Solving problem 110-41
-------- Iteration 0: --------
Derived 7 new predicates from DD and 77 new predicates from AR.
-------- Iteration 1: --------
Derived 8 new predicates from DD and 0 new predicates from AR.
-------- Iteration 2: --------
Derived 28 new predicates from DD and 9 new predicates from AR.
-------- Iteration 3: --------
Derived 1 new predicates from DD and 0 new predicates from AR.
SOLVED!! Conclusion reached at iteration 3!
Total time: 404385 us

Loading problem 110-42
Solving problem 110-42
-------- Iteration 0: --------
Derived 3 new predicates from DD and 19 new predicates from AR.
-------- Iteration 1: --------
Derived 13 new predicates from DD and 13 new predicates from AR.
-------- Iteration 2: --------
Derived 11 new predicates from DD and 8 new predicates from AR.
-------- Iteration 3: --------
Derived 1 new predicates from DD and 0 new predicates from AR.
SOLVED!! Conclusion reached at iteration 3!
Total time: 317719 us

Loading problem 110-43
Solving problem 110-43
-------- Iteration 0: --------
Derived 6 new predicates from DD and 13 new predicates from AR.
-------- Iteration 1: --------
Derived 4 new predicates from DD and 0 new predicates from AR.
SOLVED!! Conclusion reached at iteration 1!
Total time: 56697 us

Loading problem 110-44
Solving problem 110-44
-------- Iteration 0: --------
Derived 12 new predicates from DD and 12 new predicates from AR.
-------- Iteration 1: --------
Derived 1 new predicates from DD and 0 new predicates from AR.
-------- Iteration 2: --------
Derived 10 new predicates from DD and 1 new predicates from AR.
SOLVED!! Conclusion reached at iteration 2!
Total time: 33138 us

Loading problem 110-45
Solving problem 110-45
-------- Iteration 0: --------
Derived 22 new predicates from DD and 12 new predicates from AR.
-------- Iteration 1: --------
Derived 5 new predicates from DD and 21 new predicates from AR.
-------- Iteration 2: --------
Derived 2 new predicates from DD and 8 new predicates from AR.
SOLVED!! Conclusion reached at iteration 2!
Total time: 209719 us

Loading problem 110-46
Solving problem 110-46
-------- Iteration 0: --------
Derived 40 new predicates from DD and 44 new predicates from AR.
-------- Iteration 1: --------
Derived 4 new predicates from DD and 0 new predicates from AR.
-------- Iteration 2: --------
Derived 38 new predicates from DD and 64 new predicates from AR.
SOLVED!! Conclusion reached at iteration 2!
Total time: 172884 us

Loading problem 110-47
Solving problem 110-47
-------- Iteration 0: --------
Derived 12 new predicates from DD and 52 new predicates from AR.
-------- Iteration 1: --------
Derived 1 new predicates from DD and 0 new predicates from AR.
-------- Iteration 2: --------
Derived 8 new predicates from DD and 1 new predicates from AR.
-------- Iteration 3: --------
Derived 2 new predicates from DD and 0 new predicates from AR.
SOLVED!! Conclusion reached at iteration 3!
Total time: 102524 us

Loading problem 110-48
Solving problem 110-48
-------- Iteration 0: --------
Derived 19 new predicates from DD and 1 new predicates from AR.
SOLVED!! Conclusion reached at iteration 0!
Total time: 4938 us

Solved 48 out of 48 problems.
```