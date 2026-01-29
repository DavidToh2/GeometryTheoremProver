# The numeric vs logical interplay

Our logical system is "fully conservative":
- Any result that has not been shown to be true, cannot be taken to be true in general.

On the other hand, our numeric verification system is "false-optimistic":
- Any result that is numerically true in one instance, cannot be taken to be true in general.
- Any result that is numerically false in one instance, can be taken to be false in general.

The "false-optimism" makes it logically sound for us to implement our NumEngine in a way that only allows numerically valid constructions to be accepted.

## Initialising degenerate objects

A fundamental flaw of the aforementioned system is that it is unable to initialise numerically degenerate objects. This is because our NumEngine will throw errors on detection of numerically degenerate objects (such as the circumcircle of three collinear lines). 

When our GeometricGraph creates new objects,
- the object is first created with `std::make_unique<>()`, which invokes its constructor and populates the object's own `points` attribute;
- the object's numerics are then generated with `compute_object_from_points()`, which also performs an implicit numeric check;
- if the numeric check succeeds, then the GeometricGraph's `root_objects` field, as well as the points' `on_object` and `on_root_object` field, are populated by `Point::set_this_on()`;
- if the numeric check fails, then a `NumericsInternalError` is thrown before the aforementioned fields are populated, so that the state of the valid objects in the program are not polluted by this invalid object.

# Node incidence detection in predicates

It is perfectly valid under our logical system to conclude that some lines, circles, or segments are incident as a result of establishing that their constituent points are incident. However, the reverse direction does not hold.

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

# Secondary node incidence detection

Let's take the situation from the previous section one step further. We will maintain an invariant that any "resting state" of our geometric graph must have the property that no pair of numerically distinct points simultaneously lie on two root lines (which need not be numerically distinct).

Consider a situation where the points $A, B, C, D$ and root lines $l_1, l_2, l_3$ have been defined so that $l_1 = \{A, B, D\}$, $l_2 = \{B, C\}$, $l_3 = \{C, D\}$. Now, suppose `coll A B C` is called, and $l_1, l_2$ are merged. What should happen to $l_3$?

There are two scenarios:
1. either $l_3$ is incident with the other two lines, and so all three lines are merged together;
2. or $C$ and $D$ are incident, and the two points are merged together.

From our previous discussion on degenerate objects, it might be reasonable to assume that the line $l_3$ could not have been initialised at all if $C$ and $D$ were indeed numerically incident: hence Scenario 1 must apply. However, we may have a situation where $l_3$ was defined with the help of some other point $Z$, not numerically incident to $C$ and $D$, as in

$$ l_1 = \{A, B, D, X\}, \quad l_2 = \{B, C, Y\}, \quad l_3 = \{C, D, Z\} $$

We could also consider looking at the numerical gradients of the lines: if $l_1$ and $l_3$ do not have the same gradient, then we know Scenario 2 must apply. 

We can use the above ideas to design a detection algorithm. Before merging $l_1$ and $l_2$,
- identify all lines $l$ containing common points with both $l_1$ and $l_2$. Call these sets of pointns $S_1$ and $S_2$;
- by the invariant, all points in $S_1$ must be numerically equivalent, and the same goes for all points in $S_2$;
- if the numeric gradient of $l$ differs from that of $l_1$, then we may now deduce that $S_1$ and $S_2$ are themselves numerically equivalent to each other, and so the points in $S_1$ and in $S_2$ may be merged;
- otherwise, if $S_1$ and $S_2$ are numerically distinct, we can deduce that $l$ must be coincident with both $l_1$ and $l_2$;
- otherwise, do nothing with $l$. 

The invariant is maintained since the fourth bullet point is the only situation where we deal with numerically distinct points.

## AlphaGeo's approach

AlphaGeo defines an `add_coll(og_points : list[Point])` function that can take an arbitrary number of points, and does the following:

1. Get all lines* (lets call this `L0`) that pairs of supplied points `P0 = og_points` are known to lie on, then get all points `P1` on all of `L0`, then get all lines* `L1` passing through pairs of points in `P1`
   - Observe that `L1` corresponds to the set of lines $l$ in our algorithm (plus $l_1$ and $l_2$ themselves), and `P1` corresponds to the union of all $S_1$ and $S_2$'s taken over all $l$

2. Sort the lines in `L1` by name. Pick the lexicographically smallest line `line0` that already exists as the representative that we will merge all other lines with.

3. For each line `line` in `L1`, 

    1. pick out the representative points `a, b` of `line0` and `c, d` of `line`;
  
    2. Figure out the reason why each of the representative points is collinear with `og_points` using `self.coll_dep()`;

    3. For each of these representative points, create a new Dependency stating that they are collinear with `og_points`, and the `.why` being the reason(s) figured out above, then add it to the array `add`

4. Return the array `add`, which now contains the list of new `Dependency`s that have been added, along with their `.why`'s.

(Also invoked by `self.add_midp()` and `self._add_para_or_coll()`.)

*The line fetching is done via `get_line_thru_pair(p1, p2)`, which creates a new line if none exists. In other words, $O(n^2)$ lines will be fetched/created for every invocation with $n$ points.

## Third-order hypothetical scenario?

Define

$$ l_1 = \{A, B, D, Y\}, \quad l_2 = \{B, C\}, \quad l_3 = \{C, D, X\}, \quad l_4 = \{X, Y\} $$

AlphaGeo's algorithm, when invoking `add_coll(A, B, C)`, will populate:
- `L0 = l1, l2`
- `P1 = A, B, C, D, Y`
- `L1 = l1, l2, l3`

This is evidently not enough as, after merging all lines in `L1`, we now have some $l = \{A, B, C, D, X, Y\}$ and have to deal with the question of whether $l_4$ should be merged in. It would be nice to continue defining
- `P2 = A, B, C, D, X, Y`
- `L2 = l1, l2, l3, l4`

until we reach the reflexive-transitive closure `L2`. This suggests that any possible algorithm must be *recursive*.

## Proposed Detection Algorithm

`def merge_lines(l1 : Line, l2 : Line)`:

- Initialise the set of lines to merge $L_m = \{l_1\}$, the sets of pending lines to analyse $L_1 = \{l_2\}, L_0 = \{\}$, and the list of sets of points to merge $P_m = \{\}$; 
- While $L_1$ is nonempty:
  - For every pair of lines $(l_1, l_2) \in L_m \times L_1$:
    - Identify all lines $l$ having disjoint sets $S_1, S_2$ of points in common with both $l_1$ and $l_2$ respectively
    - (by the invariant, all points in $S_1$ must be numerically equivalent, and the same goes for all points in $S_2$)
    - if the numeric gradient of $l$ differs from that of $l_1$, then we may now deduce that $S_1$ and $S_2$ are themselves numerically equivalent to each other, and so:
      - add $S_1 \cup S_2$ to $P_m$;
    - otherwise, if $S_1$ and $S_2$ are numerically distinct, we can deduce that $l$ must be coincident with both $l_1$ and $l_2$, and so:
      - add $l$ to $L_0$;
    - otherwise, we can't deduce anything about $l$, so leave it alone.
  - Once all pairs of lines have been iterated through, $L_1$ can be merged into $L_m$, then set to equal $L_0$, and $L_0$ can be emptied.
- Arbitrarily select some $l_0 \in L_m$ as the "global root", and merge every single other line in $L_m$ into $l_0$;
- What do we do with all the sets of points in $P_m$ we now have to merge???????? **Let's ignore these for now.** Because having to deal with them would involve rewriting the `merge_points` function as well, which currently has its own general object incidence detection logic. 

## Potential (Global) Detection Algorithm

Define the **object incidence condition**, relative to a pair of points $(p_1, p_2)$, to be as follows:
- Two lines are newly incident if they have one point in common, one of them additionally contains $p_1$, and the other $p_2$;
- Two circles are newly incident if they have two points in common, one of them additionally contains $p_1$, and the other $p_2$...
- ...or if they have a common circle center, one of them additionally contains $p_1$, and the other $p_2$...
- ...or if they have a common point, one of them has $p_1$ as center, and the other has $p_2$;
- Two segments are newly incident if they have a common endpoint, one of them additionally contains $p_1$, and the other $p_2$.

Define a **prepared merger tree** $M$ as a directed tree, with edges pointing away from root and labelled with predicates. A pair $(u, v)$ of objects may be *added* to $M$ if $M$ already contains a node $u$ (or $v$), and the edge $u \rightarrow v$ (correspondingly $v \rightarrow u$) will be added. The tree may be iterated through, starting from its root, via DFS traversal.

The above may be implemented with the following class declaration:

```cpp
class MergerNode {
    Node* n;
    int checked = 0;       // for the preparation algorithm
    vector<pair<MergerNode*, Predicate*>> children;
}
class MergerTree {
    MergerNode* root;
    set<MergerNode*> unchecked;
}
```

Lookup can be reduced from $O(N)$ to $O(\log N)$ if we implement some form of sorting on `children`.

Maintain a list of **prepared merger trees** for each object type: $P_m = \{\}, L_m = \{\}, C_m = \{\}, S_m = \{\}$.

**Stage 1: Preparing the sets of objects to merge**

`def merge_points(p1, p2, pred)`:

Add the pair $(p_1, p_2)$ to the appropriate prepared merger tree in $P_m$ (making a brand new tree if need be), labelling the edge with `pred`, and setting the `checked` status appropriately.

In this tree:
- For every pair $(p_1, p_2)$ of (checked, unchecked) points respectively (unless the tree is new, in which case just pick the two points):
  - Add all pairs of lines $(l_1, l_2)$ satisfying the object incidence condition to $L_m$, labelling the edge with `pred`;
  - Do the same for circles $(c_1, c_2)$ to $C_m$;
  - and segments $(s_1, s_2)$ to $S_m$.

`def merge_lines(l1, l2, pred)`:

Add $(l_1, l_2)$ to some tree in $L_m$. In this tree $M$:
- For every pair $(l_1, l_2)$ of (checked, unchecked) lines respectively:
  - Identify all lines $l$ having disjoint sets $S_1, S_2$ of points in common with both $l_1$ and $l_2$ respectively
  - (by the invariant, all points in $S_1$ must be numerically equivalent, and the same goes for all points in $S_2$)
  - if the numeric gradient of $l$ differs from that of $l_1$, then we may now deduce that $S_1$ and $S_2$ are themselves numerically equivalent to each other, and so:
    - add $S_1 \cup S_2$ to an appropriate tree in $P_m$ - some trees may need to be merged at this state;
  - otherwise, if $S_1$ and $S_2$ are numerically distinct, we can deduce that $l$ must be coincident with both $l_1$ and $l_2$, and so:
    - add $l$ to $M$;
  - otherwise, we can't deduce anything about $l$, so leave it alone.

Note: At this stage, do not remove any objects from the `root_` maps.
  
**Stage 2: Performing the merge**

For every tree $M$ in every $(P, L, C, S)_m$:
- Let $r$ be the root of $M$
- Perform depth-first traversal to merge every other element into $r$, using the predicate labelling their incoming edge as the reason
- Also remove these elements from the `root_` maps



