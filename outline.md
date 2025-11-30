### Predicate Names

| Geometric Node Type |  | All nodes of the same type connect to each other to form a DSU if they are identical. |
|---|---|---|
| `Object` Nodes | `Point`, `Line`, `Circle`<br>`Triangle`, `Quadrilateral` | Every `Point` has `on_line`, `on_circle`, `on_triangle` and `on_quadrilateral` attributes.<br>Every `Line`, `Circle`, `Triangle` and `Quadrilateral` contains some `points`. |
| `Object2` Nodes | `Angle`, `Segment`, `Ratio` | Every `Object2` connects in a two-way fashion to the `Object` nodes for which they are relevant.<br>Every `Angle` has a `line1` and a `line2`.<br>Every `Segment` has two `points`.<br>Every `Ratio` has two `segments`. |
| `Value` Nodes | `Direction`, `Cong`, `Sim` | Every `Value` connects in a two-way fashion to all the `Object`s for which they are relevant.<br>A `Direction` connects to a collection of `lines` which are parallel.<br>Additionally, it also has a `perp` attribute.<br>A `Cong` connects to a collection of `triangles` which are congruent.<br>A `Sim` connects to a collection of `triangles` which are similar.<br>Additionally, it is also connected to the relevant `congs`. |
| `Value2` Nodes | `Measure`, `Length`, `Fraction` | Every `Value2` connects in a two-way fashion to all the `Object2`s for which they are relevant.<br>Every `Measure` connects to a collection of `angles`.<br>Every `Length` connects to a collection of `segments`.<br>Every `Fraction` connects to a collection of `ratios`. |

| Declarations |  |  |
|---|---|---|
| Predicate | `coll x1 x2 ...`<br>`cyclic x1 x2 ...` | An arbitrary-length list of collinear or concyclic points |
|  | `para x1 x2 x3 x4`<br>`perp x1 x2 x3 x4` | `x1x2` is either parallel or perpendicular to `x3x4` |
|  | `cong x1 x2 x3 x4`<br>`eqangle x1 x2 x3 y1 y2 y3`<br>`eqratio x1 x2 x3 x4 y1 y2 y3 y4` | Equality predicates for the segments or angles formed by the points |
|  | `constangle n x1 x2 x3`<br>`constratio n x1 x2 x3 x4` | Constant-equality predicates for the segments or angles formed by the points |
| Degenerate Predicates | `neq p q`<br>`ncoll x y z`<br>`convex a b c d` | Points `p`, `q` are not equal<br>The points `x, y, z` are not collinear<br>`abcd` is a convex quadrilateral |
| Predicate2 | `center x a b c`<br>`incenter x a b c`<br>`excenter x a b c`<br>`orthocenter x a b c`<br>`centroid x a b c` | `x` is the circumcenter / incenter of `abc`<br>`x` is the `a`-excenter of `abc` (!!!) <br> `x` is the orthocenter / centroid of `abc` |
| Note: Predicate2's double up as constructions.<br><br>Predicate2's map directly to a list of Predicates that <br>the new points must satisfy. | `orthocenter2 x d e f a b c`<br>`centroid2 x d e f a b c`<br>`incenter2 x d e f a b c` | The `2` versions of each predicate add in the inner triangles `def` as well, with `d` opposite `a`, etc.|
|  | `midp p x y`<br>`reflect p y x1 x2` | `p` is the midpoint of segment `xy`<br>`p` is the reflection of `y` across the line `x1x2` |
|  | `inbisect x a b c`<br>`exbisect x a b c` | `x` lies on the internal angle bisector of angle `abc`<br>`x` lies on the external angle bisector of angle `abc` |
|  | `tangent p o x`<br>`ptangent p o1 x1 o2 x2` | `p` lies on the tangent to circle centered at `o` at `x`<br>`p` is the common tangency point of two circles centered at `o1` and `o2` respectively |
|  | `equilateral a b c`<br>`risos a b c`<br>`square a b c d`<br>`parallelogram a b c d` | `abc` is an equilateral / right-angled isosceles triangle<br>`abcd` is a square / parallelogram |
|  | `congtri a b c p q r`<br>`simtri a b c p q r` | Triangles `abc` and `pqr` are congruent / similar, with vertices oriented in this order |

### Syntax

| Logical syntax | | |
|---|---|---|
| Predicates | Every predicate is a truth statement regarding a set of points. | `pred p1 p2 ...` |
| Constructions | Every construction introduces a set of new points and depends on a set of old points. | `construction_name [new points] : [old points]` |
| | The specification of a construction in `constructions.txt` shall include the preconditions<br>necessary to perform the construction, as well as postconditions involving the new points.<br> The preconditions shall be stored as a clause.<br>The postconditions shall be stored as a list of predicates. | `` |


### Code Structure

The problem state:

```cpp
class Problem {
  std::map<int, std::unique_ptr<Point>> points;
  std::map<int, std::unique_ptr<Line>> lines;
  ...

  std::map<int, std::unique_ptr<Predicate>> preds;
}
```

Object nodes:

```cpp
class Node {
  std::string id;

  Node* parent;
  Node* root;
  Predicate* parent_why;
}

class Object : Node {
  // empty
}

class Point : Object {
  std::map<Line*, Predicate*> on_line;
  std::map<Circle*, Predicate*> on_circle;
}
```

Object2 nodes:

```cpp
class Object2 : Node {
  std::vector<Object*, Predicate*> obj;
  std::set<Predicate2*> pred2;

  Quantity* val;
}
```

Value nodes:

```cpp
class Value : Node {
  std::map<Object2*, Predicate*> obj2;
}
```

Numericals:

```cpp
typedef struct Frac {
  int num;
  int den;
} Frac;

typedef struct Coords {
  Frac x;
  Frac y;
} Coords;

typedef struct EqLine {
  Frac m;
  Frac c;
} EqLine;

typedef struct EqCircle {
  Coords c;
  Frac r;
} EqCircle;
```

The proof graph:

```cpp
union Arg {
  Object* obj;
  Frac n;
}

class Predicate {
  std::string id;

  std::string name;
  std::string rule_name;
  std::vector<Arg> args;

  std::set<Predicate*> why;
}

class Predicate2 {
  std::string id;

  std::string name;
  std::string rule_name;
  std::vector<Arg> args;

  std::set<Predicate*> why;
  std::set<Predicate*> hence;

  std::set<Object2*> obj2;
}
```

