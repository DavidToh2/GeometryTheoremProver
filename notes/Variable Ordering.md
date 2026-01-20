# Ordering Variables

## Prolog's unification algorithm must allow arbitrary variable ordering

Consider a framework where theorems are defined as Horne clauses, with predicates in each clause having arbitrary structure.

```
A B C X : coll A B C, eqangle A X B X B X C X => eqratio A X A B C X B C
```

In the above example rule, `(A, B, C)` are technically interchangeable, so one could imagine imposing a lexicographical ordering `A < B < C` on the appearance of these three points in the `coll` predicate, to reduce the number of matches that may occur. This is no trouble provided `coll` appears as the first predicate in the rule.

We could similarly define rules for `eqangle` and `eqratio`, which take four pairs of points, stating that the two points in every pair should be lexicographically ordered.

However, these conditions may cause matching to become unsuccessful. 

1. Consider **Scenario 1** where the above rule is matched against the following points in a geometry problem:

```
coll B C D, eqangle B A C A C A D A
```

It is clear that the lexicographical ordering for `eqangle` has been violated.

2. Consider another hypothetical **Scenario 2**:

```
A B C D E F : cyclic A B C D, cyclic C D E F, coll A C E, coll B D F => para A B E F
```

matched against the following points:

```
cyclic E F A D, cyclic A D B C, coll E A B, coll F D C => para E F B C
```

It is clear that imposing a variable ordering on the predicates would make this outcome impossible to match.

3. Another **Scenario 3** where a match is impossible: the following rule

```
A B C X Y Z : cong A B X Y, cong A C X Z, eqangle A B A C X Y X Z => contri A B C X Y Z
```

will never match against $XZ = AC, YZ = BC$ (with the angle between them being equal), since the only ways to do so are by violating the variable ordering from the very beginning:

```
cong C A Z X, cong C B Z Y, eqangle C B C A Z X Z Y => ...
```

4. For **Scenario 4**, consider the following rule template:

```
A B C D : eqangle A B A C A C A D, cong B C C D => cyclic A B C D
```

If it just so happens that $\angle ADB = \angle BDC$ and $AB = BC$, then $D$ has to be matched to `A` and $A, B, C$ to `B, C, D`, which clearly violates the variable ordering.

Below we list some of the solutions we considered, as well as brief explanations of why they do not work:

## Dynamic matching algorithm based on number of remaining un-matched variables

Consider Scenario 1 at the instance when the first predicate has been matched but not the second:

```
coll B C D, eqangle B ?1 C ?1 C ?1 D ?1
```

If our matching engine can detect that 1 out of 2 variables in each of the 4 pairs have been matched, then it can proceed to perform matching for the other 4 variables in a smarter way by ignoring the ordering condition.

Maybe we could say that, for all predicates from the second one onward to completely disregard the variable ordering rule, except in specific cases and only if none of the variables have been matched thus far.

However, this does not resolve the issues in Scenarios 2 and 3, where the very first predicate to be matched already violates the internal variable ordering.

## Restricting variable orderings to pairs of points


Consider the following rule template, where the pairwise orderings `A < B, C < D, ..., K < L` have been hard-coded:

```
A B C D E F G H I J K L : eqangle A B C D E F G H, eqangle E F G H I J K L => eqangle A B C D I J K L
```

For this particular rule, swapping the two variables in each ordering do not fundamentally change the semantics of the rule. Moreover, the pairwise orderings are respected throughout the entire template. This suggests the idea of restricting variable orderings to pairs of variables represent lines or segments.

However, this is obviously not true in general: Scenarios 3 and 4 both violate the pairwise ordering from the very start.

## Ordering invariants in conclusion predicates

Consider the following rule, which utilises the implicit ordering invariant `A < B < P < Q`:

```
A B P Q : cyclic A B P Q => eqangle A P B P A Q B Q
```

This is clearly not ideal since there are five other pairs of equal angles in the cyclic quadrilateral. The whole point of using variable orderings in the first place was to avoid scenarios where we would have to list all six pairs.

For this particular rule, we would either have to use rule-based matching, or ignore the variable orderings for the `cyclic` predicate.

## Arbitrary ordering results in an explosion of substitutions

We could remove the variable ordering optimisation entirely, and fall back on Prolog's arbitrary-ordering unification. However, this is not ideal:

Consider the predicate `eqangle A B C D E F G H`. We can swap the two letters in each pair `(A, B)`, `(C, D)`, `(E, F)`, and `(G, H)`; we can also swap the pairs `(A, B), (E, F)` with the pairs `(C, D), (G, H)`. These five swaps give a total of $2^5 = 32$ combinations of predicates representing the exact same relationship.

Now, suppose the lines `l1, l2, l3, l4` each have 3 points on them. We want to represent the predicate `eqangle l1 l2 l3 l4`. We have $3^4 = 81$ choices of pairs of points to use, and for each pair, we have $32$ different combinations, leading to a total of $81 \times 32 = 2592$ combinations representing the exact same relationship.

A $10^3$ reduction in runtime, and a $10^3$-fold increase in memory usage (if we were to store every predicate), are both completely unacceptable.

## Solution

It is clear that we cannot avoid the increase in runtime at the matching stage. However, what we can do is to avoid repeat matches of the same predicate across multiple matching stages, as well as storing excessive predicates.

# Rule-based Matching

AlphaGeometry skirts all the issues brought up previously by skipping the matching step entirely. Instead, it programmatically generates the correct variable matches based on the rule supplied. This also means that each rule's matching algorithm is hard-coded into the solver: rules cannot be modified nor added by a third-party user.

Indeed, rule-based matching avoids all the issues brought about by arbitrary variable ordering.

As an extension, we will attempt to implement this for comparison.