# Ordering Variables

## Prolog's unification algorithm must allow arbitrary variable ordering

Consider a framework where theorems are defined as Horne clauses, with predicates in each clause having arbitrary structure.

```
A B C X : coll A B C, eqangle A X B X B X C X => eqratio A X A B C X B C
```

In the above example rule, `(A, B, C)` are technically interchangeable, so one could imagine imposing a lexicographical ordering `A < B < C` on the appearance of these three points in the `coll` predicate, to reduce the number of matches that may occur. This is no trouble provided `coll` appears as the first predicate in the rule.

We could similarly define rules for `eqangle` and `eqratio`, which take four pairs of points, stating that the two points in every pair should be lexicographically ordered.

However, these conditions may cause matching to become unsuccessful. Consider a scenario where the above rule is matched against the following points in a geometry problem:

```
coll B C D, eqangle B A C A C A D A
```

It is clear that the lexicographical ordering for `eqangle` has been violated.

Consider another hypothetical example:

```
A B C D E F : cyclic A B C D, cyclic C D E F, coll A C E, coll B D F => para A B E F
```

matched against the following points:

```
cyclic E F A D, cyclic A D B C, coll E A B, coll F D C => para E F B C
```

It is clear that imposing a variable ordering on the predicates would make this outcome impossible to match.

In general,
- predicates appearing as the second (or later) rules, with as-yet-unmatched variables, may be subject to missing matches that do not satisfy the variable ordering invariant.
- some predicates, like `cyclic` and `coll`, which treat three or more points as "equivalence classes", might not be able to have variable orderings imposed by the matching engine in the first place.

## Arbitrary ordering results in an explosion of substitutions

One solution would be to remove the variable ordering optimisation entirely, and fall back on Prolog's arbitrary-ordering unification. However, this is not ideal:

Consider the predicate `eqangle A B C D E F G H`. We can swap the two letters in each pair `(A, B)`, `(C, D)`, `(E, F)`, and `(G, H)`; we can also swap the pairs `(A, B), (E, F)` with the pairs `(C, D), (G, H)`. These five swaps give a total of $2^5 = 32$ combinations of predicates representing the exact same relationship.

Now, suppose the lines `l1, l2, l3, l4` each have 3 points on them. We want to represent the predicate `eqangle l1 l2 l3 l4`. We have $3^4 = 81$ choices of pairs of points to use, and for each pair, we have $32$ different combinations, leading to a total of $81 \times 32 = 2592$ combinations representing the exact same relationship.

A $10^3$ reduction in runtime, and a $10^3$-fold increase in memory usage (if we were to store every predicate), are both completely unacceptable.

## Dynamic matching algorithm based on number of remaining un-matched variables

Once again, consider our above example, at the instance when the first predicate has been matched but not the second:

```
coll B C D, eqangle B ?1 C ?1 C ?1 D ?1
```

If our matching engine can detect that 1 out of 2 variables in each of the 4 pairs have been matched, then it can proceed to perform matching for the other 4 variables in a smarter way, and also ignoring the initial ordering condition.

In general, **it would be wise, in any rule, for all predicates from the second one onward to completely disregard the variable ordering rule, except in specific cases and only if none of the variables have been matched thus far.**

The one big downside is that this is tedious to implement. But we do it anyway! :)

## Variable orderings hard-coded in rule template

Consider the following rule:

```
A B C D E F G H I J K L : eqangle A B C D E F G H, eqangle E F G H I J K L => eqangle A B C D I J K L
```

At the instance when the first predicate has been matched but not the second:

```
eqangle A B C D E F G H, eqangle E F G H ?1 ?2 ?3 ?4
```

A reasonable implementation of our dynamic matching algorithm might split `eqangle` into its 4 pairs, and match based on each individual pair. The first two pairs have been completely matched, while the last two pairs are completely empty.

We ask ourselves a question: Will there ever be a scenario where the variable ordering invariant `?1 < ?2`, `?3 < ?4` will have to be violated? The answer is no. Looking at the rule definition, we see that `I < J` and `K < L` have already been encoded in the variable templates, and are respected throughout the rest of the predicate templates. Hence, we may conclude that **for** `eqangle`, **it suffices for the matching engine to follow the same variable-ordering invariants respected by the rule template.**

## Ordering invariants in conclusion predicates

Consider the following rule, which utilises the implicit ordering invariant `A < B < P < Q`:

```
A B P Q : cyclic A B P Q => eqangle A P B P A Q B Q
```

This is clearly not ideal since there are five other pairs of equal angles in the cyclic quadrilateral. The whole point of using variable orderings in the first place was to avoid scenarios where we would have to list all six pairs.

For this particular rule, we would either have to use rule-based matching, or ignore the variable orderings for the `cyclic` predicate.

## Solution

- Predicates involving pairs of points representing lines, such as `eqangle` and `para`, should impose a variable ordering on these pairs of points, so long as neither point in the pair has been matched.

- Predicates involving triples or larger $n$-tuples of points, i.e. `coll`, `cyclic` and `circle`, should do away with variable orderings.

- Predicates involving pairs of other objects, such as pairs of angles in `eqangle` and pairs of ratios in `eqratio`, should also not be ordered.

What are the effects of the second point on the following rule?

```
A B P Q : cyclic A B P Q => eqangle A P B P A Q B Q
```

For every cyclic quadruplet `(A, B, C, D)`, 24 matches will occur. For every equal angle `eqangle A C B C A D B D`, 4 matches will occur, with each match coming from an ordered quadruplet formed by swapping the points within each pair `(A, B)` and `(C, D)`. We will thus get 4 duplicates of the same `eqangle`:

```
eqangle A C B C A D B D
eqangle B C A C B D A D
eqangle A D B D A C B C
eqangle B D A D B C A C
```

# Rule-based Matching

AlphaGeometry skirts all the issues brought up previously by skipping the matching step entirely. Instead, it programmatically generates the correct variable matches based on the rule supplied. This also means that each rule's matching algorithm is hard-coded into the solver: rules cannot be modified nor added by a third-party user.

Indeed, rule-based matching avoids all the issues brought about by arbitrary variable ordering.

As an extension, we will attempt to implement this for comparison.