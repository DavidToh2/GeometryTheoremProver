
#include "AREngine.hh"
#include "Table.hh"
#include "DD/DDEngine.hh"
#include "DD/Predicate.hh"
#include "Geometry/Object.hh"
#include "Geometry/Value.hh"
#include "Geometry/Object2.hh"
#include "Geometry/GeometricGraph.hh"
#include "Common/NumUtils.hh"

#define DEBUG_ARENGINE 0

#if DEBUG_ARENGINE
    #define LOG(x) do {std::cout << x << std::endl;} while(0)
#else 
    #define LOG(x)
#endif

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




void AREngine::add_constangle(
    Direction* d1, Direction* d2, float f, Predicate* pred
) {
    Frac ang = Frac(f / 180);
    Expr::Var var1 = __get_var(d1);
    Expr::Var var2 = __get_var(d2);
    angle_table.add_eq_3(var1, var2, ang.to_double(), pred);
}
void AREngine::add_eqangle(
    Direction* d1, Direction* d2, Direction* d3, Direction* d4, Predicate* pred, int pi_offset
) {
    Expr::Var var1 = __get_var(d1);
    Expr::Var var2 = __get_var(d2);
    Expr::Var var3 = __get_var(d3);
    Expr::Var var4 = __get_var(d4);
    angle_table.add_eq_4(var1, var2, var3, var4, pred, {{angle_table.one, pi_offset}});
}
void AREngine::add_para(
    Direction* d1, Direction* d2, Predicate* pred
) {
    Expr::Var var1 = __get_var(d1);
    Expr::Var var2 = __get_var(d2);
    angle_table.add_eq_2(var1, var2, 1, 1, pred);
}
void AREngine::add_perp(
    Direction* d1, Direction* d2, Predicate* pred
) {
    Expr::Var var1 = __get_var(d1);
    Expr::Var var2 = __get_var(d2);
    angle_table.add_eq_3(var1, var2, 0.5, pred);
}





void AREngine::add_constratio(
    Length* l1, Length* l2, float f, Predicate* pred
) {
    Expr::Var var1 = __get_var(l1);
    Expr::Var var2 = __get_var(l2);
    ratio_table.add_eq_3(var1, var2, std::log(f), pred);
}
void AREngine::add_constratio(
    Length* l1, Length* l2, float m, float n, Predicate* pred
) {
    Expr::Var var1 = __get_var(l1);
    Expr::Var var2 = __get_var(l2);
    ratio_table.add_eq_3(var1, var2, std::log(m/n), pred);
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



void AREngine::update_point_merger(Point* dest, Point* src, Predicate* pred) {
    for (auto& [var, disp] : var_to_displacement) {
        if (disp.p != src) continue;
        disp.p = dest;

        Expr::Var new_var = __get_var(disp);
        if (var == new_var) continue;

        LOG("--- Point merger: Updating displacement variable " << var << " to " << new_var);
        displacement_table.add_eq_2(var, new_var, 1, 1, pred);
    }
}
void AREngine::update_line_merger(Line* dest, Line* src, Predicate* pred) {
    for (auto& [var, disp] : var_to_displacement) {
        if (disp.l != src) continue;
        disp.l = dest;

        Expr::Var new_var = __get_var(disp);
        if (var == new_var) continue;

        LOG("--- Line merger: Updating displacement variable " << var << " to " << new_var);
        displacement_table.add_eq_2(var, new_var, 1, 1, pred);
    }
}
void AREngine::add_cong(
    Segment* s1, Segment* s2, Length* l1, Length* l2, Predicate* pred
) {
    Expr::Var var1 = __get_var(l1);
    Expr::Var var2 = __get_var(l2);

    auto [p1, p2] = s1->endpoints;
    auto [p3, p4] = s2->endpoints;
    Expr::Var disp_var1 = __get_var(Displacement{s1->get_line(), p1});
    Expr::Var disp_var2 = __get_var(Displacement{s1->get_line(), p2});
    Expr::Var disp_var3 = __get_var(Displacement{s2->get_line(), p3});
    Expr::Var disp_var4 = __get_var(Displacement{s2->get_line(), p4});

    ratio_table.add_eq_2(var1, var2, 1, 1, pred);
    displacement_table.add_eq_4(disp_var1, disp_var2, disp_var3, disp_var4, pred);
}
void AREngine::add_midp(
    Segment* s1, Segment* s2, Length* l1, Length* l2, Predicate* pred
) {
    Expr::Var var1 = __get_var(l1);
    Expr::Var var2 = __get_var(l2);

    auto [p1, m] = s1->endpoints;
    auto [m_, p2] = s2->endpoints;
    if (m != m_) {
        throw ARInternalError("AREngine::add_midp(): Midpoints " + m->name + ", " + m_->name + " do not coincide for both segments");
    }
    Line* l = s1->get_line();
    if (l != s2->get_line()) {
        throw ARInternalError("AREngine::add_midp(): Segments " + s1->name + ", " + s2->name + " are not on the same line");
    }

    Expr::Var disp_var1 = __get_var(Displacement{l, p1});
    Expr::Var disp_var2 = __get_var(Displacement{l, m});
    Expr::Var disp_var3 = __get_var(Displacement{l, p2});

    ratio_table.add_eq_2(var1, var2, 1, 1, pred);
    displacement_table.add_eq_4(disp_var1, disp_var2, disp_var2, disp_var3, pred);
}






Generator<std::tuple<Direction*, Direction*, double, std::vector<Predicate*>>> 
AREngine::get_all_constangles_and_why() {
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
        co_yield {d2, d1, d4, d3, _why};
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



Generator<std::tuple<Length*, Length*, double, std::vector<Predicate*>>> 
AREngine::get_all_constratios_and_why() {
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
        co_yield {l2, l1, l4, l3, _why};
    }
    co_return;
}



Generator<std::tuple<Length*, Length*, std::vector<Predicate*>>> 
AREngine::get_all_congs_and_why_1() {
    auto gen1 = ratio_table.get_all_eq_2s_and_why();
    while (gen1) {
        auto [var1, var2, _why] = gen1();
        Length* l1 = __get_length(var1);
        Length* l2 = __get_length(var2);
        co_yield {l1, l2, _why};
    }
    co_return;
}
Generator<std::tuple<Point*, Point*, Point*, Point*, std::vector<Predicate*>>> 
AREngine::get_all_congs_and_why_2() {
    auto gen2 = displacement_table.get_all_eq_4s_and_why();
    while (gen2) {
        auto [var1, var2, var3, var4, _why] = gen2();
        Displacement disp1 = __get_displacement(var1);
        Displacement disp2 = __get_displacement(var2);
        Displacement disp3 = __get_displacement(var3);
        Displacement disp4 = __get_displacement(var4);
        if (NodeUtils::same_as(disp1.l, disp2.l) && NodeUtils::same_as(disp3.l, disp4.l)
            && !NodeUtils::same_as(disp1.p, disp2.p) && !NodeUtils::same_as(disp3.p, disp4.p)
            && !(NodeUtils::same_as(disp1.l, disp3.l) && NodeUtils::same_as(disp2.l, disp4.l))) {
            co_yield {disp1.p, disp2.p, disp3.p, disp4.p, _why};
        } 
        if (NodeUtils::same_as(disp1.l, disp3.l) && NodeUtils::same_as(disp2.l, disp4.l)
            && !NodeUtils::same_as(disp1.p, disp3.p) && !NodeUtils::same_as(disp2.p, disp4.p)
            && !(NodeUtils::same_as(disp1.l, disp2.l) && NodeUtils::same_as(disp3.l, disp4.l))) {
            co_yield {disp1.p, disp3.p, disp2.p, disp4.p, _why};
        }
    }
}



void AREngine::derive(GeometricGraph& ggraph, DDEngine& dd) {

    angle_table.generate_all_eqs();

    LOG("Angle table:");
    LOG(angle_table.__print_M());

    auto gen_const_angle = get_all_constangles_and_why();
    while (gen_const_angle) {
        auto [d1, d2, f, why] = gen_const_angle();
        while (f < 0) f += 180;
        while (f >= 180) f -= 180;
        if (NumUtils::is_close(f, 90)) {
            dd.insert_predicate(
                std::make_unique<Predicate>(
                    pred_t::PERP, std::vector<Node*>{d1, d2}, std::move(why))
            );
        } else {
            dd.insert_predicate(
                std::make_unique<Predicate>(
                    pred_t::CONSTANGLE, std::vector<Node*>{d1, d2}, f, std::move(why))
            );
        }
    }

    auto gen_eqangle = get_all_eqangles_and_why();
    while (gen_eqangle) {
        auto [d1, d2, d3, d4, why] = gen_eqangle();
        dd.insert_predicate(
            std::make_unique<Predicate>(
                pred_t::EQANGLE, std::vector<Node*>{d1, d2, d3, d4}, std::move(why))
        );
    }

    auto gen_para = get_all_paras_and_why();
    while (gen_para) {
        auto [d1, d2, why] = gen_para();
        dd.insert_predicate(
            std::make_unique<Predicate>(
                pred_t::PARA, std::vector<Node*>{d1, d2}, std::move(why))
        );
    }



    ratio_table.generate_all_eqs();

    LOG("Ratio table:");
    LOG(ratio_table.__print_M());

    auto gen_cong_1 = get_all_congs_and_why_1();
    while (gen_cong_1) {
        auto [l1, l2, why] = gen_cong_1();
        dd.insert_predicate(
            std::make_unique<Predicate>(
                pred_t::CONG, std::vector<Node*>{l1, l2}, std::move(why))
        );
    }

    auto gen_const_ratio = get_all_constratios_and_why();
    while (gen_const_ratio) {
        auto [l1, l2, f, why] = gen_const_ratio();
        dd.insert_predicate(
            std::make_unique<Predicate>(
                pred_t::CONSTRATIO, std::vector<Node*>{l1, l2}, f, std::move(why))
        );
    }

    auto gen_eqratio = get_all_eqratios_and_why();
    while (gen_eqratio) {
        auto [l1, l2, l3, l4, why] = gen_eqratio();
        dd.insert_predicate(
            std::make_unique<Predicate>(
                pred_t::EQRATIO, std::vector<Node*>{l1, l2, l3, l4}, std::move(why))
        );
    }



    displacement_table.generate_all_eqs();

    LOG("Displacement table:");
    LOG(displacement_table.__print_M());

    auto gen_cong_2 = get_all_congs_and_why_2();
    while (gen_cong_2) {
        auto [p1, p2, p3, p4, why] = gen_cong_2();
        dd.insert_predicate(
            std::make_unique<Predicate>(
                pred_t::CONG, std::vector<Node*>{p1, p2, p3, p4}, std::move(why))
        );
    }
}

void AREngine::reset_problem() {
    angle_table.reset();
    ratio_table.reset();
    displacement_table.reset();

    var_to_direction.clear();
    var_to_length.clear();
    var_to_displacement.clear();
}