#pragma once

#include <vector>
#include <map>
#include <set>

#include "AR/LinProg.hh"
#include "Numerics/Numerics.hh"
#include "Numerics/Matrix.hh"
#include "DD/Predicate.hh"

namespace Expr {
    typedef std::string Var;
    typedef std::pair<Var, Var> VarPair;
    typedef std::map<Var, double> Expr;
    typedef Expr ExprHash;

    double fix_v(const double d);
    void fix(Expr& expr);
    void strip(Expr& expr);
    bool all_zeroes(const Expr& expr);
    void __add(Expr& expr1, const Expr& expr2);
    Expr add(const Expr& expr1, const Expr& expr2);
    Expr add_fold(const std::vector<Expr>& exprs);
    void __mult(Expr& expr, const double c);
    Expr mult(const Expr& expr, const double c);
    void __minus(Expr& expr1, const Expr& expr2);
    Expr minus(const Expr& expr1, const Expr& expr2);
    void __div(Expr& expr, const double c);
    Expr div(const Expr& expr, const double c);
    void __replace(Expr& expr, const Var& var, const Expr& sub_expr);
    Expr replace(const Expr& expr, const Var& var, const Expr& sub_expr);
    /* Given an expression of the form `v0*c0 + v1*c1 + ... + vn*cn = 0`, extracts
    the lexicographically largest variable `vn` as the subject of the equivalent
    expression `vn = -(v0*c0 + ... + v[n-1]*c[n-1]) / cn`.
    The lexicographical invariant is necessary to maintain an ordering of variables
    for the Table class, and is contingent on the underlying `std::map` being 
    sorted. */
    std::pair<Var, Expr> get_subject(const Expr& expr, const Var c);

    std::string to_string(const Var& var);
    int len(const Expr& expr);
    ExprHash hash(const Expr& expr);
    inline int hashlen(const ExprHash& expr_hash);
}

/* Table class.

`Var` stands for variable representing either `Direction` or `Length`.

## Attributes

- `A : Matrix`:
Stores the numeric values in a matrix form. Every variable gets its own row. Each
column corresponds to a zero-equality between the variables, of the 
form `v0*c0 + v1*c1 + ... + vn*cn = 0`, where `vi` are variables and `ci` are `Frac`s.
Columns are stored in pairs, with one positive and one negative version.

- `var_to_row : std::map<Var, int>`: 
Stores the mapping from variable names to their corresponding row indices in the 
matrix `A`.

- `c : std::vector<float>`:
A vector storing either 1 or -1, indicating whether each column is positive or negative.

- `deps : std::vector<Predicate*>`:
A list of predicates, one for each column pair of `A`.

The matrix `A` is only used to solve the Mixed-Integer Linear Program (MILP) used to
deduce the predicate dependencies of expressions. As a result, the entries of `A` never
need to be mutated, and we also don't need to care about variable ordering etc.
See the `why()` method for details on how this is implemented.

- `M_var_to_expr : std::map<Var, Expr>`:
Stores expressions representing each variable as a linear combination of other 
variables. Variables are stored in the format `v: {v0: c0, v1: c1, ...}` and 
indicate that `v = v0*c0 + v1*c1 + ...`.
A global ordering of variables is imposed, so that the expression for a variable `v` 
only contains variables before `v` in the ordering. 

- `equal_groups : list of std::set<VarPair>`: 
Every set stores ordered pairs `(vi, vj)` of variables which are known to have the 
same value. For example, the set `{ (v1, v2), (v3, v4), (v5, v6) }` would indicate
that `v1 - v2 = v3 - v4 = v5 - v6`. This is known as an `EqualGroup`.

- `eq_Ns : std::map<ExprHash, EqualGroup>`:
For each `N = 2, 3, 4`, this variable is a map storing sets of variable pairs `(v1, v2)` 
satisfying `v1 - v2 = (u0*c0 + ... + un*cn)`. (In other words, every `Expr` is mapped to
a corresponding `EqualGroup`). 
For `N = 2`, every `EqualGroup` contains pairs of `Var` s whose ratios are the same. Note
that this doesn't really make sense outside of the case where the `Var` s are segment
lengths, so this is only used by `RatioTable` to store length ratios.
For `N = 3`, every `EqualGroup` contains `(v1, v2)` satisfying `v1 - v2 = const * f`.
With `N = 4`, we store the general case.

- `eq_Ns_seen`:
For each `N = 2, 3, 4`, stores all variable sets which have passed through `eq_Ns`. This
is either `(v1, v2)`, `(v1, v2, f)`, or `((v1, v2), (v3, v4))` respectively.

The map `M_var_to_expr` is a matrix in packed form that stores the Gaussian-eliminated
form of `A` based on the global variable ordering. It stores the most-up-to-date and
simplified expressions for each variable, and is used to check if expressions being
added are already known to the `Table`.

## Adding expressions

When an expression is added to the `Table` via `add_eq_N()`,
- we first call `record_eq_N_as_seen()`, which returns `false` if an exact copy of the
  expression has already been added before,
- we then call `add_expr()`, which returns `false` if the expression is already known
  to the `Table` (i.e., it can be derived from existing expressions in `M_var_to_expr`),
  and adds it to `M_var_to_expr` otherwise;
- finally we call `register_expr()`, which adds the expression to the matrix `A` along
  with its associated predicate.

## Fetching equalities

When `get_all_eqs()` is called, we iterate through all ordered distinct variable pairs 
`(v1, v2)`, storing them in:
- `eq_2s` if they satisfy `v1 - v2 = 0`, otherwise
- `eq_3s` if they satisfy `v1 - v2 = f`, otherwise
- `eq_4s` in the general case

We then call each of `get_all_eq_Ns_and_why()`, which returns all newly derived, unordered 
variable pairs (or quadruples) satisfying each of the three equality types above. This is
done by
- iterating through the corresponding `eq_Ns` map,
- calling `is_eq_N_seen()` to skip unordered pairs (or quadruples) which have already been
  seen,
- calling `record_eq_N_as_seen()` to mark the pair (or quadruple) as seen,
- and calling `why()` on the corresponding expression to fetch the list of predicates
  which imply the equality.
*/
class Table {
public:
    typedef std::set<Expr::VarPair> EqualGroup;

    int num_vars;
    int num_eqs;
    const Expr::Var one;

    SparseMatrix A;
    std::map<Expr::Var, int> var_to_row;
    std::vector<double> c;
    std::vector<Predicate*> deps;
    LinProg lp_solver;

    std::map<Expr::Var, Expr::Expr> M_var_to_expr;
    std::set<EqualGroup> equal_groups;

    std::set<Expr::VarPair> eq_2s_seen;
    std::set<std::tuple<Expr::Var, Expr::Var, Frac>> eq_3s_seen;
    std::set<std::tuple<Expr::VarPair, Expr::VarPair>> eq_4s_seen;
    std::map<Expr::ExprHash, EqualGroup> eq_2s;
    std::map<Expr::ExprHash, EqualGroup> eq_3s;
    std::map<Expr::ExprHash, EqualGroup> eq_4s;

    Table(Expr::Var one_var = "1") : num_vars(0), num_eqs(0), one(one_var), A(0, 0, 4) {}

    /* Add a free variable. */
    bool add_free(const Expr::Var& var_name);

    /* Add an expression of the form `v0*c0 + v1*c1 + ... = 0` to `M_var_to_expr`.
    The addition of expressions must respect the global ordering of variables. 

    If all variables are already present in the `Table`, then:
    - If the expression is already known, we do nothing and return `false`.
    - Else, we may extract the last variable as the subject of an expression used to 
        simplify `M_var_to_expr`.

    If there is at least one new variable in the `Table`, then all but one
    of the new variables are added as free variables. The last new variable is then
    used as the subject of a row in `M_var_to_expr`. */
    bool add_expr(const Expr::Expr& expr);

    /* Simplifies all expressions in `M_var_to_expr` by replacing occurrences of `var` 
    with `expr`. 
    Note: In order to respect the global variable ordering requirement, `expr` 
    should only contain variables before `var` in the ordering. */
    void replace(const Expr::Var& var, const Expr::Expr& expr);

    /* Register an expression of the form `v0*c0 + v1*c1 + ... = 0` into the
    matrix `A`, along with an associated predicate. 
    This function is called by `add_eq`. Note that all the expressions we
    add contain at most 4 terms. 
    Also adds new variables to `var_to_row`. */
    bool register_expr(const Expr::Expr& expr, Predicate* pred);


    bool record_eq_2_as_seen(const Expr::Var& v1, const Expr::Var& v2);
    bool record_eq_3_as_seen(const Expr::Var& v1, const Expr::Var& v2, const Frac f);
    bool record_eq_4_as_seen(const Expr::Var& v1, const Expr::Var& v2, const Expr::Var& v3, const Expr::Var& v4);
    bool is_eq_2_seen(const Expr::Var var1, const Expr::Var var2);
    bool is_eq_3_seen(const Expr::Var var1, const Expr::Var var2, const Frac f);
    bool is_eq_4_seen(const Expr::Var var1, const Expr::Var var2, const Expr::Var var3, const Expr::Var var4);


    /* Adds an expression of the form `v0*c0 + v1*c1 + ... = 0` to the `Table`.
    Note: Looking at all the use cases across `add_eq_N`, we can see that in the vast
    majority of cases, the only coefficients `ci` that will ever be stored are `Frac` s.
    There IS a possible exception where we could for e.g. have length ratios which
    evaluate to irrational numbers (like `sqrt(2)`), but these problems are exceedingly
    rare. For now we maintain the general float implementation. */
    bool add_eq(const Expr::Expr& expr, Predicate* pred);

    /* Adds an expression of the form `var1*m - var2*n = 0` to the `Table`.
    Used by `RatioTable` to `add_const_ratio`. */
    bool add_eq_2(const Expr::Var& var1, const Expr::Var& var2, float m, float n, Predicate* pred);

    /* Adds an expression of the form `var1 - var2 = f` to the `Table`. 
    Used by `RatioTable` to `add_eq` (with `f = 0`).
    Used by `AngleTable` to `add_const_angle` and `add_para` (the former includes the
    special case `add_perp` with `f = 0.5`, and the latter takes `f = 0`). */
    bool add_eq_3(const Expr::Var& var1, const Expr::Var& var2, float f, Predicate* pred);

    /* Adds an expression of the form `var1 - var2 - var3 + var4 = 0` to the `Table`.
    Used by `RatioTable` to `add_eqratio`.
    Used by `AngleTable` to `add_eqangle`.
    Used by `DistanceTable` to `add_cong`. */
    bool add_eq_4(const Expr::Var& var1, const Expr::Var& var2, const Expr::Var& var3, const Expr::Var& var4, Predicate* pred);


    /* A group is a set of variables which are equivalent according to some
    equivalence relation `=`. For example, the group `{1, 2, 3, 4}` would indicate 
    that `1 = 2 = 3 = 4`.
    Given a list of `groups`, and a `new_group`, perform an update on the original list 
    that implements the new equivalence information provided by `new_groups`. Return a 
    minimal list of element pairs, called `links`, applied during the update. 
    Note: For our purposes, every element will be a `VarPair`. Hence, `links` will
    contain a minimal list of pairs of `VarPair` s.
    
    Parameters:
    - `groups : std::set<std::set<T>>` (usually `std::set<EqualGroup>`)
    - `new_group : std::Set<T>` (usually `EqualGroup`)
    - `links : std::vector<std::pair<T, T>>` (usually `std::vector<std::pair<VarPair, VarPair>>`)*/
    template <typename T>
    static bool
    update_equal_groups(std::set<std::set<T>>& groups, const std::set<T>& new_group, std::vector<std::pair<T, T>>& links) {

        std::vector<bool> merged(groups.size(), false);
        std::set<T> merged_set;
        bool _old_set = false;
        T _old;
        std::set<T> _new;
        
        for (const T& vp : new_group) {
            bool found = false;

            int i=-1;
            for (const std::set<T>& group : groups) {
                i++;
                if (!group.contains(vp)) continue;
                found = true;
                if (merged[i]) continue;
                merged[i] = true;
                Utils::__unify_sets(merged_set, group);
                if (_old_set) {
                    links.emplace_back(_old, vp);
                }
                _old = vp;
                _old_set = true;
            }
            if (!found) _new.insert(vp);
        }

        if (_old_set && (!_new.empty())) {
            links.emplace_back(_old, *_new.begin());
            Utils::__unify_sets(merged_set, _new);
        }

        if (!_new.empty()) {
            for (auto it = _new.begin(); it != std::prev(_new.end()); ++it) {
                links.emplace_back(*it, *std::next(it));
            }
        }

        if (!merged_set.empty()) {
            int i=0;
            for (auto it = groups.begin(); it != groups.end(); ) {
                if (merged[i++]) {
                    it = groups.erase(it);
                } else {
                    ++it;
                }
            }
            groups.insert(merged_set);
        }
        if (!_old_set && !(_new.empty())) {
            groups.insert(_new);
        }

        return true;
    }

    /* Figure out why an expression holds.
    This is done by finding a linear combination of expressions, as recorded in `A`, that 
    corresponds to `expr`. To find the smallest such linear combination, we use linear
    optimisation. 
    Called by `get_all_eq_Ns_and_why()`. */
    std::vector<Predicate*> why(const Expr::Expr& expr);

    /* Gets all pairs of distinct variables `(v1, v2)`. */
    Generator<Expr::VarPair> all_varpairs() const;

    /* Populate the `eq_Ns` maps. 

    Note: All ordered pairs will be populated. This is necessary as some `eh` might have
    two variable pairs `(v1, v2)` and `(v4, v3)` corresponding to it, satisfying the
    ordering `v1 < v2` and `v4 > v3`. In other words, we cannot make assumptions on the
    ordering of the variables being stored in `eq_Ns`. */
    void get_all_eqs();

    /* Returns all unordered pairs of distinct variables `(v1, v2)` which have been deduced 
    to be the same, i.e. satisfying `v1 - v2 = 0`.
    Note: Because of the way `is_eq_2_seen()` works, each unordered pair is only yielded once.

    Used by `AngleTable` to `get_all_paras`.
    Used by `RatioTable` to `get_all_congs`. */
    Generator<std::tuple<Expr::Var, Expr::Var, std::vector<Predicate*>>> get_all_eq_2s_and_why();

    /* Returns all unordered pairs of distinct variables `(v1, v2)` and float `f` which have
    been deduced to satisfy `v1 - v2 = f`.
    Note: Because of the way `is_eq_3_seen()` works, each unordered pair is only yielded once.

    Used by `AngleTable` to `get_all_const_angles`.
    Used by `RatioTable` to `get_all_const_ratios`. */
    Generator<std::tuple<Expr::Var, Expr::Var, Frac, std::vector<Predicate*>>> get_all_eq_3s_and_why();

    /* Returns all unordered 4-tuples of distinct variables `((v1, v2), (v3, v4))` which have
    been deduced to satisfy `v1 - v2 = v3 - v4`.
    Note: Because of the way `is_eq_4_seen()` works, each unordered 4-tuple is only yielded 
    once.

    Used by `AngleTable` to `get_all_eqangles`.
    Used by `RatioTable` to `get_all_eqratios`. */
    Generator<std::tuple<Expr::Var, Expr::Var, Expr::Var, Expr::Var, std::vector<Predicate*>>> get_all_eq_4s_and_why();
};