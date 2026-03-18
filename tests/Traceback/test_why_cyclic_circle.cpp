
#include <doctest.h>

#include "Geometry/GeometricGraph.hh"
#include "Traceback/TracebackEngine.hh"

TEST_SUITE("TracebackEngine: why_() functions") {
    TEST_CASE("why_cyclic() and why_circle()") {
        GeometricGraph ggraph;
        DDEngine dd;
        AREngine ar;
        TracebackEngine tr;
        ggraph.tr = &tr;
        Predicate* base_pred = dd.base_pred.get();

        Point* a = ggraph.__add_new_point("a", {-1, 0});
        Point* b = ggraph.__add_new_point("b", {-1.0 / std::sqrt(2), 1.0 / std::sqrt(2)});
        Point* c = ggraph.__add_new_point("c", {0, 1});
        Point* d = ggraph.__add_new_point("d", {1.0 / std::sqrt(2), 1.0 / std::sqrt(2)});
        Point* e = ggraph.__add_new_point("e", {1, 0});
        Point* f = ggraph.__add_new_point("f", {-1, 0});
        Point* g = ggraph.__add_new_point("g", {-1.0 / std::sqrt(2), 1.0 / std::sqrt(2)});
        Point* h = ggraph.__add_new_point("h", {0, 1});
        Point* i = ggraph.__add_new_point("i", {1.0 / std::sqrt(2), 1.0 / std::sqrt(2)});
        Point* j = ggraph.__add_new_point("j", {1, 0});
        Point* k = ggraph.__add_new_point("k", {-1, 0});
        Point* l = ggraph.__add_new_point("l", {-1.0 / std::sqrt(2), 1.0 / std::sqrt(2)});
        Point* m = ggraph.__add_new_point("m", {0, 1});
        Point* n = ggraph.__add_new_point("n", {1.0 / std::sqrt(2), 1.0 / std::sqrt(2)});
        Point* o = ggraph.__add_new_point("o", {1, 0});

        Point* w = ggraph.__add_new_point("w", {0, 0});
        Point* x = ggraph.__add_new_point("x", {0, 0});
        Point* y = ggraph.__add_new_point("y", {0, 0});
        Point* z = ggraph.__add_new_point("z", {0, 0});

        Circle* abc = ggraph.__add_new_circle(a, b, c, base_pred);
        Circle* bcd = ggraph.__add_new_circle(b, c, d, base_pred);
        Circle* abh = ggraph.__add_new_circle(a, b, h, base_pred);
        Circle* bhd = ggraph.__add_new_circle(b, h, d, base_pred);
        Circle* fbi = ggraph.__add_new_circle(f, b, i, base_pred);
        Circle* fmn = ggraph.__add_new_circle(f, m, n, base_pred);
        Circle* gce = ggraph.__add_new_circle(g, c, e, base_pred);
        Circle* fgi = ggraph.__add_new_circle(f, g, i, base_pred);
        Circle* kln = ggraph.__add_new_circle(k, l, n, base_pred);
        Circle* lmo = ggraph.__add_new_circle(l, m, o, base_pred);

        std::vector<Predicate*> preds;

        preds.emplace_back(dd.insert_new_predicate(
            std::make_unique<Predicate>(
                pred_t::CIRCLE, std::vector<Node*>{w, b, c, d})
        ));
        preds.emplace_back(dd.insert_new_predicate(
            std::make_unique<Predicate>(
                pred_t::CYCLIC, std::vector<Node*>{d, c, b, a})
        ));
        preds.emplace_back(dd.insert_new_predicate(
            std::make_unique<Predicate>(
                pred_t::CYCLIC, std::vector<Node*>{a, b, h, i})
        ));
        preds.emplace_back(dd.insert_new_predicate(
            std::make_unique<Predicate>(
                pred_t::CYCLIC, std::vector<Node*>{b, h, d, j})
        ));
        preds.emplace_back(dd.insert_new_predicate(
            std::make_unique<Predicate>(
                pred_t::CYCLIC, std::vector<Node*>{f, b, i, e})
        ));
        preds.emplace_back(dd.insert_new_predicate(
            std::make_unique<Predicate>(
                pred_t::CIRCLE, std::vector<Node*>{x, f, b, i})
        ));
        preds.emplace_back(dd.insert_new_predicate(
            std::make_unique<Predicate>(
                pred_t::CYCLIC, std::vector<Node*>{f, m, n, e})
        ));
        preds.emplace_back(dd.insert_new_predicate(
            std::make_unique<Predicate>(
                pred_t::CIRCLE, std::vector<Node*>{y, g, c, e})
        ));
        preds.emplace_back(dd.insert_new_predicate(
            std::make_unique<Predicate>(
                pred_t::CIRCLE, std::vector<Node*>{z, f, g, i})
        ));
        preds.emplace_back(dd.insert_new_predicate(
            std::make_unique<Predicate>(
                pred_t::CYCLIC, std::vector<Node*>{k, l, n, o})
        ));
        preds.emplace_back(dd.insert_new_predicate(
            std::make_unique<Predicate>(
                pred_t::CIRCLE, std::vector<Node*>{x, k, l, n})
        ));

        /* 
        1 - cyclic A B C D,      0 - circle W B C D, 
        2 - cyclic A B H I,      5 - circle X F B I,
        3 - cyclic B H D J,      7 - circle Y G C E,
        4 - cyclic F B I E,      8 - circle Z F G I, 
        6 - cyclic F M N E,      10 - circle X K L N
        9 - cyclic K L N O */
        ggraph.synthesise_preds(dd, ar);

        REQUIRE(ggraph.root_circles.size() == 9);

        PredSet why_on_d_abc = tr.why_on(d, abc);
        REQUIRE((
            why_on_d_abc.size() == 2 &&
            why_on_d_abc.contains(base_pred) &&  // because d belongs to bcd which was merged into abc
            why_on_d_abc.contains(preds[1])      // cyclic A B C D
        ));

        PredSet why_on_i_abh = tr.why_on(i, abh);
        REQUIRE((
            why_on_i_abh.size() == 1 &&
            why_on_i_abh.contains(preds[2])      // cyclic A B H I
        ));

        PredSet why_cyclic_abhi = tr.why_cyclic(a, b, h, i);
        REQUIRE((
            why_cyclic_abhi.size() == 2 &&
            why_cyclic_abhi.contains(base_pred) &&
            why_cyclic_abhi.contains(preds[2])      // cyclic A B H I
        ));

        PredSet why_circle_z_fgi = tr.why_circle(z, f, g, i);
        REQUIRE((
            why_circle_z_fgi.size() == 2 &&
            why_circle_z_fgi.contains(base_pred) &&
            why_circle_z_fgi.contains(preds[8])      // circle Z F G I
        ));

        PredSet why_circle_w_abc = tr.why_circle(w, a, b, c);
        REQUIRE((
            why_circle_w_abc.size() == 3 &&
            why_circle_w_abc.contains(base_pred) &&
            why_circle_w_abc.contains(preds[0]) &&   // circle W B C D
            why_circle_w_abc.contains(preds[1])      // cyclic A B C D
        ));

        // 11 - cyclic B H I J
        preds.emplace_back(dd.insert_new_predicate(
            std::make_unique<Predicate>(
                pred_t::CYCLIC, std::vector<Node*>{b, h, i, j})
        ));
        ggraph.synthesise_preds(dd, ar);

        /* C, D belong to circle ABCD; H, I to circle ABHI; these two circles have least common ancestor BHDJ
        ABHI -> BHDJ because 
            [ cyclic BHIJ + why_on(ABHI, {B, H, I}) + why_on(BHDJ, {B, H, J}) ]
        ABCD -> BHDJ because the circle merger algorithm identified that ABCD has A, B in common with ABHI and 
        B, D in common with BHDJ, so we have 
            [ cyclic BHIJ + why_on(ABCD, {A, B}) + why_on(ABHI, {A, B}) + why_on(ABCD, {B, D}) 
            + why_on(BHDJ, {B, D}) + diff A B + diff B D ]*/

        Circle* abcdhij = NodeUtils::get_root(abc);
        REQUIRE((
            abcdhij == bhd &&
            NodeUtils::get_root(abh) == abcdhij &&
            NodeUtils::get_root(bhd) == abcdhij &&
            ggraph.get_or_add_circle(c, i, j, dd) == abcdhij &&
            abcdhij->get_center() == w
        ));

        Predicate* diff_a_b = dd.predicates["diff a b"].get();
        Predicate* diff_b_d = dd.predicates["diff b d"].get();

        PredSet why_on_i_bhd = tr.why_on(i, bhd);
        REQUIRE((
            why_on_i_bhd.contains(preds[2]) &&    // cyclic A B H I
            why_on_i_bhd.contains(preds[3]) &&    // cyclic B H D J
            why_on_i_bhd.contains(preds[11]) &&   // cyclic B H I J
            why_on_i_bhd.contains(base_pred) &&
            why_on_i_bhd.size() == 4
        ));

        PredSet why_on_c_bhd = tr.why_on(c, bhd);
        REQUIRE((
            why_on_c_bhd.contains(preds[1]) &&    // cyclic A B C D
            why_on_c_bhd.contains(preds[2]) &&    // cyclic A B H I
            why_on_c_bhd.contains(preds[3]) &&    // cyclic B H D J
            why_on_c_bhd.contains(preds[11]) &&   // cyclic B H I J
            why_on_c_bhd.contains(diff_b_d) &&
            why_on_c_bhd.contains(diff_a_b) &&    // diff predicates for the formation of ABCDHIJ
            why_on_c_bhd.contains(base_pred) &&
            why_on_c_bhd.size() == 7
        ));

        PredSet why_eq_bhd_abh = TracebackUtils::why_ancestor(abh, bhd);
        REQUIRE((
            why_eq_bhd_abh.size() == 4 &&
            why_eq_bhd_abh.contains(preds[2]) &&    // cyclic A B H I
            why_eq_bhd_abh.contains(preds[3]) &&    // cyclic B H D J
            why_eq_bhd_abh.contains(preds[11]) &&   // cyclic B H I J
            why_eq_bhd_abh.contains(base_pred)
        ));

        PredSet why_eq_bhd_abc = TracebackUtils::why_ancestor(abc, bhd);
        REQUIRE((
            why_eq_bhd_abc.size() == 7 &&
            why_eq_bhd_abc.contains(preds[1]) &&    // cyclic A B C D
            why_eq_bhd_abc.contains(preds[2]) &&    // cyclic A B H I
            why_eq_bhd_abc.contains(preds[3]) &&    // cyclic B H D J
            why_eq_bhd_abc.contains(preds[11]) &&   // cyclic B H I J
            why_eq_bhd_abc.contains(diff_a_b) &&    // diff A B
            why_eq_bhd_abc.contains(diff_b_d) &&    // diff B D
            why_eq_bhd_abc.contains(base_pred)
        ));

        // 12 - cyclic L M N O
        preds.emplace_back(dd.insert_new_predicate(
            std::make_unique<Predicate>(
                pred_t::CYCLIC, std::vector<Node*>{l, m, n, o})
        ));
        ggraph.synthesise_preds(dd, ar);

        Circle* klmno = NodeUtils::get_root(kln);
        REQUIRE((
            klmno == kln &&
            NodeUtils::get_root(lmo) == klmno &&
            ggraph.get_or_add_circle(l, m, n, dd) == klmno &&
            klmno->get_center() == x
        ));

        // 13 - eq I N
        preds.emplace_back(dd.insert_new_predicate(
            std::make_unique<Predicate>(
                pred_t::BASE, std::vector<Node*>{i, n})
        ));
        ggraph.merge_points(i, n, preds.back(), dd, ar);

        Circle* befiklmno = NodeUtils::get_root(fbi);
        REQUIRE((
            befiklmno == fbi &&
            NodeUtils::get_root(fmn) == befiklmno &&
            NodeUtils::get_root(kln) == befiklmno &&
            NodeUtils::get_root(lmo) == befiklmno &&
            befiklmno->get_center() == x
        ));

        PredSet why_on_b_fbi = tr.why_on(b, fbi);
        REQUIRE((
            why_on_b_fbi.size() == 1 &&
            why_on_b_fbi.contains(base_pred)
        ));

        PredSet why_on_m_fbi = tr.why_on(m, fbi);
        REQUIRE(((
            why_on_m_fbi.contains(preds[5]) &&    // cyclic X F B I
            why_on_m_fbi.contains(preds[10]) &&   // circle X K L N
            why_on_m_fbi.contains(preds[9]) &&    // cyclic K L N O
            why_on_m_fbi.contains(preds[12]) &&   // cyclic L M N O
            why_on_m_fbi.contains(preds[13]) &&   // eq I N
            why_on_m_fbi.contains(base_pred)
        ) || (
            why_on_m_fbi.contains(preds[4]) &&    // cyclic F B I E
            why_on_m_fbi.contains(preds[6]) &&    // cyclic F M N E
            why_on_m_fbi.contains(preds[13]) &&   // eq I N
            why_on_m_fbi.contains(base_pred)
        )));
        // Two possible chains of reasoning, indeterminate which is chosen

        PredSet why_on_k_fbi = tr.why_on(k, fbi);
        REQUIRE((
            why_on_k_fbi.contains(preds[5]) &&    // circle X F B I
            why_on_k_fbi.contains(preds[10]) &&   // circle X K L N
            why_on_k_fbi.contains(preds[13]) &&   // eq I N
            why_on_k_fbi.contains(base_pred) &&
            why_on_k_fbi.size() == 4
        ));

        PredSet why_eq_fbi_fmn = TracebackUtils::why_ancestor(fmn, fbi);
        REQUIRE((
            why_eq_fbi_fmn.size() == 4 &&
            why_eq_fbi_fmn.contains(base_pred) &&
            why_eq_fbi_fmn.contains(preds[4]) &&   // cyclic F B I E
            why_eq_fbi_fmn.contains(preds[6]) &&   // cyclic F M N E
            why_eq_fbi_fmn.contains(preds[13])     // eq I N
        ));

        PredSet why_eq_fbi_kln = TracebackUtils::why_ancestor(kln, fbi);
        REQUIRE((
            why_eq_fbi_kln.size() == 3 &&
            why_eq_fbi_kln.contains(preds[5]) &&   // circle X F B I
            why_eq_fbi_kln.contains(preds[10]) &&  // circle X K L N
            why_eq_fbi_kln.contains(preds[13])     // eq I N
        ));     // no base_pred because base_pred only appears for why_on(circle, point)

        /* At this point, we have four circles:
        - ABCDHIJ (W)
        - BEFIKLMO (X) 
        - GCE (Y)
        - FGI (Z) 
        When Y and Z are merged,
        1. GCE <- FGI are first merged due to their common point G, forming CEFGI (Y)
        2. CEFGI <- BEFIKLMO are then merged by the circle merging algorithm due to the 
        common points E in GCE; F, I in FGI. This forms BCEFGIKLMO. This also adds the 
        predicate diff F I
        3. BCEFGIKLMO <- ABCDHIJ are then merged by the circle merging algorithm due to 
        the common points C in GCE; B, I in BEFIKLMO. This forms our final circle. 
        This also adds the predicate diff B I
        4. Step 2 also triggers the merge Y <- X, and step 4 the merge Y <- W.  */

        // 14 - eq Y Z
        preds.emplace_back(dd.insert_new_predicate(
            std::make_unique<Predicate>(
                pred_t::BASE, std::vector<Node*>{y, z})
        ));
        ggraph.merge_points(y, z, preds.back(), dd, ar);

        Circle* final = NodeUtils::get_root(gce);
        REQUIRE((
            final == gce &&
            NodeUtils::get_root(fgi) == final &&
            NodeUtils::get_root(kln) == final &&
            NodeUtils::get_root(lmo) == final &&
            NodeUtils::get_root(abcdhij) == final &&
            NodeUtils::get_root(befiklmno) == final
        ));

        Point* center = final->get_center();
        REQUIRE((
            y == center &&
            NodeUtils::get_root(w) == center &&
            NodeUtils::get_root(x) == center
        ));

        Predicate* diff_b_i = dd.predicates["diff b i"].get();
        Predicate* diff_f_i = dd.predicates["diff f i"].get();

        PredSet why_eq_y_x = TracebackUtils::why_ancestor(x, y);
        REQUIRE((
            why_eq_y_x.contains(preds[14]) &&   // eq Y Z
            why_eq_y_x.contains(preds[7]) &&    // circle Y G C E
            why_eq_y_x.contains(preds[8]) &&    // circle Z F G I - up to this point, we have explained CEFGI(Y)
            why_eq_y_x.contains(preds[5]) &&    // circle X F B I
            why_eq_y_x.contains(preds[4]) &&    // cyclic F B I E - explains why FBIE(X) was merged with CEFGI(Y) to get BCEFGI
            why_eq_y_x.contains(diff_f_i) &&    // diff predicate for merging CEFGI(Y) <- FBIE(X)
            why_eq_y_x.contains(base_pred) &&
            why_eq_y_x.size() == 7
        ));

        PredSet why_eq_y_w = TracebackUtils::why_ancestor(w, y);
        REQUIRE((
            why_eq_y_w.contains(preds[14]) &&   // eq Y Z
            why_eq_y_w.contains(preds[7]) &&    // circle Y G C E
            why_eq_y_w.contains(preds[8]) &&    // circle Z F G I - up to this point, we have explained CEFGI(Y)
            why_eq_y_w.contains(preds[4]) &&    // cyclic F B I E - explains why FBIE was merged with CEFGI(Y) to get BCEFGI
            why_eq_y_w.contains(diff_f_i) &&    // diff predicate for merging CEFGI(Y) <- FBIE
            why_eq_y_w.contains(preds[1]) &&    // cyclic A B C D
            why_eq_y_w.contains(preds[2]) &&    // cyclic A B H I
            why_eq_y_w.contains(preds[3]) &&    // cyclic B H D J
            why_eq_y_w.contains(preds[11]) &&   // cyclic B H I J
            why_eq_y_w.contains(preds[0]) &&    // circle W B C D - explains the formation of ABCDHIJ(W)
            why_eq_y_w.contains(diff_b_d) &&
            why_eq_y_w.contains(diff_a_b) &&    // diff predicates for the formation of ABCDHIJ(W)
            why_eq_y_w.contains(diff_b_i) &&    // diff predicate for merging BCEFGI(Y) <- ABCDHIJ(W)
            why_eq_y_w.contains(base_pred) &&
            why_eq_y_w.size() == 14
        ));

        // ------------ after 11 - cyclic B H I J

        PredSet why_cyclic_abhj = tr.why_cyclic(a, b, h, j);
        REQUIRE((
            why_cyclic_abhj.size() == 4 &&
            why_cyclic_abhj.contains(base_pred) &&
            why_cyclic_abhj.contains(preds[2]) &&     // cyclic A B H I
            why_cyclic_abhj.contains(preds[3]) &&     // cyclic B H D J
            why_cyclic_abhj.contains(preds[11])       // cyclic B H I J
        ));

        PredSet why_cyclic_adij = tr.why_cyclic(a, d, i, j);
        REQUIRE((
            why_cyclic_adij.size() == 4 &&
            why_cyclic_adij.contains(base_pred) &&
            why_cyclic_adij.contains(preds[2]) &&     // cyclic A B H I
            why_cyclic_adij.contains(preds[3]) &&     // cyclic B H D J
            why_cyclic_adij.contains(preds[11])       // cyclic B H I J
        ));

        PredSet why_cyclic_cdhi = tr.why_cyclic(c, d, h, i);
        REQUIRE((
            why_cyclic_cdhi.size() == 7 &&
            why_cyclic_cdhi.contains(base_pred) &&
            why_cyclic_cdhi.contains(preds[1]) &&     // cyclic A B C D
            why_cyclic_cdhi.contains(preds[2]) &&     // cyclic A B H I
            why_cyclic_cdhi.contains(preds[3]) &&     // cyclic B H D J
            why_cyclic_cdhi.contains(preds[11]) &&    // cyclic B H I J
            why_cyclic_cdhi.contains(diff_a_b) &&
            why_cyclic_cdhi.contains(diff_b_d)
        ));

        PredSet why_cyclic_bcij = tr.why_cyclic(b, c, i, j);
        /* Similar analysis as above applies. B belongs to all three circles; C only to ABCD; I only to ABHI;
        J only to BHDJ */
        REQUIRE((
            why_cyclic_bcij.size() == 7 &&
            why_cyclic_bcij.contains(base_pred) &&
            why_cyclic_bcij.contains(preds[1]) &&     // cyclic A B C D
            why_cyclic_bcij.contains(preds[2]) &&     // cyclic A B H I
            why_cyclic_bcij.contains(preds[3]) &&     // cyclic B H D J
            why_cyclic_bcij.contains(preds[11]) &&    // cyclic B H I J
            why_cyclic_bcij.contains(diff_a_b) &&
            why_cyclic_bcij.contains(diff_b_d)
        ));

        // ------------ after 12 - cyclic L M N O

        PredSet why_cyclic_lmno = tr.why_cyclic(l, m, n, o);
        REQUIRE((
            why_cyclic_lmno.size() == 3 &&
            why_cyclic_lmno.contains(base_pred) &&
            why_cyclic_lmno.contains(preds[9]) &&     // cyclic K L N O
            why_cyclic_lmno.contains(preds[12])       // cyclic L M N O
        ));

        PredSet why_circle_x_klm = tr.why_circle(x, k, l, m);
        REQUIRE((
            why_circle_x_klm.size() == 4 &&
            why_circle_x_klm.contains(base_pred) &&
            why_circle_x_klm.contains(preds[10]) &&   // circle X K L N
            why_cyclic_lmno.contains(preds[9]) &&     // cyclic K L N O
            why_circle_x_klm.contains(preds[12])      // cyclic L M N O
        ));

        // ------------ after 13 - eq I N

        PredSet why_cyclic_ilmo = tr.why_cyclic(i, l, m, o);
        REQUIRE((
            why_cyclic_ilmo.size() == 4 &&
            why_cyclic_ilmo.contains(base_pred) &&
            why_cyclic_lmno.contains(preds[9]) &&     // cyclic K L N O
            why_cyclic_ilmo.contains(preds[12]) &&    // cyclic L M N O
            why_cyclic_ilmo.contains(preds[13])       // eq I N
        ));
        
        PredSet why_cyclic_bfim = tr.why_cyclic(b, e, i, m);
        REQUIRE((
            why_cyclic_bfim.size() == 4 &&
            why_cyclic_bfim.contains(base_pred) &&
            why_cyclic_bfim.contains(preds[4]) &&     // cyclic F B I E
            why_cyclic_bfim.contains(preds[6]) &&     // cyclic F M N E
            why_cyclic_bfim.contains(preds[13])       // eq I N
        ));

        PredSet why_cyclic_efkm = tr.why_cyclic(e, f, k, m);
        REQUIRE((
            why_cyclic_efkm.size() == 6 &&
            why_cyclic_efkm.contains(base_pred) &&
            why_cyclic_efkm.contains(preds[4]) &&     // cyclic F B I E
            why_cyclic_efkm.contains(preds[5]) &&     // circle X F B I
            why_cyclic_efkm.contains(preds[6]) &&     // cyclic F M N E
            why_cyclic_efkm.contains(preds[10]) &&    // circle X K L N
            why_cyclic_efkm.contains(preds[13])       // eq I N
        ));

        // ------------ after 14 - eq Y Z

        PredSet why_cyclic_cefi = tr.why_cyclic(c, e, f, i);
        REQUIRE((
            why_cyclic_cefi.size() == 4 &&
            why_cyclic_cefi.contains(base_pred) &&
            why_cyclic_cefi.contains(preds[7]) &&     // circle Y G C E
            why_cyclic_cefi.contains(preds[8]) &&     // circle Z F G I
            why_cyclic_cefi.contains(preds[14])       // eq Y Z
        ));

        PredSet why_cyclic_bcgk = tr.why_cyclic(b, c, g, k);
        REQUIRE((
            why_cyclic_bcgk.contains(preds[4]) &&     // cyclic F B I E
            why_cyclic_bcgk.contains(preds[5]) &&     // circle X F B I - up to here, explains FBIE(X)
            why_cyclic_bcgk.contains(preds[10]) &&    // circle X K L N - explains KLNO(X)
            why_cyclic_bcgk.contains(preds[13]) &&    // eq I N - explains merger of FBIE(X) <- KLNO(X) to get BEFIKLO(X)
            why_cyclic_bcgk.contains(preds[7]) &&     // circle Y G C E - explains CEG(Y)
            why_cyclic_bcgk.contains(preds[8]) &&     // circle Z F G I - explains FGI(Z)
            why_cyclic_bcgk.contains(preds[14]) &&    // eq Y Z - explains merger of CEG(Y) <- FGI(Z)
            why_cyclic_bcgk.contains(diff_f_i) &&     // diff predicate for merging CEFGI(Y) <- FBIE(X)
            why_cyclic_bcgk.contains(base_pred) &&
            why_cyclic_bcgk.size() == 9
        ));

        PredSet why_cyclic_acfm = tr.why_cyclic(a, c, f, m);
        REQUIRE((
            why_cyclic_acfm.contains(preds[4]) &&     // cyclic F B I E
            why_cyclic_acfm.contains(preds[6]) &&     // cyclic F M N E
            why_cyclic_acfm.contains(preds[13]) &&    // eq I N - explains merger of FBIE <- FMNE to get BEFMI
            why_cyclic_acfm.contains(preds[7]) &&     // circle Y G C E
            why_cyclic_acfm.contains(preds[8]) &&     // circle Z F G I
            why_cyclic_acfm.contains(preds[14]) &&    // eq Y Z - explains merger of CEG(Y) <- FGI(Z)
            why_cyclic_acfm.contains(diff_f_i) &&     // diff predicate for merging CEFGI(Y) <- BEFMI to get BCEFGIM
            why_cyclic_acfm.contains(preds[1]) &&     // cyclic A B C D
            why_cyclic_acfm.contains(preds[2]) &&     // cyclic A B H I
            why_cyclic_acfm.contains(preds[3]) &&     // cyclic B H D J
            why_cyclic_acfm.contains(preds[11]) &&    // cyclic B H I J - explains ABCDHIJ
            why_cyclic_acfm.contains(diff_a_b) &&
            why_cyclic_acfm.contains(diff_b_d) &&     // diff predicates for forming ABCDHIJ
            why_cyclic_acfm.contains(diff_b_i) &&     // diff predicate for merging BCEFGI(Y) <- ABCDHIJ(W)
            why_cyclic_acfm.contains(base_pred) &&
            why_cyclic_acfm.size() == 15
        ));

        PredSet why_circle_y_ceg = tr.why_circle(y, c, e, g);
        REQUIRE((
            why_circle_y_ceg.size() == 2 &&
            why_circle_y_ceg.contains(base_pred) &&
            why_circle_y_ceg.contains(preds[7])    // circle Y G C E
        ));

        PredSet why_circle_y_lmi = tr.why_circle(y, l, m, i);
        REQUIRE((
            why_circle_y_lmi.contains(preds[7]) &&    // circle Y G C E 
            why_circle_y_lmi.contains(preds[8]) &&    // circle Z F G I
            why_circle_y_lmi.contains(preds[14]) &&   // eq Y Z - explains merger of CEG(Y) <- FGI(Z)
            why_circle_y_lmi.contains(preds[10]) &&   // circle X K L N
            why_circle_y_lmi.contains(preds[5]) &&    // circle X F B I
            why_circle_y_lmi.contains(preds[4]) &&    // cyclic F B I E - explains FBIE(X)
            why_circle_y_lmi.contains(preds[6]) &&    // cyclic F M N E
            why_circle_y_lmi.contains(preds[13]) &&   // eq I N - explains merger of FBIE(X) <- FMNE to form BEFMI(X)
                                                            // and subsequent merger of BEFMI(X) <- KLN(X) to form BEFIKLM(X) 
            why_circle_y_lmi.contains(preds[14]) &&   // eq Y Z - explains merger of CEG(Y) <- FGI(Z)
            why_circle_y_lmi.contains(diff_f_i) &&    // diff predicate for merging CEFGI(Y) <- BEFMI to get BCEFGIM
            why_circle_y_lmi.contains(base_pred)
        ));

        PredSet why_circle_y_abc = tr.why_circle(y, a, b, c);
        REQUIRE((
            why_circle_y_abc.contains(preds[7]) &&     // circle Y G C E 
            why_circle_y_abc.contains(preds[8]) &&     // circle Z F G I
            why_circle_y_abc.contains(preds[14]) &&    // eq Y Z - explains formation of CEFGI(Y)
            why_circle_y_abc.contains(preds[4]) &&     // cyclic F B I E - explains why CEFGI(Y) <- FBIE to get BCEFGI(Y)
            why_circle_y_abc.contains(diff_f_i) &&     // diff predicate for merging CEFGI(Y) <- FBIE
            why_circle_y_abc.contains(preds[1]) &&     // cyclic A B C D
            why_circle_y_abc.contains(preds[2]) &&     // cyclic A B H I
            why_circle_y_abc.contains(preds[3]) &&     // cyclic B H D J
            why_circle_y_abc.contains(preds[11]) &&    // cyclic B H I J - explains ABCDHIJ
            why_circle_y_abc.contains(diff_a_b) &&
            why_circle_y_abc.contains(diff_b_d) &&     // diff predicates for forming ABCDHIJ
            why_circle_y_abc.contains(diff_b_i) &&     // diff predicate for merging BCEFGI(Y) <- ABCDHIJ(W)
            why_circle_y_abc.contains(base_pred) &&
            why_circle_y_abc.size() == 13
        ));
    }
}