
#include "Numerics/Numerics.hh"
#include <Highs.h>

/* Table class.

Attributes:

- `M : std::map<std::string, std::map<std::string, Frac>>`:
Stores expressions representing each value as a linear combination of other values.
Values are stored in the format `v: {v0: c0, v1: c1, ...}` and indicate
that `v = v0*c0 + v1*c1 + ...`.
A global ordering of values is imposed, so that the expression for a value `v` only
contains values before `v` in the ordering. */
class Table {
public:
    std::map<std::string, std::map<std::string, Frac>> M;

};