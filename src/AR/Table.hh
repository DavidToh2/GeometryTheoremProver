#pragma once

#include <vector>
#include <map>
#include <set>

#include "Numerics/Numerics.hh"
#include "Numerics/Matrix.hh"
#include "DD/Predicate.hh"
#include <Highs.h>

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
    ExprHash hash(const Expr& expr);
    inline int hashlen(const ExprHash& expr_hash);
}

/* Table class.

Attributes:

- `A : Matrix`:
Stores the numeric values in a matrix form. Every variable gets its own row. Each
column corresponds to a zero-equality between the variables, of the 
form `v0*c0 + v1*c1 + ... + vn*cn = 0`, where `vi` are variables and `ci` are `Frac`s.

- `M : std::map<Var, std::map<Var, Frac>>`:
Stores expressions representing each variable as a linear combination of other 
variables. Variables are stored in the format `v: {v0: c0, v1: c1, ...}` and 
indicate that `v = v0*c0 + v1*c1 + ...`.
A global ordering of variables is imposed, so that the expression for a variable `v` 
only contains variables before `v` in the ordering. 
- `var_to_row : std::map<Var, int>`: 
Stores the mapping from variable names to their corresponding row indices in the 
matrix `A`.

- `equal_groups : list of std::set<std::pair<Var, Var>>`: 
Every set stores ordered pairs `(vi, vj)` of variables which are known to have the 
same value. For example, the set `{ (v1, v2), (v3, v4), (v5, v6) } would indicate
that `v1 - v2 = v3 - v4 = v5 - v6`. This is known as an `EqualGroup`.

- `deps : std::vector<Predicate*>`:
A list of predicates, one for each column. 

- `eq_Ns : std::map<Expr::ExprHash, EqualGroup>`:
For each `N = 2, 3, 4`, this variable is a map storing sets of variable pairs `(v1, v2)` 
satisfying `v1 - v2 = (u0*c0 + ... + un*cn)`. (In other words, every `Expr` is mapped to
a corresponding `EqualGroup`). 
For `N = 2`, every `EqualGroup` contains pairs of `Var` s which are identical.
For `N = 3`, every `EqualGroup` contains `(v1, v2)` satisfying `v1 - v2 = const * f`.
With `N = 4`, we store the general case.

- `eq_Ns_seen : std::set<Expr::VarPair>`:
For each `N = 2, 3, 4`, stores all variable pairs which have passed through `new_eq_Ns`.
*/
class Table {
public:
    typedef std::set<Expr::VarPair> EqualGroup;

    int num_vars;
    int num_eqs;
    const Expr::Var one;

    Matrix A;
    std::map<Expr::Var, Expr::Expr> M;
    std::map<Expr::Var, int> var_to_row;
    std::map<Expr::Var, Expr::Expr> var_to_expr;
    std::set<EqualGroup> equal_groups;

    std::vector<float> c;
    std::vector<Predicate*> deps;

    std::set<Expr::VarPair> eq_2s_seen;
    std::set<std::tuple<Expr::Var, Expr::Var, Frac>> eq_3s_seen;
    std::set<std::tuple<Expr::VarPair, Expr::VarPair>> eq_4s_seen;
    std::map<Expr::ExprHash, EqualGroup> eq_2s;
    std::map<Expr::ExprHash, EqualGroup> eq_3s;
    std::map<Expr::ExprHash, EqualGroup> eq_4s;

    Table(Expr::Var one_var = "1") : num_vars(0), num_eqs(0), one(one_var), A(0, 0) {}

    /* Add a free variable. */
    bool add_free(const Expr::Var& var_name);
    /* Add an expression of the form `v0*c0 + v1*c1 + ... = 0` to `var_to_expr`.
    The addition of expressions must respect the global ordering of variables. 
    If all variables are already present in the `Table`, then we may extract the last
    variable as the subject of an expression used to simplify `var_to_expr`.
    If there is at least one new variable in the `Table`, then all but one
    of the new variables are added as free variables. The last new variable is then
    used as the subject of a row in `var_to_expr`. */
    bool add_expr(const Expr::Expr& expr);
    /* Simplifies all expressions in `var_to_expr` by replacing occurrences of `var` 
    with `expr`. 
    Note: In order to respect the global variable ordering requirement, `expr` 
    should only contain variables before `var` in the ordering. */
    void replace(const Expr::Var& var, const Expr::Expr& expr);
    /* Register an expression of the form `v0*c0 + v1*c1 + ... = 0` into the
    matrix `A`, along with an associated predicate. */
    bool register_expr(const Expr::Expr& expr, Predicate* pred);

    /* Register an expression of the form `var1*m - var2*n = 0`. */
    bool register_2(const Expr::Var& var1, const Expr::Var& var2, float m, float n, Predicate* pred);
    /* Register an expression of the form `var1 - var2 = f`. */
    bool register_3(const Expr::Var& var1, const Expr::Var& var2, float f, Predicate* pred);
    /* Register an expression of the form `var1 - var2 = var3 - var4`. */
    bool register_4(const Expr::Var& var1, const Expr::Var& var2, const Expr::Var& var3, const Expr::Var& var4, Predicate* pred);


    bool record_eq_2_as_seen(const Expr::VarPair& vp);
    bool record_eq_3_as_seen(const Expr::VarPair& vp, const Frac f);
    bool record_eq_4_as_seen(const Expr::VarPair& vp1, const Expr::VarPair& vp2);
    bool is_eq_4_seen(const Expr::Var var1, const Expr::Var var2, const Expr::Var var3, const Expr::Var var4);


    bool add_eq_2(const Expr::Var& var1, const Expr::Var& var2, float m, float n, Predicate* pred);
    bool add_eq_3(const Expr::Var& var1, const Expr::Var& var2, float f, Predicate* pred);
    bool add_eq_4(const Expr::Var& var1, const Expr::Var& var2, const Expr::Var& var3, const Expr::Var& var4, Predicate* pred);


    /* A group is a set of variables which are equivalent according to some
    equivalence relation `=`. For example, the group `{1, 2, 3, 4}` would indicate 
    that `1 = 2 = 3 = 4`.
    Given a list of `groups`, and a second list of `new_groups`, perform an update 
    on the original list that implements the new equivalence information provided 
    by `new_groups`. Return a minimal list of element pairs, called `links`, applied
    during the update. 
    Note: For our purposes, every element will be a `VarPair`. Hence, `links` will
    contain a minimal list of pairs of `VarPair` s.*/
    template <typename T>
    static void
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
    }

    /* Figure out why an expression holds.
    This is done by finding a linear combination of expressions, as recorded in `A`, that 
    corresponds to `expr`. To find the smallest such linear combination, we use linear
    optimisation. */
    std::vector<Predicate*> why(const Expr::Expr& expr);


    Generator<Expr::VarPair> all_varpairs() const;
    void get_all_eqs();
    Generator<std::tuple<Expr::Var, Expr::Var, std::vector<Predicate*>>> get_all_eq_2s_and_why();
    Generator<std::tuple<Expr::Var, Expr::Var, Frac, std::vector<Predicate*>>> get_all_eq_3s_and_why();
    Generator<std::tuple<Expr::Var, Expr::Var, Expr::Var, Expr::Var, std::vector<Predicate*>>> get_all_eq_4s_and_why();
};