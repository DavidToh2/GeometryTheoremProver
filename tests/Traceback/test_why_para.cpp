
#include <doctest.h>

#include "Geometry/GeometricGraph.hh"
#include "Traceback/TracebackEngine.hh"

TEST_SUITE("TracebackEngine: why_() functions") {
    TEST_CASE("why_para()") {
        GeometricGraph ggraph;
        DDEngine dd;
        AREngine ar;
        TracebackEngine tr;
        ggraph.tr = &tr;
        Predicate* base_pred = dd.base_pred.get();

        dd.add_theorem_template_from_text("A B C : para A B A C, diff B C => coll A B C");

        Point* a = ggraph.__add_new_point("a", {0, 0});
        Point* b = ggraph.__add_new_point("b", {1, 0});
        Point* c = ggraph.__add_new_point("c", {2, 0});
        Point* d = ggraph.__add_new_point("d", {0, 1});
        Point* e = ggraph.__add_new_point("e", {1, 1});
        Point* f = ggraph.__add_new_point("f", {0, 1});
        Point* g = ggraph.__add_new_point("g", {0, 2});
        Point* h = ggraph.__add_new_point("h", {1, 2});
        Point* i = ggraph.__add_new_point("i", {2, 2});
        Point* j = ggraph.__add_new_point("j", {3, 2});
        Point* k = ggraph.__add_new_point("k", {2, 2});
        Point* l = ggraph.__add_new_point("l", {0, 4});
        Point* m = ggraph.__add_new_point("m", {1, 4});
        Point* n = ggraph.__add_new_point("n", {2, 4});
        Point* o = ggraph.__add_new_point("o", {3, 4});
        Point* p = ggraph.__add_new_point("p", {4, 4});
        Point* q = ggraph.__add_new_point("q", {0, 5});
        Point* r = ggraph.__add_new_point("r", {1, 5});
        Point* s = ggraph.__add_new_point("s", {2, 5});
        Point* t = ggraph.__add_new_point("t", {3, 5});

        std::vector<Predicate*> preds;

        Line* ab = ggraph.get_or_add_line(a, b, dd);
        Line* bc = ggraph.get_or_add_line(b, c, dd);
        Line* ef = ggraph.get_or_add_line(e, f, dd);
        Line* gh = ggraph.get_or_add_line(g, h, dd);
        Line* ij = ggraph.get_or_add_line(i, j, dd);
        Line* hk = ggraph.get_or_add_line(h, k, dd);
        Line* lm = ggraph.get_or_add_line(l, m, dd);
        Line* mn = ggraph.get_or_add_line(m, n, dd);
        Line* lo = ggraph.get_or_add_line(l, o, dd);
        Line* op = ggraph.get_or_add_line(o, p, dd);
        Line* qr = ggraph.get_or_add_line(q, r, dd);
        Line* rs = ggraph.get_or_add_line(r, s, dd);
        Line* st = ggraph.get_or_add_line(s, t, dd);
        Line* qs = ggraph.get_or_add_line(q, s, dd);
        Line* rt = ggraph.get_or_add_line(r, t, dd);
        Line* qt = ggraph.get_or_add_line(q, t, dd);

        /* Group 1 */

        preds.emplace_back(dd.insert_new_predicate(
            std::make_unique<Predicate>(
                pred_t::PARA, std::vector<Node*>{a, b, e, f})
        ));
        preds.emplace_back(dd.insert_new_predicate(
            std::make_unique<Predicate>(
                pred_t::COLL, std::vector<Node*>{a, b, c})
        ));
        ggraph.synthesise_preds(dd, ar);

        preds.emplace_back(dd.insert_new_predicate(
            std::make_unique<Predicate>(
                pred_t::BASE, std::vector<Node*>{d, f})
        ));
        ggraph.merge_points(d, f, preds.back(), dd, ar);

        REQUIRE((
            NodeUtils::get_root(ab) == bc &&
            ef->points.contains(d) && ef->points.contains(e) && ef->points.size() == 2
        ));

        Direction* dir1 = ab->get_direction();

        /* 
        0 - para A B E F
        1 - coll A B C
        2 - eq D F
        */

        PredSet why_dir1_of_bc = tr.why_direction_of(dir1, bc);
        REQUIRE((
            why_dir1_of_bc.contains(preds[1]) &&
            why_dir1_of_bc.contains(base_pred) &&   // dir1 was set for EF with base_pred
            why_dir1_of_bc.contains(preds[0]) &&    // dir1 was set for AB through EF with para A B E F
            why_dir1_of_bc.size() == 3
        ));

        PredSet why_dir1_of_ef = tr.why_direction_of(dir1, ef);
        REQUIRE((
            why_dir1_of_ef.contains(base_pred) &&   // dir1 was set for EF with base_pred
            why_dir1_of_ef.size() == 1
        ));

        /* Group 2 */

        preds.emplace_back(dd.insert_new_predicate(
            std::make_unique<Predicate>(
                pred_t::PARA, std::vector<Node*>{g, h, h, k})
        ));
        Direction* dir2_ = ggraph.get_or_add_direction(ij, dd);
        preds.emplace_back(dd.insert_new_predicate(
            std::make_unique<Predicate>(
                pred_t::PARA, std::vector<Node*>{i, j, h, k})
        ));
        ggraph.synthesise_preds(dd, ar);

        dd.search(ggraph);  // apply para H K H G => coll H K G (or coll H G K)
        REQUIRE((
            (dd.recent_predicates[0]->to_string() == "coll h k g") ||
            (dd.recent_predicates[0]->to_string() == "coll h g k")
        ));
        // We "hack" the system to force coll H K G
        dd.recent_predicates.emplace_front(dd.predicates["coll h k g"].get());
        REQUIRE(ggraph.synthesise_preds(dd, ar) == 1);

        preds.emplace_back(dd.predicates["coll h k g"].get());

        preds.emplace_back(dd.insert_new_predicate(
            std::make_unique<Predicate>(
                pred_t::BASE, std::vector<Node*>{i, k})
        ));
        ggraph.merge_points(i, k, preds.back(), dd, ar);
        REQUIRE(ggraph.synthesise_preds(dd, ar) == 0);

        dd.search(ggraph);  // apply para I J I H => coll I J H (or coll I H J, or replace J with G since IHG now collinear)
        REQUIRE((
            (dd.recent_predicates[0]->to_string() == "coll i j h") ||
            (dd.recent_predicates[0]->to_string() == "coll i h j") ||
            (dd.recent_predicates[0]->to_string() == "coll i j g") ||
            (dd.recent_predicates[0]->to_string() == "coll i g j")
        ));
        // We "hack" the system to force coll I J H
        dd.recent_predicates.emplace_front(dd.predicates["coll i j h"].get());
        REQUIRE(ggraph.synthesise_preds(dd, ar) == 1);

        preds.emplace_back(dd.predicates["coll i j h"].get());

        Line* ghij = NodeUtils::get_root(gh);
        Direction* dir2 = ghij->get_direction();    // dir2 == d_ij

        dd.search(ggraph);
        REQUIRE(ggraph.synthesise_preds(dd, ar) == 0);

        /*
        3 - para G H H K
        4 - para I J H K
        5 - coll H K G
        6 - eq I K
        7 - coll I J H
        */

        REQUIRE((
            gh == ghij &&
            NodeUtils::get_parent(hk) == ghij &&
            NodeUtils::get_parent(ij) == ghij
        ));
        REQUIRE((
            ghij->contains(i) &&
            ghij->contains(j) &&
            ghij->points.size() == 4
        ));
        REQUIRE((
            NodeUtils::get_root(k) == i &&
            i->children[0] == k
        ));

        PredSet why_dir2_of_ghij = tr.why_direction_of(dir2, ghij);
        REQUIRE((
            why_dir2_of_ghij.contains(preds[3]) &&   // para G H H K - because GHIJ is now represented by GH
            why_dir2_of_ghij.contains(preds[4]) &&   // para I J H K - resulted in dir2 <- dir_l_h_k
            why_dir2_of_ghij.contains(base_pred) &&
            why_dir2_of_ghij.size() == 3
        ));

        /* Group 3 */

        Direction* dir3_1 = ggraph.get_or_add_direction(lm, dd);
        Direction* dir3_2 = ggraph.get_or_add_direction(mn, dd);
        Direction* dir3_3 = ggraph.get_or_add_direction(lo, dd);
        Direction* dir3_4 = ggraph.get_or_add_direction(op, dd);

        preds.emplace_back(dd.insert_new_predicate(
            std::make_unique<Predicate>(
                pred_t::PARA, std::vector<Node*>{l, m, q, r})
        ));
        preds.emplace_back(dd.insert_new_predicate(
            std::make_unique<Predicate>(
                pred_t::PARA, std::vector<Node*>{m, n, s, t})
        ));
        preds.emplace_back(dd.insert_new_predicate(
            std::make_unique<Predicate>(
                pred_t::PARA, std::vector<Node*>{r, s, l, o})
        ));
        preds.emplace_back(dd.insert_new_predicate(
            std::make_unique<Predicate>(
                pred_t::PARA, std::vector<Node*>{q, t, o, p})
        ));

        preds.emplace_back(dd.insert_new_predicate(
            std::make_unique<Predicate>(
                pred_t::PARA, std::vector<Node*>{l, m, m, n})
        ));
        preds.emplace_back(dd.insert_new_predicate(
            std::make_unique<Predicate>(
                pred_t::PARA, std::vector<Node*>{l, o, o, p})
        ));
        ggraph.synthesise_preds(dd, ar);

        dd.search(ggraph);  // apply para M L M N => coll M N L and para O L O P => coll O L P
        REQUIRE((
            (dd.recent_predicates[0]->to_string() == "coll m n l" || dd.recent_predicates[1]->to_string() == "coll m n l") ||
            (dd.recent_predicates[2]->to_string() == "coll o p l" || dd.recent_predicates[3]->to_string() == "coll o l p")
        ));
        // We "hack" the system to force coll M N L and coll O P L to be synthesised in this order
        dd.recent_predicates.emplace_front(dd.predicates["coll o p l"].get());
        dd.recent_predicates.emplace_front(dd.predicates["coll m n l"].get());    // extract the correct coll predicates
        REQUIRE(ggraph.synthesise_preds(dd, ar) == 2);
        preds.emplace_back(dd.predicates["coll m n l"].get());
        preds.emplace_back(dd.predicates["coll o p l"].get());

        preds.emplace_back(dd.insert_new_predicate(
            std::make_unique<Predicate>(
                pred_t::PARA, std::vector<Node*>{o, p, m, n})
        ));
        ggraph.synthesise_preds(dd, ar);
        /* GeometricGraph::__make_para() chooses the direction of LOP, dir_3, as the new root
        direction. The direction of LMN, dir_2, is merged into dir_3. */

        PredSet why_dir3_1_of_qr = tr.why_direction_of(dir3_1, qr);
        REQUIRE((
            why_dir3_1_of_qr.contains(preds[8]) &&    // para L M Q R
            why_dir3_1_of_qr.contains(base_pred) &&   
            why_dir3_1_of_qr.size() == 2
        ));

        PredSet why_dir3_1_of_st = tr.why_direction_of(dir3_1, st);
        REQUIRE((
            why_dir3_1_of_st.contains(preds[9]) &&    // para M N S T
            why_dir3_1_of_st.contains(preds[12]) &&   // para L M M N
            why_dir3_1_of_st.contains(base_pred) &&
            why_dir3_1_of_st.size() == 3
        ));

        dd.search(ggraph);  // apply para L O L N => coll L O N (and the other 3 combinations)
        // Q, R, S, T also become collinear

        /* Here, it is entirely arbitrary (and infact indeterminate) whether coll L O N or
        coll L N O is synthesised, as we treat the logic of the DDEngine as a black box.
        The former would cause the merger LOP <- LMN; the latter would cause LMN <- LOP.
        
        We are similarly unsure exactly which two predicates are synthesised to allow Q, R, S, T
        to be deduced as collinear.
        The rest of the test case proceeds under the assumption that the aforementioned two
        predicates were synthesised in the given order. 
        
        We "hack" the system to ensure that the three predicates synthesised are:
        coll L O N, coll R S Q, coll S R T in this order
        
        Note that when coll S R T is called, the following mergers happen in order:
        1. RT <- ST(b)
        2. RST <- QRS(c) (adds diff R S)
        3. QRST <- QT(d) (adds diff T Q)
        However, the sequence in which these lines are merged into RT is indeterminate. See
        GeometricGraph::merge_lines() for more info: the lines are all dumped into a std::set
        L before being iterated on.
        Whichever line gets merged into RT first will take priority in tr.direction_line_root_map.
        For example, if RT <- QT first, then { QT, d } will appear in the root_map. */
        REQUIRE((
            std::ranges::find(dd.recent_predicates, dd.predicates["coll l o n"].get()) != dd.recent_predicates.end() &&
            std::ranges::find(dd.recent_predicates, dd.predicates["coll r s q"].get()) != dd.recent_predicates.end() &&
            std::ranges::find(dd.recent_predicates, dd.predicates["coll s r t"].get()) != dd.recent_predicates.end()
        ));
        dd.recent_predicates.emplace_front(dd.predicates["coll s r t"].get());
        dd.recent_predicates.emplace_front(dd.predicates["coll r s q"].get());
        dd.recent_predicates.emplace_front(dd.predicates["coll l o n"].get());
        REQUIRE(ggraph.synthesise_preds(dd, ar) == 3);
        preds.emplace_back(dd.predicates["coll l o n"].get());
        preds.emplace_back(dd.predicates["coll r s q"].get());
        preds.emplace_back(dd.predicates["coll s r t"].get());

        dd.search(ggraph);
        REQUIRE(ggraph.synthesise_preds(dd, ar) == 0);

        Predicate* diff_t_q = dd.predicates["diff t q"].get();
        if (!diff_t_q) {
            diff_t_q = dd.predicates["diff q t"].get();
        }

        /* 
        8 - para L M Q R
        9 - para M N S T
        10 - para R S L O
        11 - para Q T O P
        12 - para L M M N
        13 - para L O O P
        14 - coll L M N
        15 - coll L O P
        16 - para M N O P
        17 - coll L O N
        18 - coll R S Q
        19 - coll S R T
        */

        REQUIRE((
            lm->is_root() &&
            NodeUtils::get_parent(mn) == lm &&
            NodeUtils::get_parent(lo) == lm &&
            NodeUtils::get_parent(op) == lo
        ));
        REQUIRE((
            lm->contains(l) &&
            lm->contains(m) &&
            lm->contains(n) &&
            lm->contains(o) &&
            lm->contains(p) &&
            lm->points.size() == 5
        ));
        REQUIRE((
            rt->is_root() && 
            NodeUtils::get_parent(qr) == qs &&
            NodeUtils::get_parent(rs) == qs &&
            NodeUtils::get_parent(st) == rt &&
            NodeUtils::get_parent(qs) == rt &&
            NodeUtils::get_parent(qt) == rt
        ));
        REQUIRE((
            rt->contains(q) &&
            rt->contains(r) &&
            rt->contains(s) &&
            rt->contains(t) &&
            rt->points.size() == 4
        ));
        REQUIRE((
            dir3_3->is_root() &&
            NodeUtils::get_parent(dir3_2) == dir3_1 &&
            NodeUtils::get_parent(dir3_1) == dir3_3 &&
            NodeUtils::get_parent(dir3_4) == dir3_3
        ));

        PredSet why_on_l_lm = tr.why_on(l, lm);
        REQUIRE((
            why_on_l_lm.contains(base_pred) &&
            why_on_l_lm.size() == 1
        ));

        PredSet why_on_m_lm = tr.why_on(m, lm);
        REQUIRE((
            why_on_m_lm.contains(base_pred) &&
            why_on_m_lm.size() == 1
        ));

        PredSet why_on_n_lm = tr.why_on(n, lm);
        REQUIRE((
            why_on_n_lm.contains(preds[14]) &&   // coll L M N
            why_on_n_lm.contains(base_pred) &&
            why_on_n_lm.size() == 2
        ));

        PredSet why_on_o_lm = tr.why_on(o, lm);
        REQUIRE((
            why_on_o_lm.contains(preds[14]) &&   // coll L M N
            why_on_o_lm.contains(preds[17]) &&   // coll L O N
            why_on_o_lm.contains(base_pred) &&
            why_on_o_lm.size() == 3
        ));

        PredSet why_on_q_rt = tr.why_on(q, rt);
        REQUIRE((
            why_on_q_rt.contains(preds[18]) &&   // coll R S Q
            why_on_q_rt.contains(preds[19]) &&   // coll S R T
            why_on_q_rt.contains(base_pred)
            // might have diff T Q as well
        ));

        PredSet why_on_s_rt = tr.why_on(s, rt);
        REQUIRE((
            why_on_s_rt.contains(preds[19]) &&   // coll S R T
            why_on_s_rt.contains(base_pred)
        ));

        PredSet why_dir3_3_of_lm = tr.why_direction_of(dir3_3, lm);
        REQUIRE((
            why_dir3_3_of_lm.contains(preds[16]) &&   // para M N O P - explains why line LMNOP has direction dir3_3
            why_dir3_3_of_lm.contains(base_pred) &&
            why_dir3_3_of_lm.size() == 2
        ));

        PredSet why_dir3_3_of_rt = tr.why_direction_of(dir3_3, rt);
        auto dir3_3_rt_map = tr.direction_line_root_map[dir3_3][rt];
        /* Depending on the order of merger of RT <- [QRS, ST, QT], one of these lines' directions
        will take precedence in tr.direction_line_root_map */
        if (dir3_3_rt_map.second == qs) {
            REQUIRE((
                why_dir3_3_of_rt.contains(preds[10]) &&   // para R S L O - explains why line RT has direction dir3_3
                why_dir3_3_of_rt.contains(preds[18]) &&   // coll R S Q
                why_dir3_3_of_rt.contains(preds[19]) &&   // coll S R T - explains line QRST
                why_dir3_3_of_rt.contains(base_pred) &&
                why_dir3_3_of_rt.size() == 4
            ));
        } else if (dir3_3_rt_map.second == qt) {
            REQUIRE((
                why_dir3_3_of_rt.contains(preds[11]) &&   // para Q T O P - explains how d_op was assigned to QT
                why_dir3_3_of_rt.contains(preds[13]) &&   // para L O O P - explains dir3_3 <- d_op
                why_dir3_3_of_rt.contains(preds[18]) &&   // coll R S Q
                why_dir3_3_of_rt.contains(preds[19]) &&   // coll S R T
                why_dir3_3_of_rt.contains(diff_t_q) &&    // diff T Q - these explain line QT and RT <- QT
                why_dir3_3_of_rt.contains(base_pred) &&
                why_dir3_3_of_rt.size() == 6
            ));
        } else if (dir3_3_rt_map.second == st) {
            REQUIRE((
                why_dir3_3_of_rt.contains(preds[9]) &&    // para M N S T - explains how d_mn was assigned to ST
                why_dir3_3_of_rt.contains(preds[19]) &&   // coll S R T - explains RT <- ST
                why_dir3_3_of_rt.contains(preds[12]) &&   // para L M M N
                why_dir3_3_of_rt.contains(preds[16]) &&   // para M N O P - these explain dir3_3 <- d_mn
                why_dir3_3_of_rt.contains(base_pred) &&
                why_dir3_3_of_rt.size() == 5
            ));
        }

        /* Inter-group interactions
        
        20 - para J K Q S
        21 - coll D E G
        22 - coll D E H
        */

        preds.emplace_back(dd.insert_new_predicate(
            std::make_unique<Predicate>(
                pred_t::PARA, std::vector<Node*>{j, k, q, s})
        ));
        ggraph.synthesise_preds(dd, ar);

        REQUIRE((
            dir2->is_root() &&
            NodeUtils::get_parent(dir3_3) == dir2
        ));

        PredSet why_dir2_of_rt = tr.why_direction_of(dir2, rt);
        if (dir3_3_rt_map.second == st) {
            // missing para G H H K, para I J H K
            REQUIRE((
                why_dir2_of_rt.contains(preds[9]) &&    // para M N S T - explains how d_mn was assigned to ST
                why_dir2_of_rt.contains(preds[19]) &&   // coll S R T - explains RT <- ST
                why_dir2_of_rt.contains(preds[12]) &&   // para L M M N
                why_dir2_of_rt.contains(preds[16]) &&   // para M N O P - these explain dir3_3 <- d_mn
                why_dir2_of_rt.contains(preds[3]) &&    // para G H H K
                why_dir2_of_rt.contains(preds[4]) &&    // para I J H K - these explain why dir2 = d_ij was assigned to GH and IJ
                why_dir2_of_rt.contains(preds[20]) &&   // para J K Q S - explains dir2 <- dir3_3
                why_dir2_of_rt.contains(base_pred) &&
                why_dir2_of_rt.size() == 8
            ));
        } else if (dir3_3_rt_map.second == qt) {
            REQUIRE((
                why_dir2_of_rt.contains(preds[11]) &&   // para Q T O P - explains how d_op was assigned to QT
                why_dir2_of_rt.contains(preds[13]) &&   // para L O O P - explains dir3_3 <- d_op
                why_dir2_of_rt.contains(preds[18]) &&   // coll R S Q
                why_dir2_of_rt.contains(preds[19]) &&   // coll S R T
                why_dir2_of_rt.contains(diff_t_q) &&    // diff T Q - these explain line QT and RT <- QT
                why_dir2_of_rt.contains(preds[3]) &&    // para G H H K
                why_dir2_of_rt.contains(preds[4]) &&    // para I J H K - these explain why dir2 = d_ij was assigned to GH and IJ
                why_dir2_of_rt.contains(preds[20]) &&   // para J K Q S - explains dir2 <- dir3_3
                why_dir2_of_rt.contains(base_pred) &&
                why_dir2_of_rt.size() == 9
            ));
        }

        preds.emplace_back(dd.insert_new_predicate(
            std::make_unique<Predicate>(
                pred_t::COLL, std::vector<Node*>{d, e, g})
        ));
        preds.emplace_back(dd.insert_new_predicate(
            std::make_unique<Predicate>(
                pred_t::COLL, std::vector<Node*>{d, e, h})
        ));
        ggraph.synthesise_preds(dd, ar);

        REQUIRE((
            dir1->is_root() &&
            NodeUtils::get_parent(dir2) == dir1
        ));

        REQUIRE((
            ef->is_root() &&
            NodeUtils::get_parent(ghij) == ef &&
            ef->contains(g) &&
            ef->contains(h) &&
            ef->contains(i) &&
            ef->contains(j) &&
            ef->points.size() == 6
        ));
        REQUIRE((
            ggraph.root_lines.contains(bc) &&
            ggraph.root_lines.contains(ef) &&
            ggraph.root_lines.contains(lm) &&
            ggraph.root_lines.contains(rt) &&
            ggraph.root_lines.size() == 4
        ));

        /* 
        0 - para A B E F        10 - para R S L O       20 - para J K Q S
        1 - coll A B C          11 - para Q T O P       21 - coll D E G
        2 - eq D F              12 - para L M M N       22 - coll D E H
        3 - para G H H K        13 - para L O O P
        4 - para I J H K        14 - coll L M N
        5 - coll H K G          15 - coll L O P
        6 - eq I K              16 - para M N O P
        7 - coll I J H          17 - coll L O N
        8 - para L M Q R        18 - coll R S Q
        9 - para M N S T        19 - coll S R T
        */

        /* Group 1 */

        PredSet why_para_ab_ef = tr.why_para(a, b, e, f);
        REQUIRE((
            why_para_ab_ef.contains(preds[0]) &&    // para A B E F
            why_para_ab_ef.contains(base_pred) &&
            why_para_ab_ef.size() == 2
        ));

        PredSet why_para_ac_de = tr.why_para(a, c, d, e);
        REQUIRE((
            why_para_ac_de.contains(preds[0]) &&    // para A B E F
            why_para_ac_de.contains(preds[1]) &&    // coll A B C
            why_para_ac_de.contains(preds[2]) &&    // eq D F
            why_para_ac_de.contains(base_pred) &&
            why_para_ac_de.size() == 4
        ));

        /* Group 2 */

        PredSet why_para_hg_hk = tr.why_para(g, h, h, k);
        REQUIRE((
            why_para_hg_hk.contains(preds[3]) &&    // para G H H K
            why_para_hg_hk.contains(base_pred) &&
            why_para_hg_hk.size() == 2
        ));

        PredSet why_para_ij_hk = tr.why_para(i, j, h, k);
        REQUIRE((
            why_para_ij_hk.contains(preds[4]) &&    // para I J H K
            why_para_ij_hk.contains(base_pred) &&
            why_para_ij_hk.size() == 2
        ));

        PredSet why_para_ij_ih = tr.why_para(i, j, i, h);
        REQUIRE((
            why_para_ij_ih.contains(preds[4]) &&    // para I J H K
            why_para_ij_ih.contains(preds[6]) &&    // eq I K
            why_para_ij_ih.contains(base_pred) &&
            why_para_ij_ih.size() == 3
        ));

        /* Group 3 */

        PredSet why_para_lm_mn = tr.why_para(l, m, m, n);
        REQUIRE((
            why_para_lm_mn.contains(preds[12]) &&    // para L M M N
            why_para_lm_mn.contains(base_pred) &&
            why_para_lm_mn.size() == 2
        ));

        PredSet why_para_lm_st = tr.why_para(l, m, s, t);
        REQUIRE((
            why_para_lm_st.contains(preds[9]) &&    // para M N S T
            why_para_lm_st.contains(preds[12]) &&   // para L M M N
            why_para_lm_st.contains(base_pred) &&
            why_para_lm_st.size() == 3
        ));

        PredSet why_para_mn_qr = tr.why_para(m, n, q, r);
        REQUIRE((
            why_para_mn_qr.contains(preds[8]) &&    // para L M Q R
            why_para_mn_qr.contains(preds[12]) &&   // para L M M N
            why_para_mn_qr.contains(base_pred) &&
            why_para_mn_qr.size() == 3
        ));

        PredSet why_para_mn_qt = tr.why_para(m, n, q, t);
        REQUIRE((
            why_para_mn_qt.contains(preds[11]) &&   // para Q T O P
            why_para_mn_qt.contains(preds[12]) &&   // para L M M N
            why_para_mn_qt.contains(preds[13]) &&   // para L O O P
            why_para_mn_qt.contains(preds[16]) &&   // para M N O P
            why_para_mn_qt.contains(base_pred) &&
            why_para_mn_qt.size() == 5
        ));

        PredSet why_para_np_rt = tr.why_para(n, p, r, t);
        REQUIRE((
            why_para_np_rt.contains(preds[14]) &&   // coll L M N
            why_para_np_rt.contains(preds[15]) &&   // coll L O P
            why_para_np_rt.contains(preds[17]) &&   // coll L O N - explains line LMNOP
            why_para_np_rt.contains(preds[18]) &&   // coll R S Q
            why_para_np_rt.contains(preds[19]) &&   // coll S R T - explains line QRST
            why_para_np_rt.contains(preds[10]) &&   // para R S L O - explains why RT has direction d_lo 
            why_para_np_rt.contains(preds[16]) &&   // para M N O P - explains why NP has direction d_lo
            why_para_np_rt.contains(base_pred) &&
            why_para_np_rt.size() == 8
        ));

        /* Inter-group interactions */

        PredSet why_para_gh_rt = tr.why_para(g, h, r, t);
        // missing para L M M N, para O P M N, para M N S T
        REQUIRE((
            why_para_gh_rt.contains(preds[3]) &&    // para G H H K - explains why GH has direction d_hk
            why_para_gh_rt.contains(preds[4]) &&    // para I J H K - explains d_ij <- d_hk
            why_para_gh_rt.contains(preds[10]) &&   // para R S L O - explains why RS has direction d_lo
            why_para_gh_rt.contains(preds[18]) &&   // coll R S Q - explains QS <- RS
            why_para_gh_rt.contains(preds[19]) &&   // coll S R T - explains RT <- QS
            why_para_gh_rt.contains(preds[20]) &&   // para J K Q S - explains d_ij <- d_lo
            why_para_gh_rt.contains(base_pred)
        ));

        PredSet why_para_dg_np = tr.why_para(d, g, n, p);
        // missing coll S R T, para L M M N, para M N S T
        REQUIRE((
            why_para_dg_np.contains(preds[14]) &&   // coll L M N
            why_para_dg_np.contains(preds[15]) &&   // coll L O P
            why_para_dg_np.contains(preds[17]) &&   // coll L O N - these explain line LMNOP (specifically, why N, P lie on LM)
            why_para_dg_np.contains(preds[16]) &&   // para M N O P - explains d_lo <- d_lm
            why_para_dg_np.contains(preds[20]) &&   // para J K Q S - explains d_ij <- d_lo and thus d_ij <- d_lm
            why_para_dg_np.contains(preds[3]) &&    // para G H H K - explains why GH has direction d_hk
            why_para_dg_np.contains(preds[4]) &&    // para I J H K - explains d_ij <- d_hk
            why_para_dg_np.contains(preds[2]) &&    // eq D F - explains how DE has direction d_ef
            why_para_dg_np.contains(preds[21]) &&   // coll D E G
            why_para_dg_np.contains(preds[22]) &&   // coll D E H - these explain why DE == GH, and so DG has direction d_ef, and also that d_ef <- d_ij
            why_para_dg_np.contains(base_pred)
        ));

        PredSet why_para_rs_ac = tr.why_para(r, s, a, c);
        // missing coll S R T, para L M M N, para O P M N, para M N S T
        REQUIRE((
            why_para_rs_ac.contains(preds[0]) &&    // para A B E F - explains why AB has direction d_ef
            why_para_rs_ac.contains(preds[1]) &&    // coll A B C - explains why AC has direction d_ef
            why_para_rs_ac.contains(preds[3]) &&    // para G H H K
            why_para_rs_ac.contains(preds[4]) &&    // para I J H K - these explain why GH has direction d_ij
            why_para_rs_ac.contains(preds[10]) &&   // para R S L O - explains why RS has direction d_lo
            why_para_rs_ac.contains(preds[20]) &&   // para J K Q S - explains d_ij <- d_lo
            why_para_rs_ac.contains(preds[21]) &&   // coll D E G
            why_para_rs_ac.contains(preds[22]) &&   // coll D E H - these explain why DE == GH, and so d_ef <- d_ij
            why_para_rs_ac.contains(base_pred)
        ));
    }
}