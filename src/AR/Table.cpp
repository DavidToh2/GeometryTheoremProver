
#include "Table.hh"
#include "Matrix.hh"
#include "Common/NumUtils.hh"

#define DEBUG_TABLE 0

#if DEBUG_TABLE
    #define LOG(x) do {std::cout << x << std::endl;} while(0)
#else 
    #define LOG(x)
#endif

double Expr::fix_v(const double d) {
    return Frac(d).to_double();
}
void Expr::fix(Expr& expr) {
    for (auto& [var, coeff] : expr) {
        // HOTFIX: remove integer parts from pi
        if (var == "pi") {
            coeff = coeff - floorf(coeff);
        }
        coeff = fix_v(coeff);
    }
}
void Expr::strip(Expr& expr) {
    for (auto it = expr.cbegin(); it != expr.cend(); ) {
        if (NumUtils::is_close(it->second, 0.0)) {
            it = expr.erase(it);
        } else {
            ++it;
        }
    }
}
bool Expr::all_zeroes(const Expr& expr) {

    for (const auto& [var, coeff] : expr) {
        // HOTFIX: allow pi to be non-zero integer multiples
        if (var == "pi") {
            if (!NumUtils::is_close(coeff, roundf(coeff))) {
                return false;
            }
            continue;
        }
        if (!NumUtils::is_close(coeff, 0.0)) {
            return false;
        }
    }
    return true;
}
void Expr::__add(Expr& expr1, const Expr& expr2) {
    for (const auto& [var, coeff] : expr2) {
        if (expr1.contains(var)) {
            expr1[var] += coeff;
        } else {
            expr1[var] = coeff;
        }
    }
}
Expr::Expr Expr::add(const Expr& expr1, const Expr& expr2) {
    Expr result = expr1;    // copy-construction
    for (const auto& [var, coeff] : expr2) {
        if (result.contains(var)) {
            result[var] += coeff;
        } else {
            result[var] = coeff;
        }
    }
    return result;
}
void Expr::__mult(Expr& expr, const double c) {
    for (auto& [var, coeff] : expr) {
        coeff *= c;
    }
}
Expr::Expr Expr::mult(const Expr& expr, const double c) {
    Expr result = expr;   // copy-construction
    for (auto& [_, coeff] : result) {
        coeff *= c;
    }
    return result;
}
void Expr::__minus(Expr& expr1, const Expr& expr2) {
    for (const auto& [var, coeff] : expr2) {
        if (expr1.contains(var)) {
            expr1[var] -= coeff;
        } else {
            expr1[var] = -coeff;
        }
    }
}
Expr::Expr Expr::minus(const Expr& expr1, const Expr& expr2) {
    Expr result = expr1;    // copy-construction
    for (const auto& [var, coeff] : expr2) {
        if (result.contains(var)) {
            result[var] -= coeff;
        } else {
            result[var] = -coeff;
        }
    }
    return result;
}
void Expr::__div(Expr& expr, const double c) {
    for (auto& [var, coeff] : expr) {
        coeff /= c;
    }
}
Expr::Expr Expr::div(const Expr& expr, const double c) {
    Expr result = expr;   // copy-construction
    for (auto& [_, coeff] : result) {
        coeff /= c;
    }
    return result;
}
void Expr::__replace(Expr& expr, const Var& var, const Expr& sub_expr) {
    if (expr.contains(var)) {
        double coeff = expr[var];
        expr.erase(var);
        __add(expr, mult(sub_expr, coeff));
    }
}
Expr::Expr Expr::replace(const Expr& expr, const Var& var, const Expr& sub_expr) {
    Expr result = expr;    // copy-construction
    if (result.contains(var)) {
        double coeff = result[var];
        result.erase(var);
        __add(result, mult(sub_expr, coeff));
    }
    return result;
}
std::pair<Expr::Var, Expr::Expr> Expr::get_subject(const Expr& expr, const Var c) {
    Expr result = expr;   // copy-construction
    strip(result);
    Var subject = "";
    double subject_c;
    for (const auto& [var, coeff] : result) {
        if (var != c) {
            subject = var;
            subject_c = coeff;
            break;
        }
    }
    if (subject.empty()) {
        return {"", {}};
    }
    result.erase(subject);
    __div(result, -subject_c);
    return {subject, result};
}
std::string Expr::to_string(const Var& var) {
    return var;
}
int Expr::len(const Expr& expr) {
    return expr.size();
}
Expr::ExprHash Expr::hash(const Expr& expr) {
    // auto it = expr.cbegin();
    // std::string h = to_string(it->first) + "*" + std::to_string(it->second) + ",";
    // while(it != expr.cend()) {
    //     const auto& [var, coeff] = *(it++);
    //     h += " + " + to_string(var) + "*" + std::to_string(coeff);
    // }
    // return h;
    ExprHash expr_hash = expr;
    return expr_hash;
}
int Expr::hashlen(const ExprHash& expr_hash) {
    return expr_hash.size();
}
std::string Expr::to_string(const Expr& expr) {
    std::string s = "";
    auto it = expr.cbegin();
    if (it == expr.cend()) {
        return "0";
    }
    s += to_string(it->first) + "*" + std::to_string(it->second);
    ++it;
    while(it != expr.cend()) {
        const auto& [var, coeff] = *(it++);
        s += " + " + to_string(var) + "*" + std::to_string(coeff);
    }
    return s;
}






bool Table::add_free(const Expr::Var& var_name) {
    M_var_to_expr[var_name] = {{var_name, 1.0}};
    return true;
}
bool Table::is_free(const Expr::Var& var_name) const {
    if (!M_var_to_expr.contains(var_name)) {
        return false;
    }
    const Expr::Expr& expr = M_var_to_expr.at(var_name);
    return (expr.size() == 1) && (expr.contains(var_name)) && (NumUtils::is_close(expr.at(var_name), 1.0));
}
bool Table::add_expr(const Expr::Expr& expr) {
    // Invariant: Every expression in M_var_to_expr should only contain free variables.

    // Find new variables
    std::vector<std::pair<Expr::Var, double>> new_vars;
    Expr::Expr result;

    LOG("Adding the expression " << Expr::to_string(expr));

    for (const auto& [var, d] : expr) {
        if (M_var_to_expr.contains(var)) {
            Expr::__add(result, Expr::mult(M_var_to_expr[var], d));
            // By the invariant, result only contains free variables
        } else {
            new_vars.push_back({var, d});
        }
    }

    Expr::strip(result);
    Expr::fix(result);

    if (new_vars.size() == 0) {
        if (Expr::all_zeroes(result)) {
            LOG("(-1) Expression already known!");
            return false; // Expression already known
        }
        auto [subject, expr_subj] = Expr::get_subject(result, one);
        if (subject.empty()) return false;
        // By the invariant, subject must be a free variable, which is about to become non-free
        // as it is substituted by expr_subj and replaced in all other expressions.
        replace(subject, expr_subj);

        LOG("(0) Replaced occurrences of " << Expr::to_string(subject) << " with " << Expr::to_string(expr_subj));

    } else if (new_vars.size() == 1) {
        auto [var, d] = new_vars[0];
        M_var_to_expr[var] = Expr::div(result, -d);
        // Invariant maintained: M_var_to_expr[var] only contains free variables
        
        LOG("(1) Added the expression " << Expr::to_string(var) << " = " << Expr::to_string(M_var_to_expr[var]));

    } else {
        Expr::Var dependent_var = "";
        double dependent_d = 0;
        for (auto& [var, d] : new_vars) {
            if (dependent_var.empty() && (var != one)) {
                dependent_var = var;
                dependent_d = d;
                continue;
            }
            add_free(var);
            Expr::__add(result, {{var, d}});
        }
        M_var_to_expr[dependent_var] = Expr::div(result, -dependent_d);
        // Invariant maintained: M_var_to_expr[var] only contains free variables

        LOG("(2) Added the expression " << Expr::to_string(dependent_var) << " = " << Expr::to_string(M_var_to_expr[dependent_var]));
    }

    return true;
}

void Table::replace(const Expr::Var& var, const Expr::Expr& sub_expr) {
    // Invariant: This function is only ever invoked with var being a free variable.
    for (auto& [_, expr] : M_var_to_expr) {
        Expr::__replace(expr, var, sub_expr);
    }
    return;

    // The code from here on should never run, due to the invariant mentioned above.

    Expr::Expr new_expr = Expr::minus(M_var_to_expr[var], sub_expr);
    Expr::strip(new_expr);
    Expr::fix(new_expr);
    if (Expr::all_zeroes(new_expr)) return;

    assert(new_expr.size() >= 2);

    auto [subject, expr_subj] = Expr::get_subject(new_expr, one);
    replace(subject, expr_subj);
}

bool Table::register_expr(const Expr::Expr& expr, Predicate* pred) {
    if (Expr::all_zeroes(expr)) {
        return false;
    }
    for (const auto& [var, _] : expr) {
        if (!var_to_idx.contains(var)) {
            var_to_idx[var] = num_vars++;
        }
    }
    if (num_vars > A.m) {
        A.extend_rows(num_vars - A.m);
    }
    SparseMatrix new_columns = SparseMatrix(num_vars, 2, 4);
    for (const auto& [var, coeff] : expr) {
        new_columns.set(var_to_idx[var], 0, coeff);
        new_columns.set(var_to_idx[var], 1, -coeff);
    }
    A.extend_columns(new_columns);
    num_eqs += 1;
    c.emplace_back(1);
    c.emplace_back(-1);
    deps.push_back(pred);
    return true;
}



bool Table::record_eq_2_as_seen(const Expr::Var& v1, const Expr::Var& v2) {
    return eq_2s_seen.insert({v1, v2}).second;
}
bool Table::record_eq_3_as_seen(const Expr::Var& v1, const Expr::Var& v2, const Frac f) {
    return eq_3s_seen.insert({v1, v2, f}).second;
}
bool Table::record_eq_4_as_seen(const Expr::Var& v1, const Expr::Var& v2, const Expr::Var& v3, const Expr::Var& v4) {
    return eq_4s_seen.insert({{v1, v2}, {v3, v4}}).second;
}
bool Table::is_eq_2_seen(const Expr::Var var1, const Expr::Var var2) {
    return (
        eq_2s_seen.contains({var1, var2}) 
        || eq_2s_seen.contains({var2, var1})
    );
}
bool Table::is_eq_3_seen(const Expr::Var var1, const Expr::Var var2, const Frac f) {
    return (
        eq_3s_seen.contains({var1, var2, f}) 
        || eq_3s_seen.contains({var2, var1, Frac(1)-f})
    );
}
bool Table::is_eq_4_seen(const Expr::Var var1, const Expr::Var var2, const Expr::Var var3, const Expr::Var var4) {
    return (
        eq_4s_seen.contains({{var1, var2}, {var3, var4}}) 
        || eq_4s_seen.contains({{var2, var1}, {var4, var3}})
        || eq_4s_seen.contains({{var3, var4}, {var1, var2}})
        || eq_4s_seen.contains({{var4, var3}, {var2, var1}})
    );
}


bool Table::add_eq(const Expr::Expr& expr, Predicate* pred) {
    return (
        add_expr(expr) 
        && register_expr(expr, pred)
    );
}
bool Table::add_eq_2(const Expr::Var& var1, const Expr::Var& var2, float m, float n, Predicate* pred) {
    return (
        record_eq_2_as_seen(var1, var2)
        && add_eq({{var1, m}, {var2, -n}}, pred)
    );
}
bool Table::add_eq_3(const Expr::Var& var1, const Expr::Var& var2, float f, Predicate* pred) {
    return (
        record_eq_3_as_seen(var1, var2, Frac(f))
        && add_eq({{var1, 1}, {var2, -1}, {one, -f}}, pred) 
    );
}
bool Table::add_eq_4(const Expr::Var& var1, const Expr::Var& var2, const Expr::Var& var3, const Expr::Var& var4, Predicate* pred, Expr::Expr offset) {
    std::vector<std::pair<Expr::VarPair, Expr::VarPair>> links;
    return (
        (record_eq_4_as_seen(var1, var2, var3, var4) || record_eq_4_as_seen(var1, var3, var2, var4))
        && add_eq(Expr::add({{var1, 1}, {var2, -1},{var3, -1}, {var4, 1}}, offset), pred)
        // && Table::update_equal_groups(equal_groups, {{var1, var2}, {var3, var4}}, links)
        // && Table::update_equal_groups(equal_groups, {{var2, var1}, {var4, var3}}, links)
    );
}



std::vector<Predicate*> Table::why(const Expr::Expr& expr) {
    std::vector<Predicate*> result;

    Expr::Expr target = expr;
    Expr::strip(target);
    Expr::fix(target);

    // Convert the target expr into a std::vector<double> b
    std::vector<double> b_vec(num_vars, 0.0);
    for (const auto& [var, coeff] : target) {
        b_vec[var_to_idx.at(var)] = coeff;
    }
    lp_solver.populate(A, b_vec, c);

    // Solve the linear program min c^T * x subject to A * x = b, x >= 0
    std::vector<double> solution;
    if (!lp_solver.solve(solution)) {
        return result;
    }
    assert(solution.size() == 2*num_eqs);

    for (int i = 0; i < num_eqs; i++) {
        if (!(NumUtils::is_close(solution[2*i], 0.0) && NumUtils::is_close(solution[2*i + 1], 0.0))) {
            result.push_back(deps[i]);
        }
    }
    return result;
}



Generator<Expr::VarPair> Table::all_varpairs() const {
    for (const auto& [var1, _] : M_var_to_expr) {
        if (var1 == one) continue;
        for (const auto& [var2, _] : M_var_to_expr) {
            if (var2 == one || var2 == var1) continue;
            co_yield {var1, var2};
        }
    }
    co_return;
}

void Table::generate_all_eqs() {
    eq_2s.clear();
    eq_3s.clear();
    eq_4s.clear();

    auto all_varpairs_gen = all_varpairs();
    while (all_varpairs_gen) {
        auto [var1, var2] = all_varpairs_gen();
        Expr::Expr e1 = M_var_to_expr[var1], e2 = M_var_to_expr[var2];
        Expr::Expr e12 = Expr::minus(e1, e2);
        Expr::strip(e12);
        Expr::fix(e12);
        Expr::ExprHash eh = Expr::hash(e12);    // eh == e12
        
        int l = Expr::hashlen(eh);
        if (l == 0) {
            eq_2s[eh].insert({var1, var2});
        } else if ((l == 1) && (e12.contains(one))) {
            eq_3s[eh].insert({var1, var2});
        } else {
            eq_4s[eh].insert({var1, var2});
        }
    }
}

Generator<std::tuple<Expr::Var, Expr::Var, std::vector<Predicate*>>> Table::get_all_eq_2s_and_why() {
    for (const auto& [eh, varpairs] : eq_2s) {
        for (const auto& [v1, v2] : varpairs) {
            if (is_eq_2_seen(v1, v2)) continue;
            record_eq_2_as_seen(v1, v2);
            
            Expr::Expr em = Expr::minus(M_var_to_expr[v1], M_var_to_expr[v2]);   // should be the same as eh
            Expr::strip(em);
            Expr::fix(em);
            assert(Expr::hash(em) == eh);        // only true with a Expr::strip() surrounding it

            std::vector<Predicate*> _why = why(em); 
            co_yield {v1, v2, _why};
        }
    }
    co_return;
}
Generator<std::tuple<Expr::Var, Expr::Var, Frac, std::vector<Predicate*>>> Table::get_all_eq_3s_and_why() {
    for (auto& [eh, varpairs] : eq_3s) {
        Frac f(eh.at(one));
        for (const auto& [v1, v2] : varpairs) {
            if (is_eq_3_seen(v1, v2, f)) continue;
            record_eq_3_as_seen(v1, v2, f);

            Expr::Expr em = Expr::minus(M_var_to_expr[v1], M_var_to_expr[v2]);   // should be the same as eh
            Expr::strip(em);
            Expr::fix(em);
            assert(Expr::hash(em) == eh);        // only true with a Expr::strip() surrounding it

            std::vector<Predicate*> _why = why(em);
            co_yield {v1, v2, f, _why};
        }
    }
    co_return;
}
Generator<std::tuple<Expr::Var, Expr::Var, Expr::Var, Expr::Var, std::vector<Predicate*>>> Table::get_all_eq_4s_and_why() {
    std::vector<std::pair<Expr::VarPair, Expr::VarPair>> links;
    for (const auto& [eh, varpairs] : eq_4s) {
        Table::update_equal_groups<Expr::VarPair>(equal_groups, varpairs, links);
    }
    for (const auto& [vp1, vp2] : links) {
        // Note: Any link {vp1, vp2} must have come from some EqualGroup varpairs, indexed by an
        //  expression eh. It is thus not unreasonable to say that
        // M_var_to_expr[v1] - M_var_to_expr[v2] == M_var_to_expr[v3] - M_var_to_expr[v4] == eh.
        auto& [v1, v2] = vp1;
        auto& [v3, v4] = vp2;
        if (is_eq_4_seen(v1, v2, v3, v4)) continue;
        record_eq_4_as_seen(v1, v2, v3, v4);

        Expr::Expr e12 = Expr::minus(M_var_to_expr[v1], M_var_to_expr[v2]);
        Expr::Expr e34 = Expr::minus(M_var_to_expr[v3], M_var_to_expr[v4]);
        Expr::Expr em = Expr::minus(e12, e34);  // should be zero
        Expr::strip(em);
        Expr::fix(em);
        assert(Expr::all_zeroes(em));        // only true with a Expr::strip() surrounding it

        Expr::Expr e{{v1, 1}, {v2, -1}, {v3, -1}, {v4, 1}};
        Expr::__minus(e, em);
        std::vector<Predicate*> _why = why(e);
        co_yield {v1, v2, v3, v4, _why};
    }
    co_return;
}



std::string Table::__print_A() const {
    return A.__print_matrix();
}

std::string Table::__print_M() const {
    std::string s = "M_var_to_expr:\n";
    for (const auto& [var, expr] : M_var_to_expr) {
        s += "  " + Expr::to_string(var) + " = " + Expr::to_string(expr) + "\n";
    }
    return s;
}

void Table::reset() {
    num_vars = 0;
    num_eqs = 0;
    A = SparseMatrix(0, 0, 4);
    var_to_idx.clear();
    c.clear();
    deps.clear();
    M_var_to_expr.clear();
    equal_groups.clear();
    eq_2s_seen.clear();
    eq_3s_seen.clear();
    eq_4s_seen.clear();
    eq_2s.clear();
    eq_3s.clear();
    eq_4s.clear();
}