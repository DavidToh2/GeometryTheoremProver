
#include "AREngine.hh"
#include "Table.hh"
#include "DD/Predicate.hh"
#include "Geometry/Object.hh"
#include "Geometry/Value.hh"
#include "Geometry/Object2.hh"
#include "Geometry/GeometricGraph.hh"

std::vector<Direction*> AREngine::__get_directions(const std::vector<Expr::Var>& vars) {
    std::vector<Direction*> result(vars.size(), nullptr);
    for (int i=0; i<vars.size(); i++) {
        result[i] = __get_direction(vars[i]);
    }
    return result;
}
std::vector<Length*> AREngine::__get_lengths(const std::vector<Expr::Var>& vars) {
    std::vector<Length*> result(vars.size(), nullptr);
    for (int i=0; i<vars.size(); i++) {
        result[i] = __get_length(vars[i]);
    }
    return result;
}

void AREngine::add_const_angle(
    Direction* d1, Direction* d2, float f, Predicate* pred
) {
    Frac ang = Frac(f / 180);
    Expr::Var var1 = __get_var(d1);
    Expr::Var var2 = __get_var(d2);
    angle_table.add_eq_3(var1, var2, ang.to_double(), pred);
}
void AREngine::add_eqangle(
    Direction* d1, Direction* d2, Direction* d3, Direction* d4, Predicate* pred
) {
    Expr::Var var1 = __get_var(d1);
    Expr::Var var2 = __get_var(d2);
    Expr::Var var3 = __get_var(d3);
    Expr::Var var4 = __get_var(d4);
    angle_table.add_eq_4(var1, var2, var3, var4, pred);
}
void AREngine::add_eqangle(
    Angle* a1, Angle* a2, Predicate* pred
) {
    add_eqangle(
        a1->direction1, a1->direction2,
        a2->direction1, a2->direction2,
        pred
    );
}
void AREngine::add_para(
    Direction* d1, Direction* d2, Predicate* pred
) {
    Expr::Var var1 = __get_var(d1);
    Expr::Var var2 = __get_var(d2);
    angle_table.add_eq_3(var1, var2, 0, pred);
}
void AREngine::add_perp(
    Direction* d1, Direction* d2, Predicate* pred
) {
    Expr::Var var1 = __get_var(d1);
    Expr::Var var2 = __get_var(d2);
    angle_table.add_eq_3(var1, var2, 0.5, pred);
}

void AREngine::add_const_ratio(
    Length* l1, Length* l2, float m, float n, Predicate* pred
) {
    Expr::Var var1 = __get_var(l1);
    Expr::Var var2 = __get_var(l2);
    ratio_table.add_eq_2(var1, var2, m, n, pred);
}
void AREngine::add_eqratio(
    Length* l1, Length* l2, Length* l3, Length* l4, Predicate* pred
) {
    Expr::Var var1 = __get_var(l1);
    Expr::Var var2 = __get_var(l2);
    Expr::Var var3 = __get_var(l3);
    Expr::Var var4 = __get_var(l4);
    ratio_table.add_eq_4(var1, var2, var3, var4, pred);
}
void AREngine::add_cong(
    Length* l1, Length* l2, Predicate* pred
) {
    Expr::Var var1 = __get_var(l1);
    Expr::Var var2 = __get_var(l2);
    ratio_table.add_eq_2(var1, var2, 1, 1, pred);
}


Generator<std::tuple<Direction*, Direction*, float, std::vector<Predicate*>>> 
AREngine::get_all_const_angles_and_why() {
    auto gen = angle_table.get_all_eq_3s_and_why();
    while (gen) {
        auto [var1, var2, f, _why] = gen();
        Direction* d1 = __get_direction(var1);
        Direction* d2 = __get_direction(var2);
        co_yield {d1, d2, f.to_double() * 180, _why};
    }
    co_return;
}
Generator<std::tuple<Direction*, Direction*, Direction*, Direction*, std::vector<Predicate*>>> 
AREngine::get_all_eqangles_and_why() {
    auto gen = angle_table.get_all_eq_4s_and_why();
    while (gen) {
        auto [var1, var2, var3, var4, _why] = gen();
        Direction* d1 = __get_direction(var1);
        Direction* d2 = __get_direction(var2);
        Direction* d3 = __get_direction(var3);
        Direction* d4 = __get_direction(var4);
        co_yield {d1, d2, d3, d4, _why};
    }
    co_return;
}
Generator<std::tuple<Direction*, Direction*, std::vector<Predicate*>>> 
AREngine::get_all_paras_and_why() {
    auto gen = angle_table.get_all_eq_2s_and_why();
    while (gen) {
        auto [var1, var2, _why] = gen();
        Direction* d1 = __get_direction(var1);
        Direction* d2 = __get_direction(var2);
        co_yield {d1, d2, _why};
    }
    co_return;
}

Generator<std::tuple<Length*, Length*, float, std::vector<Predicate*>>> 
AREngine::get_all_const_ratios_and_why() {
    auto gen = ratio_table.get_all_eq_3s_and_why();
    while (gen) {
        auto [var1, var2, f, _why] = gen();
        Length* l1 = __get_length(var1);
        Length* l2 = __get_length(var2);
        co_yield {l1, l2, f.to_double(), _why};
    }
    co_return;
}
Generator<std::tuple<Length*, Length*, Length*, Length*, std::vector<Predicate*>>>
AREngine::get_all_eqratios_and_why() {
    auto gen = ratio_table.get_all_eq_4s_and_why();
    while (gen) {
        auto [var1, var2, var3, var4, _why] = gen();
        Length* l1 = __get_length(var1);
        Length* l2 = __get_length(var2);
        Length* l3 = __get_length(var3);
        Length* l4 = __get_length(var4);
        co_yield {l1, l2, l3, l4, _why};
    }
    co_return;
}
Generator<std::tuple<Length*, Length*, std::vector<Predicate*>>> 
AREngine::get_all_congs_and_why() {
    auto gen = ratio_table.get_all_eq_2s_and_why();
    while (gen) {
        auto [var1, var2, _why] = gen();
        Length* l1 = __get_length(var1);
        Length* l2 = __get_length(var2);
        co_yield {l1, l2, _why};
    }
    co_return;
}

void AREngine::derive(GeometricGraph& ggraph) {
    // To be implemented
    
}