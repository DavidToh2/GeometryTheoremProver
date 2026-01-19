# Angles

In AlphaGeometry's AR reasoning tool, each directed angle $\angle(AB, CD)$ is identified by two symbols $s_{AB}, s_{CD}$, with $s_{l}$ representing the counterclockwise angle of rotation of $l$ from the $y$-axis (mod $\pi$). We use this to represent $\angle(AB, CD) = s_{AB} - s_{CD}$. The columns of our angle matrix then correspond to the $s_l$'s. 

An alternative approach would be to let every matrix column correspond to a whole angle $\angle(d_1, d_2)$, where $d_1$ and $d_2$ are `Direction` s. However, this is not conducive to letting the AREngine introduce new angles to the GeometricGraph, as there is no way to "create new columns" representing new combinations of angles. For example, if the variable $\angle(d_1, d_3)$ did not already exist in the matrix, then there is no conducive way to determine if $\angle(d_1, d_2) + \angle(d_2, d_3)$ gives us any new `eqangles`, without checking the actual `Direction` nodes.

The objective of the AR engine is to exhaustively deduce all new equalities and perpendicularities: in other words, we are trying to find all new statements of the form
- $s_{l_1} - s_{l_2} - s_{l_3} + s_{l_4} \equiv 0$ or $\angle(d_1, d_2) = \angle(d_3, d_4)$
- $s_{l_1} - s_{l_2} = \pi/2$ or $\angle(d_1, d_2) = \pi/2$
- $s_{l_1} = s_{l_2}$ or $\angle(d_1, d_2) = 0$

## Modulo pi: Numeric resolution

Because our AR tool works with absolute expressions, it cannot handle expressions (mod $\pi$) by itself, meaning we need to devise a scheme to take care of the modulo for it. In other words, we need to add or subtract $\pi$ as necessary. If we think about our definition of $s$ as a "proxy" for line gradients, starting from $-\infty$ and increasing to $\infty$, then we can see that $s_{l_1} - s_{l_2}$ is accurate to the actual angle whenever the gradient of $l_1$ is greater than $l_2$, i.e. when the difference is positive. On the other hand, when the difference is negative, we will need to add $\pi$.

In other words, due to the modulo, we have $s_{l_1} - s_{l_2} = \angle(d_1, d_2)$ when $g_1 > g_2$, otherwise $s_{l_1} - s_{l_2} + \pi = \angle(d_1, d_2)$.

# Ratios and Lengths

AlphaGeo associates every length $AB$ with its log $\log{AB}$ so we can perform linear operations on them. I agree.

AlphaGeo also maintains two tables, `RatioTable` and `DistanceTable`. I think only the former table uses log-lengths (for multiplication and division of lengths); the latter table uses the raw lengths themselves (for addition and subtraction). This actually makes a lot of sense!

In their implementation:
- the `RatioTable` is used to `add_eq()` (functionally same as `add_cong()`), `add_const_ratio()` and `add_eqratio()` and is used to derive `cong2` and `eqratio`;
- the `DistanceTable` is used only to `add_cong()`s and is used to derive `inci`, `cong` and `rconst`.

## Directed lengths

Directed lengths are useful for deducing point coincidences: when $AB = AC$ and $B, C$ are on the same side of $A$, we may deduce that $B = C$. They are also useful for adding and subtracting lengths, since we are always clear of which we are doing. It is also well-known that Menelaus' and Ceva's theorems are only valid when we have information regarding the directed length ratios. 

However, directed ratios don't work so well for finding similar triangles. Picture a scenario where 

$$ \frac{AB}{BC} = -\frac{DE}{EF} \quad \text{and} \quad \angle ABC = \angle DEF $$

Should we still conclude that the two triangles are similar? Indeed, similar triangle chasing is a whole other can of worms in itself: in a situation where $B$ is the midpoint of $CD$, we could reasonably say that

$$ \frac{AB}{BC} = -\frac{AB}{BD} \quad \text{and} \quad \angle(AB, BC) = \angle(AB, BD) $$

but clearly $ABC, ABD$ are not similar. 

AlphaGeo's solution is to only employ directed lengths in the `DistanceTable`. For the `RatioTable`,
- Ceva and Menelaus' theorems are not treated (I assume it is assumed that the solver's ability to perform constructions will automatically compensate for this loss, since both theorems may be easily shown using constructed similar triangles);
- for the midpoint issue (which is an issue regardless of whether directed lengths are used), a numerical check is done by `dd.match_..._simtri()` in the form of `nm.same_clock(a.num, b.num, c.num, p.num, q.num, r.num)` to ensure that the absolute angles $\angle ABC = \angle DEF$ are exact and not just complementary.

Are there any alternatives we can consider?

## Vectors

Would it be feasible to conceptually encode every segment $AB$ as a vector $\overrightarrow{AB}$ using its polar coordinates $(r, \theta)$? This would fix both the point coincidence and similar triangle vs midpoint issue. 

However, ratio-chasing in general would become difficult, as the most general case is where we only need the absolute value of the ratio, not its complex form. `cong` itself would become difficult to parse, requiring $\lvert AB\rvert = \lvert XY\rvert$.

More seriously, the very existence of negative ratios would render our naive `log`-based interpretation invalid. For example, similar triangles, with triangles oriented in opposite directions (i.e. one clockwise and another anticlockwise), would yield 

$$ \frac{AB}{BC} = -\frac{DE}{EF} $$

which cannot be logged. 

...Unless we introduce complex logarithms in the form $\log{re^{i\theta}} = \log{r} + i(\theta + 2\pi k)$? Then we can take the imaginary part mod $2\pi$ - the real part corresponds to the vector's magnitude, while the imaginary part can be conceptualised as its counterclockwise angle of rotation. However, this is technically a duplication of functionality: see below for more information.

Other, somewhat contrived issues: in a situation where $AB$ bisects $\angle CAD$ (i.e. the internal angle bisector theorem) we have

$$ \left\lvert \frac{AC}{AD} \right\rvert = \left\lvert \frac{BC}{BD} \right\rvert \quad \text{but} \quad \frac{AC}{AD} \neq \frac{BC}{BD} $$

In a situation where $ABCD$ is a cyclic quadrilateral with $AC$ the symmedian line of triangle $BAD$ we also have

$$ \left\lvert \frac{AB}{AD} \right\rvert = \left\lvert \frac{CB}{CD} \right\rvert \quad \text{but} \quad \frac{AB}{AD} \neq \frac{CB}{CD} $$

Would it be feasible to maintain two separate tables, one conceptualising the "log vectors" and the other storing "log norms"? The first table's `get_all_eqs()` would provide us with:
- `get_all_eq_2s()`: all pairs of vectors which are the same, which is helpful for deducing point incidence, but also has the added benefit of being useful for identifying parallelograms
- `get_all_eq_3s()`: all pairs of vectors which ratio up to a constant. In this case the only reasonable constants are multiples of $1$ and $i$, so we would be able to find all pairs of vectors which are parallel or perpendicular. The former is just a duplication of the `2` variant; the latter might actually be useful, but to get there in the first place we must have derived it from `perp` predicates so doesn't seem that handy.
- `get_all_eq_4s()`: this is the interesting one which basically just directly tells us which triangles are similar. 

However, the last point is also the reason why we do not select this method: there is duplication of functionality. 
- Angle information will be recorded by both the angle table and our vector table. 
- The notion that angles are taken between directions is also broken, as there is now an additional layer of indirection - the directions - between our vectors and the angles, which we must introduce to the DDEngine. 
- Finally, we would need new predicates `const_ratio_vec` and `eqratio_vec` to indicate equality of ratios in the vector sense (distinguishing it from `eqratio`), introducing yet another layer of complexity to our rules (which I honestly just don't have the patience for right now).

Once nice thing about the vector solution would have been that our Numeric system supports direct verification of vector equality (and norm equality).

# Degenerate predicates

If `eqangle A B C D C D A B` or `const_angle 90 A B C D` then we know that `perp A B C D` so we should just add a `perp`.

If `eqratio A B C D C D A B` or `const_ratio 1 A B C D` then we know that `cong A B C D` so we should just add a `cong`.