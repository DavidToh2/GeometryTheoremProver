
#include <doctest.h>

#include "Geometry/GeometricGraph.hh"
#include "Traceback/TracebackEngine.hh"

TEST_SUITE("TracebackEngine: why_() functions") {
    TEST_CASE("why_eqangle()") {
        GeometricGraph ggraph;
        DDEngine dd;
        AREngine ar;
        TracebackEngine tr;
        ggraph.tr = &tr;
        Predicate* base_pred = dd.base_pred.get();

        Point* a = ggraph.__add_new_point("a", {0, 0});
        Point* b = ggraph.__add_new_point("b", {0, 4});
        Point* c = ggraph.__add_new_point("c", {4, 4});
        Point* d = ggraph.__add_new_point("d", {4, 0});
        Point* e = ggraph.__add_new_point("e", {2, 4});
        Point* f = ggraph.__add_new_point("f", {4, 2});
        Point* g = ggraph.__add_new_point("g", {3, 4});
        Point* h = ggraph.__add_new_point("h", {6, 4});
        Point* i = ggraph.__add_new_point("i", {0, 2});
        Point* j = ggraph.__add_new_point("j", {8, 4});

        std::vector<Predicate*> preds;

        Angle* daf = ggraph.get_or_add_angle(d, a, f, dd);
        Angle* ifa = ggraph.get_or_add_angle(i, f, a, dd);
        Angle* fic = ggraph.get_or_add_angle(f, i, c, dd);
        Angle* bci = ggraph.get_or_add_angle(b, c, i, dd);
        Angle* cjf = ggraph.get_or_add_angle(c, j, f, dd);

        Angle* eab = ggraph.get_or_add_angle(e, a, b, dd);
        Angle* hdc = ggraph.get_or_add_angle(h, d, c, dd);

        Angle* cde = ggraph.get_or_add_angle(c, d, e, dd);
        Angle* fag = ggraph.get_or_add_angle(f, a, g, dd);
        Angle* cfg = ggraph.get_or_add_angle(c, f, g, dd);

        // Build the square ABCD
        preds.emplace_back(dd.insert_new_predicate(
            std::make_unique<Predicate>(
                pred_t::CONG, std::vector<Node*>{a, b, b, c})
        ));
        preds.emplace_back(dd.insert_new_predicate(
            std::make_unique<Predicate>(
                pred_t::CONG, std::vector<Node*>{b, c, c, d})
        ));
        preds.emplace_back(dd.insert_new_predicate(
            std::make_unique<Predicate>(
                pred_t::CONG, std::vector<Node*>{c, d, d, a})
        ));
        preds.emplace_back(dd.insert_new_predicate(
            std::make_unique<Predicate>(
                pred_t::PARA, std::vector<Node*>{a, b, c, d})
        ));
        preds.emplace_back(dd.insert_new_predicate(
            std::make_unique<Predicate>(
                pred_t::PARA, std::vector<Node*>{b, c, d, a})
        ));
        preds.emplace_back(dd.insert_new_predicate(
            std::make_unique<Predicate>(
                pred_t::PERP, std::vector<Node*>{a, b, b, c})
        ));

        // Build the other points
        preds.emplace_back(dd.insert_new_predicate(
            std::make_unique<Predicate>(
                pred_t::MIDP, std::vector<Node*>{e, b, c})
        ));
        preds.emplace_back(dd.insert_new_predicate(
            std::make_unique<Predicate>(
                pred_t::MIDP, std::vector<Node*>{f, c, d})
        ));
        preds.emplace_back(dd.insert_new_predicate(
            std::make_unique<Predicate>(
                pred_t::MIDP, std::vector<Node*>{c, b, j})
        ));
        preds.emplace_back(dd.insert_new_predicate(
            std::make_unique<Predicate>(
                pred_t::MIDP, std::vector<Node*>{g, e, c})
        ));
        preds.emplace_back(dd.insert_new_predicate(
            std::make_unique<Predicate>(
                pred_t::MIDP, std::vector<Node*>{h, c, j})
        ));
        preds.emplace_back(dd.insert_new_predicate(
            std::make_unique<Predicate>(
                pred_t::MIDP, std::vector<Node*>{i, a, b})
        ));
        preds.emplace_back(dd.insert_new_predicate(
            std::make_unique<Predicate>(
                pred_t::COLL, std::vector<Node*>{a, f, j})
        ));
        ggraph.synthesise_preds(dd, ar);

        REQUIRE(NodeUtils::get_root(daf) == cjf);

        /*
        0 - cong A B B C
        1 - cong B C C D
        2 - cong C D D A
        3 - para A B C D
        4 - para B C D A
        5 - perp A B B C
        6 - midp E B C
        7 - midp F C D
        8 - midp C B J
        9 - midp G E C
        10 - midp H C J
        11 - midp I A B
        12 - coll A F J
        */

        /* Round 1 */

        preds.emplace_back(dd.insert_new_predicate(
            std::make_unique<Predicate>(
                pred_t::PARA, std::vector<Node*>{b, c, f, i})
        ));
        preds.emplace_back(dd.insert_new_predicate(
            std::make_unique<Predicate>(
                pred_t::PARA, std::vector<Node*>{d, e, f, g})
        ));
        preds.emplace_back(dd.insert_new_predicate(
            std::make_unique<Predicate>(
                pred_t::PARA, std::vector<Node*>{a, e, d, h})
        ));
        preds.emplace_back(dd.insert_new_predicate(
            std::make_unique<Predicate>(
                pred_t::EQANGLE, std::vector<Node*>{f, a, a, g, e, a, a, b})
        ));
        ggraph.synthesise_preds(dd, ar);

        REQUIRE((
            NodeUtils::get_root(ifa) == cjf &&
            NodeUtils::get_root(fic) == bci &&
            NodeUtils::get_root(cfg) == cde &&
            NodeUtils::get_root(hdc) == eab
        ));
        REQUIRE((
            Angle::is_equal(fag, eab) &&
            Angle::is_equal(fag, hdc)
        ));
        REQUIRE((
            ggraph.root_angles.size() == 5 &&
            ggraph.root_measures.size() == 1
        ));

        /* 
        13 - para B C F I
        14 - para D E F G
        15 - para A E D H
        16 - eqangle F A A G E A A B
        */

        Measure* m_fag = fag->get_measure();
        REQUIRE(m_fag->to_string() == "m_a_d_l_a_f_d_l_a_g");

        PredSet why_eab_m_fag = tr.why_measure_of(m_fag, eab);
        REQUIRE((
            why_eab_m_fag.contains(preds[16]) &&    // eqangle F A A G E A A B
            why_eab_m_fag.size() == 1
        ));

        PredSet why_eq_hdc_eab = TracebackUtils::why_ancestor(hdc, eab);
        REQUIRE((
            why_eq_hdc_eab.contains(preds[3]) &&    // para A B C D
            why_eq_hdc_eab.contains(preds[15])      // para A E D H
        ));

        PredSet why_eq_cfg_cde = TracebackUtils::why_ancestor(cfg, cde);
        REQUIRE((
            why_eq_cfg_cde.contains(preds[7]) &&    // midp F C D
            why_eq_cfg_cde.contains(preds[14])      // para D E F G
        ));

        /* Round 2 */

        Measure* m_cde = ggraph.get_or_add_measure(cde, dd);
        REQUIRE(m_cde->to_string() == "m_a_d_l_c_d_d_l_d_e");
        Measure* m_bci = ggraph.get_or_add_measure(bci, dd);
        REQUIRE(m_bci->to_string() == "m_a_d_l_b_c_d_l_c_i");
        Measure* m_daf = ggraph.get_or_add_measure(daf, dd);
        REQUIRE(m_daf->to_string() == "m_a_d_l_c_j_d_l_f_j");

        preds.emplace_back(dd.insert_new_predicate(
            std::make_unique<Predicate>(
                pred_t::EQANGLE, std::vector<Node*>{c, f, f, g, i, f, f, a})
        ));
        preds.emplace_back(dd.insert_new_predicate(
            std::make_unique<Predicate>(
                pred_t::EQANGLE, std::vector<Node*>{f, a, a, g, b, c, c, i})
        ));
        ggraph.synthesise_preds(dd, ar);

        REQUIRE(ggraph.root_measures.size() == 2);

        /*
        17 - eqangle C F F G I F F A
        18 - eqangle F A A G B C C I
        */

        m_cde = cde->get_measure();
        REQUIRE(m_cde->to_string() == "m_a_d_l_c_d_d_l_d_e");   // cde

        PredSet why_cde_m_cde = tr.why_measure_of(m_cde, NodeUtils::get_root(cde));
        REQUIRE((
            why_cde_m_cde.contains(base_pred) &&
            why_cde_m_cde.size() == 1
        ));

        m_fag = fag->get_measure();
        REQUIRE(m_fag->to_string() == "m_a_d_l_a_f_d_l_a_g");   // fag

        PredSet why_bci_m_fag = tr.why_measure_of(m_fag, bci);
        REQUIRE((
            why_bci_m_fag.contains(preds[18]) &&    // eqangle F A A G B C C I
            why_bci_m_fag.contains(base_pred) &&
            why_bci_m_fag.size() == 2
        ));

        /* Round 3 */
        
        preds.emplace_back(dd.insert_new_predicate(
            std::make_unique<Predicate>(
                pred_t::PARA, std::vector<Node*>{c, i, a, f})
        ));
        ggraph.synthesise_preds(dd, ar);

        REQUIRE((
            NodeUtils::get_root(cjf) == bci &&
            Angle::is_equal(bci, cde) &
            Angle::is_equal(cjf, fag)
        ));

        /*
        19 - para A F C I
        */

        PredSet why_eq_cjf_bci = TracebackUtils::why_ancestor(cjf, bci);
        REQUIRE((
            why_eq_cjf_bci.contains(preds[8]) &&    // midp C B J
            why_eq_cjf_bci.contains(preds[19])      // para A F C I
        ));

        /*
        0 - cong A B B C        10 - midp H C J
        1 - cong B C C D        11 - midp I A B
        2 - cong C D D A        12 - coll A F J
        3 - para A B C D        13 - para B C F I
        4 - para B C D A        14 - para D E F G
        5 - perp A B B C        15 - para A E D H
        6 - midp E B C          16 - eqangle F A A G E A A B
        7 - midp F C D          17 - eqangle C F F G I F F A
        8 - midp C B J          18 - eqangle F A A G B C C I
        9 - midp G E C          19 - para A F C I
        */

        /* After round 1 */

        PredSet why_eqangle_fag_eab = tr.why_eqangle(f, a, a, g, e, a, a, b);
        REQUIRE((
            why_eqangle_fag_eab.contains(preds[16]) &&      // eqangle F A A G E A A B
            why_eqangle_fag_eab.contains(base_pred) &&
            why_eqangle_fag_eab.size() == 2
        ));

        PredSet why_eqangle_fag_hdc = tr.why_eqangle(f, a, a, g, h, d, d, c);
        REQUIRE((
            why_eqangle_fag_hdc.contains(preds[3]) &&       // para A B C D
            why_eqangle_fag_hdc.contains(preds[15]) &&      // para A E D H - these explain EAB = HDC
            why_eqangle_fag_hdc.contains(preds[16]) &&      // eqangle F A A G E A A B
            why_eqangle_fag_hdc.contains(base_pred) &&
            why_eqangle_fag_hdc.size() == 4
        ));

        /* After round 2 */

        PredSet why_eqangle_fic_hdc = tr.why_eqangle(f, i, i, c, h, d, d, c);
        REQUIRE((
            why_eqangle_fic_hdc.contains(preds[3]) &&       // para A B C D
            why_eqangle_fic_hdc.contains(preds[15]) &&      // para A E D H - these explain EAB = HDC
            why_eqangle_fic_hdc.contains(preds[13]) &&      // para B C F I - explains BCI = FIC
            why_eqangle_fic_hdc.contains(preds[16]) &&      // eqangle F A A G E A A B
            why_eqangle_fic_hdc.contains(preds[18]) &&      // eqangle F A A G B C C I
            why_eqangle_fic_hdc.contains(base_pred)
        ));

        /* After round 3 */

        PredSet why_eqangle_cjf_bci = tr.why_eqangle(c, j, j, f, b, c, c, i);
        REQUIRE((
            why_eqangle_cjf_bci.contains(preds[8]) &&       // midp C B J
            why_eqangle_cjf_bci.contains(preds[19]) &&      // para A F C I
            why_eqangle_cjf_bci.contains(base_pred)
        ));

        PredSet why_eqangle_dfg_fag = tr.why_eqangle(d, f, f, g, f, a, a, g);
        REQUIRE((
            why_eqangle_dfg_fag.contains(preds[7]) &&       // midp F C D
            why_eqangle_dfg_fag.contains(preds[14]) &&      // para D E F G - these explain CDE = CFG
            why_eqangle_dfg_fag.contains(preds[8]) &&       // midp C B J
            why_eqangle_dfg_fag.contains(preds[17]) &&      // eqangle C F F G I F F A - these explain CJF = CDE
            why_eqangle_dfg_fag.contains(preds[18]) &&      // eqangle F A A G B C C I
            why_eqangle_dfg_fag.contains(preds[19]) &&      // para A F C I - explains BCI = CJF
            why_eqangle_dfg_fag.contains(base_pred)
        ));

        PredSet why_eqangle_hdc_cfg = tr.why_eqangle(h, d, d, c, c, f, f, g);
        REQUIRE((
            why_eqangle_hdc_cfg.contains(preds[7]) &&       // midp F C D
            why_eqangle_hdc_cfg.contains(preds[14]) &&      // para D E F G - these explain CDE = CFG
            why_eqangle_hdc_cfg.contains(preds[8]) &&       // midp C B J
            why_eqangle_hdc_cfg.contains(preds[17]) &&      // eqangle C F F G I F F A - these explain CJF = CDE
            why_eqangle_hdc_cfg.contains(preds[18]) &&      // eqangle F A A G B C C I
            why_eqangle_hdc_cfg.contains(preds[3]) &&       // para A B C D
            why_eqangle_hdc_cfg.contains(preds[15]) &&      // para A E D H - these explain EAB = HDC
            why_eqangle_hdc_cfg.contains(preds[16]) &&      // eqangle F A A G E A A B - at this point we have HDC = EAB = FAG = BCI
            why_eqangle_hdc_cfg.contains(preds[19]) &&      // para A F C I - explains BCI = CJF
            why_eqangle_hdc_cfg.contains(base_pred)
        ));

    }
}