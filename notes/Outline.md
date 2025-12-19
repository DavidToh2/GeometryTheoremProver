### Predicate Names

| Geometric Node Type |  | All nodes of the same type connect to each other to form a DSU if they are identical. |
|---|---|---|
| `Point` Nodes | `Point` | Every `Point` has `on_line`, `on_circle`, `on_triangle` and `on_quadrilateral` attributes. |
| `Object` Nodes | `Line`, `Circle`<br>`Segment`<br>`Triangle`, `Quadrilateral` | Every `Line`, `Circle`, `Triangle` and `Quadrilateral` contains some `points`.<br>Every `Segment` has two `points`.<br>Note: `Triangle` is a special case, congruent triangles form a DSU.<br>Note: We will leave `Triangle` and `Quadrilateral` to last as they are harder to deal with. |
| `Value` Nodes | `Direction`, `Length`, `Shape` | Every `Value` connects in a two-way fashion to all the `Object`s for which they are relevant.<br>A `Direction` connects to a collection of `lines` which are parallel.<br>Additionally, it also has a `perp` attribute.<br>A `Length` connects to a collection of `segments`.<br>A `Shape` connects to a collection of `triangles` which are similar. |
| `Object2` Nodes | `Angle`, `Ratio` | Every `Object2` connects in a two-way fashion to the `Value` nodes for which they are relevant.<br>Every `Angle` has a `direction1` and a `direction2`.<br>Every `Ratio` has a `Length1` and a `Length2`. |
| `Value2` Nodes | `Measure`, `Fraction` | Every `Value2` connects in a two-way fashion to all the `Object2`s for which they are relevant.<br>Every `Measure` connects to a collection of `angles`.<br>Every `Fraction` connects to a collection of `ratios`. |

Associations between `Value`s and `Object2`s, or between themselves, shall always store root nodes.

Associations between `Object(2)s` and `Value(2)s` need not store root nodes.

| Declarations |  |  |
|---|---|---|
| Predicate | `coll x1 x2 ...`<br>`cyclic x1 x2 ...` | An arbitrary-length list of collinear or concyclic points |
|  | `para x1 x2 x3 x4`<br>`perp x1 x2 x3 x4` | `x1x2` is either parallel or perpendicular to `x3x4` |
|  | `cong x1 x2 x3 x4`<br>`eqangle x1 x2 x3 y1 y2 y3`<br>`eqratio x1 x2 x3 x4 y1 y2 y3 y4` | Equality predicates for the segments or angles formed by the points |
|  | `contri a1 b1 c1 a2 b2 c2`<br>`simtri a1 b1 c1 a2 b2 c2` | Triangle congruency and similarity predicates |
|  | `midp m x1 x2`<br>`circle o x1 x2 x3` | Midpoint and circle center respectively |
|  | `constangle n x1 x2 x3`<br>`constratio n x1 x2 x3 x4` | Constant-equality predicates for the segments or angles formed by the points |
| Degenerate Predicates | `diff p q`<br>`ncoll x y z`<br>`convex a b c d` | Points `p`, `q` are not equal<br>The points `x, y, z` are not collinear<br>`abcd` is a convex quadrilateral |
| Predicate2 | `center x a b c`<br>`incenter x a b c`<br>`excenter x a b c`<br>`orthocenter x a b c`<br>`centroid x a b c` | `x` is the circumcenter / incenter of `abc`<br>`x` is the `a`-excenter of `abc` (!!!) <br> `x` is the orthocenter / centroid of `abc` |
| Note: Predicate2's double up as constructions.<br><br>Predicate2's map directly to a list of Predicates that <br>the new points must satisfy. | `orthocenter2 x d e f a b c`<br>`centroid2 x d e f a b c`<br>`incenter2 x d e f a b c` | The `2` versions of each predicate add in the inner triangles `def` as well, with `d` opposite `a`, etc.|
|  | `reflect p y x1 x2` | `p` is the reflection of `y` across the line `x1x2` |
|  | `inbisect x a b c`<br>`exbisect x a b c` | `x` lies on the internal angle bisector of angle `abc`<br>`x` lies on the external angle bisector of angle `abc` |
|  | `tangent p o x`<br>`ptangent p o1 x1 o2 x2` | `p` lies on the tangent to circle centered at `o` at `x`<br>`p` is the common tangency point of two circles centered at `o1` and `o2` respectively |
|  | `equilateral a b c`<br>`risos a b c`<br>`square a b c d`<br>`parallelogram a b c d` | `abc` is an equilateral / right-angled isosceles triangle<br>`abcd` is a square / parallelogram |
|  | `congtri a b c p q r`<br>`simtri a b c p q r` | Triangles `abc` and `pqr` are congruent / similar, with vertices oriented in this order |

### Syntax

| Logical syntax | | |
|---|---|---|
| Predicates | Every predicate is a truth statement regarding a set of points. | `pred p1 p2 ...` |
| Constructions | Every construction introduces a set of new points and depends on a set of old points. | `parallelogram x : a b c` |
| | The specification of a construction in `constructions.txt` shall include the preconditions<br>necessary to perform the construction, as well as postconditions involving the new points.<br> The preconditions shall be stored as a clause.<br>The postconditions shall be stored as a list of predicates. | `ncoll a b c`<br>`para a b c x, para a x b c, cong a b c x, cong a x b c` |
| Problem Definitions | A problem definition is a  |  |


