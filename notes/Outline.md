## Geometric Object Classes

| Geometric Node Type |  | All nodes of the same type connect to each other to form a DSU if they are identical. |
|---|---|---|
| `Point` Nodes | `Point` | Every `Point` has `on_line`, `on_circle`, and `on_segment` attributes. |
| `Object` Nodes | `Line`, `Circle`<br>`Segment` | Every `Line` and `Circle` contains some `points`.<br>Every `Segment` has two `endpoints`. |
| `Value` Nodes | `Direction`, `Length` | Every `Value` connects in a two-way fashion to all the `Object`s for which they are relevant.<br>A `Direction` connects to a collection of `lines` which are parallel.<br>Additionally, it also has a `perp` attribute.<br>A `Length` connects to a collection of `segments`. |
| `Object2` Nodes | `Angle`, `Ratio` | Every `Object2` connects in a two-way fashion to the `Value` nodes for which they are relevant.<br>Every `Angle` has a `direction1` and a `direction2`.<br>Every `Ratio` has a `Length1` and a `Length2`. |
| `Value2` Nodes | `Measure`, `Fraction` | Every `Value2` connects in a two-way fashion to all the `Object2`s for which they are relevant.<br>Every `Measure` connects to a collection of `angles`.<br>Every `Fraction` connects to a collection of `ratios`. |

Associations between `Value`s and `Object2`s, or between themselves, shall always store root nodes.

Associations between `Object(2)s` and `Value(2)s` need not store root nodes. (However, `Value(2)s` will always contain a `root_obj(2)s` set that must store root nodes.)

Note that our current implementation of `Triangle`, `Dimension` and `Shape` only serves to record congruent and similar triangles derived by deduction rules, both to deduplicate deductions, as well as to perform traceback in the near future. (In other words, merging two `Dimension`s, for example, does not merge the corresponding segment `Length`s.)

| Geometric Node Type |  | All nodes of the same type connect to each other to form a DSU if they are identical. |
|---|---|---|
| `Object` nodes | `Triangle` | Every `Triangle` has three `Points` and three `Segments`. These points and segments are stored in the order <br>`points = [A, B, C]`<br>`segments = [BC, CA, AB]`<br>In other words, `points[i]` will be an endpoint of `segments[i+1]` and `segments[i+2]`, and `segments[0]` stores the segment `points[1]points[2]`, where indices are taken mod 3. These must always be root nodes. <br>A permutation of vertex indices is given in the array `perm`, and indicates the order in which the triangle's `points` and `segments` should be read. For example, the array `perm = [1, 0, 2]` might be accompanied by `points = [B, A, C], segments = [AC, CB, BA]` to indicate triangle `ABC`. This is used for the mappings described below. |
| `Object2` nodes | `Dimension` | A `Dimension` object connects in a two-way fashion to all `Triangle`s which are congruent.<br>The `Dimension` object also has a list `lengths` corresponding to the lengths of segments `[BC, CA, AB]`. These must always be root nodes. <br>For any given `Triangle` associated with this `Dimension`, its segments may be mapped to the correct lengths as such:<br>`lengths[i] -> segments[perm[i]]` <br>The `Dimension` object additionally contains an `isosceles` array which stores "swappable" vertices. For instance, if `ABC` is isosceles with `AC = BC`, then `isosceles = [1, 1, 0]` since `A, B` are swappable. |
| `Object3` nodes | `Shape` | A `Shape` object connects in a two-way fashion to all `Dimension`s which are similar.<br>The `Shape` object also has a list `measures` corresponding to the measures of the angles at vertices `[A, B, C]`. These must always be root nodes. <br>For any given `Triangle` associated with this `Shape`, its vertices may be mapped to the correct measures as such:<br>`measures[i] -> points[perm[i]]` |

Note: Every triangle is initialised together with a `Dimension` and a `Shape`.

Every `Measure` merger would need to check for newly incident `Shape`s. Conversely, every `Shape` merger would need to merge the associated `Measure`s. (We would also need to re-map every `Triangle`'s `perm`.)

Every `Length` merger would need to check for newly incident `Dimension`s. Conversely, every `Dimension` merger would need to merge the associated `Length`s, as well as their `Shape`s. (We would also need to re-map every `Triangle`'s `perm`.')

**Question:** What exactly is the benefit of defining these classes? The main one I can think of is the ability to hardcode triangles' "special points" into the graph. This would allow for more complex rules, like

`A B C O H : circumcenter O A B C, orthocenter H A B C => eqangle A B B O H B B C`

as well as the identification of corresponding points, as in

`A B C D E F X Y : simtri A B C D E F, corr X (A B C) Y (D E F) => simtri A B X D E Y`

## Predicates

| Declarations |  |  |
|---|---|---|
| Predicate | `coll x1 x2 ...`<br>`cyclic x1 x2 ...` | An arbitrary-length list of collinear or concyclic points |
|  | `para x1 x2 x3 x4`<br>`perp x1 x2 x3 x4` | `x1x2` is either parallel or perpendicular to `x3x4` |
|  | `cong x1 x2 x3 x4`<br>`eqangle x1 x2 x3 y1 y2 y3`<br>`eqratio x1 x2 x3 x4 y1 y2 y3 y4` | Equality predicates for the segments or angles formed by the points |
|  | `contri a1 b1 c1 a2 b2 c2`<br>`simtri a1 b1 c1 a2 b2 c2` | Triangle congruency and similarity predicates |
|  | `midp m x1 x2`<br>`circle o x1 x2 x3` | Midpoint and circle center respectively |
|  | `constangle n x1 x2 x3`<br>`constratio n x1 x2 x3 x4` | Constant-equality predicates for the segments or angles formed by the points |
| Degenerate Predicates | `diff p q ...`<br>`ncoll x y z ...`<br>`npara p q r s`<br>`sameclock/diffclock a b c p q r`<br>`convex a b c d` | Points `p`, `q` are not equal<br>The points `x, y, z` are not collinear<br>`pq` and `rs` are not parallel<br>`abc` and `pqr` are either both clockwise or both counterclockwise; and the converse<br>`abcd` is a convex quadrilateral |

Note: The matching rules for `eqangle`, `perp` and `para`, as well as those of `eqratio` and `cong`, will not interfere with one another. The rule for `eqangle`, for instance, iterates over `Measure`s, while that of `para` iterates over `Direction`s. 

There is room for expanding the list of available predicates, in order to encode more complex rules.

| Extension Declarations |  |  |
|---|---|---|
| Predicate2 | `incenter x a b c`<br>`excenter x a b c`<br>`orthocenter x a b c`<br>`centroid x a b c` | `x` is the incenter of `abc`<br>`x` is the `a`-excenter of `abc` (!!!) <br> `x` is the orthocenter / centroid of `abc` |
|  | `orthocenter2 x d e f a b c`<br>`centroid2 x d e f a b c`<br>`incenter2 x d e f a b c` | The `2` versions of each predicate add in the inner triangles `def` as well, with `d` opposite `a`, etc.|
|  | `reflect p y x1 x2` | `p` is the reflection of `y` across the line `x1x2` |
|  | `inbisect x a b c`<br>`exbisect x a b c` | `x` lies on the internal angle bisector of angle `abc`<br>`x` lies on the external angle bisector of angle `abc` |
|  | `tangent p o x`<br>`ptangent p o1 x1 o2 x2` | `p` lies on the tangent to circle centered at `o` at `x`<br>`p` is the common tangency point of two circles centered at `o1` and `o2` respectively |
|  | `equilateral a b c`<br>`r_triangle_p a b c`<br>`square a b c d`<br>`parallelogram a b c d` | `abc` is an equilateral / right-angled isosceles triangle<br>`abcd` is a square / parallelogram |

The main benefit of these extension predicates would be the declaration of more complex rules, as mentioned above. We pass on these for now.

Of note is that many of these extension predicates are now constructions.

## Rule and Construction Syntax

| Logical syntax | | |
|---|---|---|
| Predicates | Every predicate is a truth statement regarding a set of points. | `pred p1 p2 ...` |
| Rules | Every rule is formed of a list of predicates (its body), then an arrow `=>`, then the goal. <br>The rule is prefaced by the list of points it involves.<br>All degenerate predicates must appear at the back of the rule. | `A B C O : cong O A O B, cong O B O C, diff O A B C => circle O A B C` |
| Constructions | Every construction introduces a set of new points and depends on a set of old points. These are separated by a colon `:`. | `parallelogram x : a b c` |
| | The specification of a construction in `constructions.txt` shall include the preconditions necessary to perform the construction, as well as postconditions involving the new points.<br> The preconditions shall be stored as a clause.<br>The postconditions shall be stored as a list of predicates. | `- ncoll a b c`<br>`- para a b c x, para a x b c, cong a b c x, cong a x b c` |
| | The specification must then be followed by a list of numeric steps needed to establish the set of new points. | `- x = line_para a b c, line_para c a b` |
| Problem Definitions | A problem is indicated by the word `problem` followed by its name. It is defined by a series of **construction stages** enclosed in curly brackets. | `problem P1 {` |
| | A **construction stage** introduces a set of new points along with their relevant constructions. New points are listed, before a `=`.<br>A comma-separated list of constructions follow. Every construction takes the same form as its definition - its name, then the new points involved, then a `:`, then the old points it relies on.<br>Stages are separated with a `;`.<br>Indentation shown on the right is optional. | `p1 p2 ... pk = `<br>`construct1 p1 : q1 q2 ...,`<br>`construct2 p2 p3 : q1 ...,`<br>`...;` | 
| | The goal of the problem is indicated by a `?` followed by a singular predicate. | `... ? pred p q r s`<br>`}` | 


