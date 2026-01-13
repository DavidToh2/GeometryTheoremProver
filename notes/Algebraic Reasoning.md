# Angles

In AlphaGeometry's AR reasoning tool, each directed angle $\angle(AB, CD)$ is identified by two symbols $s_{AB}, s_{CD}$, with $s_{l}$ representing the counterclockwise angle of rotation of $l$ from the $y$-axis (mod $\pi$). We use this to represent $\angle(AB, CD) = s_{AB} - s_{CD}$. The columns of our angle matrix then correspond to the $s_l$'s. 

An alternative approach would be to let every matrix column correspond to a whole angle $\angle(d_1, d_2)$, where $d_1$ and $d_2$ are `Direction` s. 

The objective of the AR engine is to exhaustively deduce all new equalities and perpendicularities: in other words, we are trying to find all new statements of the form
- $s_{l_1} - s_{l_2} - s_{l_3} + s_{l_4} \equiv 0$ or $\angle(d_1, d_2) = \angle(d_3, d_4)$
- $s_{l_1} - s_{l_2} = \pi/2$ or $\angle(d_1, d_2) = \pi/2$
- $s_{l_1} = s_{l_2}$ or $\angle(d_1, d_2) = 0$

Let's go with AlphaGeo's implementation for now.

Because our AR tool works with absolute expressions, it cannot handle expressions (mod $\pi$) by itself, meaning we need to devise a scheme to take care of the modulo for it. In other words, we need to add or subtract $\pi$ as necessary. If we think about our definition of $s$ as a "proxy" for line gradients, starting from $-\infty$ and increasing to $\infty$, then we can see that $s_{l_1} - s_{l_2}$ is accurate to the actual angle whenever the gradient of $l_1$ is greater than $l_2$, i.e. when the difference is positive. On the other hand, when the difference is negative, we will need to add $\pi$.

In other words, due to the modulo, we have $s_{l_1} - s_{l_2} = \angle(d_1, d_2)$ when $g_1 > g_2$, otherwise $s_{l_1} - s_{l_2} + \pi = \angle(d_1, d_2)$.

Comment: If this issue is not resolved, then erroneous expressions may be processed by the AR tool:

```
Adding the expression d_l_a_b*1.000000 + d_l_a_f*-1.000000 + d_l_b_c*-1.000000 + d_l_e_f*1.000000
Checking if expression is all zeroes: d_l_a_b*0.000000 + d_l_a_f*0.000000 + d_l_b_c*0.000000 + pi*0.000000
(-1) Expression already known!

Adding the expression d_l_a_b*-1.000000 + d_l_a_f*-1.000000 + d_l_b_c*1.000000 + d_l_e_f*1.000000
Checking if expression is all zeroes: d_l_a_b*0.000000 + d_l_a_f*0.000000 + d_l_b_c*0.000000 + pi*-1.000000
(0) Replaced occurrences of d_l_e_f with d_l_a_b*1.000000 + d_l_a_f*1.000000 + d_l_b_c*-1.000000
Checking if expression is all zeroes: d_l_a_b*-1.000000 + d_l_a_f*-1.000000 + d_l_b_c*1.000000 + d_l_e_f*1.000000
```

For now, we hotfix the above problem by hotwiring `Expr::all_zeroes()` to throw away the non-fractional parts of `pi`. THIS NEEDS TO BE FIXED LATER

# Ratios

