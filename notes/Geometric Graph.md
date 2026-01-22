# A note on node incidence detection

Our approach to theorem proving is "conservative": any result that has not been shown to be true, cannot be taken to be true. It is perfectly valid under this approach to conclude that some lines, circles, or segments are incident as a result of establishing that their constituent points are incident. However, the reverse direction does not hold.

Consider a situation where the points $A, B, C$ and lines $l_1, l_2, l_3$ have been defined so that $l_1$ contains $A$ and $B$, $l_2$ contains $A$ and $C$, and $l_3$ contains $B$ and $C$. Suppose that, as a result of some deduction, we establish that $l_1$ and $l_2$ can be merged. What can we say about $l_3$?

There are two scenarios:
1. either $l_3$ is incident with the other two lines, and so all three lines are merged together;
2. or $A$ and $C$ are incident, and the two points are merged together.

We do not have any way to determine using the basic predicates alone which scenario we should invoke. We consider below a few solutions:

## Degeneracy predicates

The degeneracy predicate `diff` tells us when two points are distinct. If we knew that `diff A C`, then we would know that scenario 2 cannot possibly hold, and scenario 1 must be employed.

Similarly, the degeneracy predicate `npara` may be used to deduce that scenario 1 does not hold, and scenario 2 must be employed. These degeneracy predicates may be used to build up an "anti-graph" relating pairs of objects which are known NOT to be identical.

The principal difficulty with this solution is the issue of "distance between objects": due to the way our construction rules are structured, degeneracy predicates may only be specified for points in the same "layer" of the problem definition. Consider a question that defines a triangle $PQR,$ then some points $S, T, U$ based on the triangle, then $V, W$, then $X, Y, Z$, then $A$. Now, consider the situation but with the points $P, Q$ and $A$. Because the point $A$ was defined at the very back, or "as the very last layer", of the problem, it is not directly related to $P$ and $Q$ and so in all likelihood we do not have any predicates (much less degeneracy predicates) relating $A$ to $P$ and $Q$.

## Numerical verification

Maybe we could augment our lack of degeneracy predicates with numeric equality checks by our NumEngine? 

The biggest issue with this is obviously that it is unscientific. This is so important it deserves its own paragraph.

Notwithstanding that, it would be theoretically possible to build up our "anti-graph" from numerical checks on all pairs of elements. This would be an $O(n^2)$ process and not the best use of computer memory. Nonetheless, I believe it is worth exploring as an addition if the NumEngine is eventually integrated into an "interactive proof assistant" (capable of eliminating predicates which are numerically false).

## Deduction-specific node incidence

In our logical system, `coll A B C` makes the implicit assumption that all three points must be known to be distinct. This by itself does not align with our approach, so **we will need to explicitly enforce point-distinctness in our rule definitions by specifying degeneracy predicates**.

For example, in the rule

```
#para_same_line
A B C : para A B A C, diff B C => coll A B C
```

the `match_para()` function already ensures that `A != B` and `A != C`. Hence, it suffices for us to add the degeneracy predicate `diff B C`.

With this enforcement, we are able to hardcode the `make_coll()` function in `GeometricGraph` to employ Scenario 1, where $l_1, l_2, l_3$ are merged together. There is a design choice here: because the aforementioned assumption on `coll` is a feature of the `coll` predicate, not a ground truth of the `Line` class, so our implementation of Scenario 1 is done in the `GeometricGraph` rather than as a member function of `Line`.

As an example of the converse, consider a situation where $l_1$ also contained some $D$, and $l_2$ some $E$, and it has been shown that $D$ and $E$ are incident. The lines $l_1$ and $l_2$ may thus be merged, but not as a result of any collinear deduction. In this situation, Scenario 2 could still be possible.

## AlphaGeo's approach

We once again observe that, very annoyingly, AlphaGeometry skirts around this issue by performing rule-specific matching on the graph objects directly, so it has granularity at the rule level about which rules can tolerate incident points and which rules cannot do so. In fact, the only three rules involving `coll` which have matching functions implemented are:

```
circle O A B C, coll M B C, eqangle A B A C O B O M => midp M B C
eqratio6 d b d c a b a c, coll d b c, ncoll a b c => eqangle6 a b a d a d a c
eqangle6 a b a d a d a c, coll d b c, ncoll a b c => eqratio6 d b d c a b a c
```

all of which "by definition" will never successfully match against coincident points (for example, in the first rule, `B, C` cannot coincide as `eqangle A B A B ...` does not make sense; `M, B` cannot coincide as `eqangle ... O B O B` does not make sense; `M, C` also cannot coincide as $\angle BAC \neq \angle BOC$).

It is interesting to note that, of the remaining rules with matching functions, 15 of these have a `ncoll` tacked on at the end of the rule.