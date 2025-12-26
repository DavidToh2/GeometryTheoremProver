
#include "Table.hh"
#include "Numerics/Matrix.hh"
#include "Numerics/Numerics.hh"
#include "Common/Exceptions.hh"

double Expr::fix_v(const double d) {
    return Frac(d).to_double();
}
void Expr::fix(Expr& expr) {
    for (auto& [var, coeff] : expr) {
        coeff = fix_v(coeff);
    }
}
void Expr::strip(Expr& expr) {
    for (auto it = expr.cbegin(); it != expr.cend(); ) {
        if (std::abs(it->second) < Frac::TOL) {
            it = expr.erase(it);
        } else {
            ++it;
        }
    }
}
bool Expr::all_zeroes(const Expr& expr) {
    for (const auto& [var, coeff] : expr) {
        if (std::abs(coeff) >= Frac::TOL) {
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
Expr::Expr Expr::add_fold(const std::vector<Expr>& exprs) {
    Expr result = {};
    for (const auto& expr : exprs) {
        __add(result, expr);
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
    for (auto it = result.rbegin(); it != result.rend(); ++it) {
        const auto& [var, coeff] = *it;
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



bool Table::add_free(const Expr::Var& var_name) {
    var_to_expr[var_name] = {{var_name, 1}};
    return true;
}
bool Table::add_expr(const Expr::Expr& expr) {
    // find new variables
    std::vector<std::pair<Expr::Var, double>> new_vars;
    Expr::Expr result;

    for (const auto& [var, d] : expr) {
        if (var_to_expr.contains(var)) {
            Expr::__add(result, Expr::mult(var_to_expr[var], d));
        } else {
            new_vars.push_back({var, d});
        }
    }

    if (new_vars.size() == 0) {
        if (Expr::all_zeroes(expr)) return false;
        auto [subject, expr_subj] = Expr::get_subject(expr, one);
        if (subject.empty()) return false;
        replace(subject, expr_subj);

    } else if (new_vars.size() == 1) {
        auto [var, d] = new_vars[0];
        var_to_expr[var] = Expr::div(result, -d);

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
        var_to_expr[dependent_var] = Expr::div(result, -dependent_d);
    }

    return true;
}

void Table::replace(const Expr::Var& var, const Expr::Expr& sub_expr) {
    for (auto& [_, expr] : var_to_expr) {
        Expr::__replace(expr, var, sub_expr);
    }
}

bool Table::register_expr(const Expr::Expr& expr, Predicate* pred) {
    if (Expr::all_zeroes(expr)) {
        return false;
    }
    for (const auto& [var, _] : expr) {
        if (!var_to_expr.contains(var)) {
            var_to_row[var] = num_vars++;
        }
    }
    if (num_vars > A.m) {
        A.extend_rows(num_vars - A.m);
    }
    Matrix new_columns = Matrix(num_vars, 2);
    for (const auto& [var, coeff] : expr) {
        new_columns(var_to_row[var], 0) = coeff;
        new_columns(var_to_row[var], 1) = -coeff;
    }
    A.extend_columns(new_columns);
    c.emplace_back(1);
    c.emplace_back(-1);
    deps.push_back(pred);
    return true;
}


bool Table::register_2(const Expr::Var& var1, const Expr::Var& var2, float m, float n, Predicate* pred) {
    return register_expr({{var1, m}, {var2, -n}}, pred);
}
bool Table::register_3(const Expr::Var& var1, const Expr::Var& var2, float f, Predicate* pred) {
    return register_expr({{var1, 1}, {var2, -1}, {one, -f}}, pred);
}
bool Table::register_4(const Expr::Var& var1, const Expr::Var& var2, const Expr::Var& var3, const Expr::Var& var4, Predicate* pred) {
    return register_expr({{var1, 1}, {var2, -1}, {var3, -1}, {var4, 1}}, pred);
}


bool Table::record_eq_2_as_seen(const Expr::VarPair& vp) {
    return eq_2s_seen.insert(vp).second;
}
bool Table::record_eq_3_as_seen(const Expr::VarPair& vp, const Frac f) {
    return eq_3s_seen.insert({vp.first, vp.second, f}).second;
}
bool Table::record_eq_4_as_seen(const Expr::VarPair& vp1, const Expr::VarPair& vp2) {
    return eq_4s_seen.insert({vp1, vp2}).second;
}
bool Table::is_eq_4_seen(const Expr::Var var1, const Expr::Var var2, const Expr::Var var3, const Expr::Var var4) {
    return (
        eq_4s_seen.contains({{var1, var2}, {var3, var4}}) 
        || eq_4s_seen.contains({{var2, var1}, {var4, var3}})
        || eq_4s_seen.contains({{var3, var4}, {var1, var2}})
        || eq_4s_seen.contains({{var4, var3}, {var2, var1}})
    );
}


bool Table::add_eq_2(const Expr::Var& var1, const Expr::Var& var2, float m, float n, Predicate* pred) {
    return (add_expr({{var1, m}, {var2, -n}}) && register_2(var1, var2, m, n, pred));
}
bool Table::add_eq_3(const Expr::Var& var1, const Expr::Var& var2, float f, Predicate* pred) {
    return (add_expr({{var1, 1}, {var2, -1}, {one, -f}}) && register_3(var1, var2, f, pred));
}
bool Table::add_eq_4(const Expr::Var& var1, const Expr::Var& var2, const Expr::Var& var3, const Expr::Var& var4, Predicate* pred) {
    return (add_expr({{var1, 1}, {var2, -1}, {var3, -1}, {var4, 1}}) && register_4(var1, var2, var3, var4, pred));
}



std::vector<Predicate*> Table::why(const Expr::Expr& expr) {
    std::vector<Predicate*> result;
    return result;

    Expr::Expr target = expr;
    Expr::strip(target);
    Expr::fix(target);

    Matrix b(num_vars, 1);
    for (const auto& [var, coeff] : target) {
        if (!var_to_row.contains(var)) {
            throw ARInternalError("Cannot explain expression with unknown variable: " + var);
        }
        b(var_to_row[var], 0) = -coeff;
    }

    Matrix A_ext = A;
    A_ext.extend_columns(b);

    // Solve the linear program min c^T * x subject to A_ext * x = 0
    Matrix solution(1, 1);

    for (int i = 0; i < deps.size(); i++) {
        if (std::abs(solution(i, 0)) > Frac::TOL) {
            result.push_back(deps[i]);
        }
    }
    return result;
}



Generator<Expr::VarPair> Table::all_varpairs() const {
    for (const auto& [var1, _] : var_to_expr) {
        if (var1 == one) continue;
        for (const auto& [var2, _] : var_to_expr) {
            if (var2 == one) continue;
            co_yield {var1, var2};
        }
    }
    co_return;
}

void Table::get_all_eqs() {
    eq_2s.clear();
    eq_3s.clear();
    eq_4s.clear();

    auto all_varpairs_gen = all_varpairs();
    while (all_varpairs_gen) {
        auto [var1, var2] = all_varpairs_gen();
        Expr::Expr e1 = var_to_expr[var1], e2 = var_to_expr[var2];
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
    for (const auto& [_, varpairs] : eq_2s) {
        for (const auto& [v1, v2] : varpairs) {
            if (eq_2s_seen.contains({v1, v2}) || eq_2s_seen.contains({v2, v1})) continue;
            record_eq_2_as_seen({v1, v2});
            
            std::vector<Predicate*> _why = why(Expr::minus(var_to_expr[v1], var_to_expr[v2]));
            co_yield {v1, v2, _why};
        }
    }
    co_return;
}
Generator<std::tuple<Expr::Var, Expr::Var, Frac, std::vector<Predicate*>>> Table::get_all_eq_3s_and_why() {
    for (auto& [h, varpairs] : eq_3s) {
        Frac f(h.at(one));
        for (const auto& [v1, v2] : varpairs) {
            if (eq_3s_seen.contains({v1, v2, f})) continue;
            record_eq_3_as_seen({v1, v2}, f);

            Expr::Expr e = Expr::minus(var_to_expr[v1], var_to_expr[v2]);
            std::vector<Predicate*> _why = why(e);
            co_yield {v1, v2, f, _why};
        }
    }
    co_return;
}
Generator<std::tuple<Expr::Var, Expr::Var, Expr::Var, Expr::Var, std::vector<Predicate*>>> Table::get_all_eq_4s_and_why() {
    std::vector<std::pair<Expr::VarPair, Expr::VarPair>> links;
    for (const auto& [_, varpairs] : eq_4s) {
        Table::update_equal_groups<Expr::VarPair>(equal_groups, varpairs, links);
    }
    for (const auto& [vp1, vp2] : links) {
        auto& [v1, v2] = vp1;
        auto& [v3, v4] = vp2;
        if (is_eq_4_seen(v1, v2, v3, v4)) continue;
        record_eq_4_as_seen(vp1, vp2);

        Expr::Expr e12 = Expr::minus(var_to_expr[v1], var_to_expr[v2]);
        Expr::Expr e34 = Expr::minus(var_to_expr[v3], var_to_expr[v4]);
        Expr::Expr e{{v1, 1}, {v2, -1}, {v3, -1}, {v4, 1}};
        Expr::__minus(e, Expr::minus(e12, e34));
        std::vector<Predicate*> _why = why(e);
        co_yield {v1, v2, v3, v4, _why};
    }
    co_return;
}