# Angles

In Alphageometry's AR reasoning tool, each directed angle $\angle(AB, CD)$ is identified by two symbols $s_{AB}, s_{CD}$, with $s_{l}$ representing the clockwise angle of rotation of $l$ from the $x$-axis (mod pi). We use this to represent $\angle(AB, CD) = s_{AB} - s_{CD}$. The columns of our angle matrix then correspond to the $s_l$'s. 

An alternative approach would be to let every matrix column correspond to a whole angle $\angle(d_1, d_2)$, where $d_1$ and $d_2$ are `Direction` s. 

The objective of the AR engine is to exhaustively deduce all new equalities and perpendicularities: in other words, we are trying to find all new statements of the form
- $s_{l_1} - s_{l_2} - s_{l_3} + s_{l_4} = 0$ or $\angle(d_1, d_2) = \angle(d_3, d_4)$
- $s_{l_1} - s_{l_2} = \pi/2$ or $\angle(d_1, d_2) = \pi/2$
- $s_{l_1} = s_{l_2}$ or $\angle(d_1, d_2) = 0$

Let's go with AlphaGeo's implementation for now.