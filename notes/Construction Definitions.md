# Construction Definitions

## The most basic definitions

Constructions fundamentally involve:

- adding some new points to a collection of already existing points;
- a list of preconditions that the already existing points must satisfy;
- a list of postconditions that the new (and existing) points will together satisfy.

For example,

```
Let x be the reflection of a over bc.
CONSTRUCTION:   reflect x : a b c
PRECONDITIONS:  diff b c, ncoll a b c
POSTCONDITIONS: cong b a b x, cong c a c x, perp b c a x
```

This definition structure of `new points : old points`, `preconditions`, `postconditions` works well conceptually in capturing the essence of geometric constructions: as single-step additions being made to a diagram, based on already existing elements of the diagram.

The above specification works well for basic, single-step constructions like reflections. However, **specifically in problem definitions**, we often have to deal with more complex construction statements, such as

```
Let the perpendicular to a through bc intersect the circumcircle of ade at p.
Let the circumcircles of pqr and pxy intersect at z.
```

as well as auxiliary conditions such as

```
Let ad and be intersect at g. Given that og is perpendicular to ab, ...
```

as well as esoteric construction statements like

```
Let p be the point such that the triangles abc and adp are spirally similar.
Let t be the a-mixtilinear intouch point on the minor arc bc of the circle abc.
```

## Addressing complex construction statements using "construction stages"

In the case when we have a single object introduced in two or more constructions simultaneously, we could reasonably use a structure similar to the one below:

```
Let the perpendicular to a through bc intersect the circumcircle of ade at p.
p = on_pline p : a b c, on_circum p : a d e; ...

Let the circumcircles of pqr and pxy intersect at z.
z = on_circum z : p q r, on_circum z : x y z; ...
```

This allows us to define a **construction stage** involving multiple constructions.

However, certain construction definitions, like `square x y : a b`, intersect two or more points. This means we may have to deal with statements like the second line of the problem definition below (indented for clarity):

```
Let p, q be points on bc, and let r, s be points on ab, ac respectively such that pqrs is a square with vertices in this order.
p = on_line p : b c; q = on_line q : b c;
r, s = square r s : p q, on_line r : a b, on_line s : a c; ...
```

If we allow constructing multiple new points with multiple new constructions in a single step, then what's to prevent someone from just doing the entire problem at once in one giant construction stage?

```
p, q, r, s = on_line p : b c, on_line q : b c, [square r s : p q], on_line r : a b, on_line s : a c; ...
```

The issue with that approach would be the square-bracketed construction above. Because `p, q` are also new points being introduced in this step, they cannot be part of any construction's "existing points" set. Thus, we safely avoid the possibility of falling down that rabbit hole.

To summarise, a **construction stage**:
- can introduce multiple points
- can make use of multiple constructions, each of which involves some or all of the newly introduced points

This structure allows us to construct a "layered directed acyclic dependency graph" of constructed points. Every point has in-edges from all the points it depends on, with every in-edge labelled by the (possibly multiple) constructions involved. Each construction stage gives us a new layer of the graph.

Having said that, will we ever be in a situation where some points reference each other in a circular manner, i.e. the graph becomes cyclic? We address this below.

## Addressing auxiliary conditions

Some auxiliary conditions are easy to work with and may be simply treated as complex construction statements.

```
Let ad and be intersect at g. [Given that og is perpendicular to ab], ...
g = on_line g : a d, on_line g : b e, [on_pline g : o a b]; ...
```

Some others, however, are troublesome, especially when their subjects involve points which were defined a very long time ago. Consider the following example:

```
Let abc be a triangle with ... Let p, q be the intersections of rf, rg with the circumcircle of abc. [Given that a is the circumcenter of pqo], prove that...
```

We cannot make use of `circle a : p q o` because `a` was defined at the very start of the problem. In terms of our dependency graph, it would cause `p, q, o` to loop back to `a`. 

A clever way might be to attempt **re-defining the auxiliary condition** so that the new points `p, q` are the subjects. This might work something like

```
p = on_line p : r f, on_circum p : a b c, eqdistance p : a a o;
q = on_line q : r g, on_circum q : a b c, eqdistance q : a a o
```

There are, of course, going to be absolutely abhorrent questions which go something along the lines of

```
Given that a, b, c are the incenter, orthocenter and circumcenter of pqr respectively...
```

which would basically mandate an extremely long and convoluted construction stage of the form (indentions for clarity)

```
p = [constructions defining p];
q = [constructions defining q];
r = [constructions defining r], [establish a as incenter of pqr], [establish b as orthocenter of pqr], [establish c as circumcenter of pqr];
```

Following our definition structure, we have no choice but to ignore these cases. 

## Addressing esoteric construction statements

We take a bit of wisdom from Geogebra here, which doesn't try to overextend itself beyond the basics. We will require the end user translate every esoteric construction statement into a series of basic constructions with the same effect.

For example, the mixtilinear incenter `j` and intouch `t` may be defined as follows (indentation for clarity):

```
i = incenter i : a b c;
y = iso_triangle y : b c, on_tline y : a a i, on_circum y : a b c;
t = on_line t : i y, on_circum t : a b c;
d = on_tline d : i i a, on_line d : a b;
j = on_pline j : d a b, on_line j : a i
```

## Why not just use predicates?

Every construction introduced can satisfy one or more predicates. For stylistic reasons, it would be unwise to mix constructions and predicates in problem definitions.