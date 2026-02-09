
#include <doctest.h>
#include <iostream>

#include "Geometry/GeometricGraph.hh"
#include "Common/Exceptions.hh"
#include "Geometry/Node.hh"

TEST_SUITE("GeometricGraph: Triangle congruence and similarity") {
    TEST_CASE("Triangle permutation") {
        GeometricGraph ggraph;
        DDEngine dd;
        AREngine ar;
        Predicate* base_pred = dd.base_pred.get();

        Point* a = ggraph.__add_new_point("a");
        Point* b = ggraph.__add_new_point("b");
        Point* c = ggraph.__add_new_point("c");

        Triangle* t = ggraph.get_or_add_triangle(a, b, c, base_pred);
        std::array<int, 3> perm = t->get_perm({b, c, a});
        REQUIRE(perm == std::array<int, 3>{1, 2, 0});
        // This means b is in index 1, c in index 2, a in index 0

        // compose_perm tests with hardcoded inputs
        std::array<std::array<int, 3>, 6> perms = {{
            {0, 1, 2},
            {1, 2, 0},
            {2, 0, 1},
            {0, 2, 1},
            {2, 1, 0},
            {1, 0, 2}
        }};
        bool all_pass = true;
        for (const auto& perm1 : perms) {
            for (const auto& perm2 : perms) {
                std::array<int, 3> result_perm = Triangle::compose_perm(perm1, perm2);
                std::array<Point*, 3> old_vertices = t->vertices;
                t->permute(perm2);
                std::array<Point*, 3> neutral_vertices = t->vertices;
                t->vertices = old_vertices;
                t->permute(result_perm);
                std::array<Point*, 3> new_vertices = t->vertices;
                t->permute(perm1);
                all_pass = all_pass && (t->vertices == neutral_vertices);

                t->vertices = old_vertices;
                std::array<int, 3> expected_perm = t->get_perm(new_vertices);
                all_pass = all_pass && (result_perm == expected_perm);
            }
        }
        REQUIRE(all_pass);
    }
    TEST_CASE("Setting congruence") {
        GeometricGraph ggraph;
        DDEngine dd;
        AREngine ar;
        Predicate* base_pred = dd.base_pred.get();

        Point* a = ggraph.__add_new_point("a");
        Point* b = ggraph.__add_new_point("b", {1, 0});
        Point* c = ggraph.__add_new_point("c", {1, 1});
        Point* d = ggraph.__add_new_point("d", {2, 1});
        Point* e = ggraph.__add_new_point("e", {2, 0});

        // Oppositely oriented congruent triangles: ABC ~ BCD
        ggraph.__make_contri(a, b, c, b, c, d, nullptr, dd, ar);
        REQUIRE((
            ggraph.check_cong(a, b, b, c) &&
            ggraph.check_cong(b, c, c, d) &&
            ggraph.check_cong(a, c, b, d)
        ));
        REQUIRE((
            ggraph.check_eqangle(a, b, b, c, d, c, c, b) &&
            ggraph.check_eqangle(b, c, c, a, b, d, d, c) &&
            ggraph.check_eqangle(c, a, a, b, c, b, b, d)
        ));

        // Similarly oriented congruent triangles: BDE ~ DBC <-> ABC ~ BCD ~ DEB
        ggraph.__make_contri(b, d, e, d, b, c, nullptr, dd, ar);
        REQUIRE(ggraph.check_cong(b, e, d, e));
        // BCD, DEB similarly oriented
        REQUIRE((
            ggraph.check_eqangle(d, b, b, e, b, d, d, c) &&
            ggraph.check_eqangle(b, c, c, d, d, e, e, b)
        ));
        // ABC, DEB oppositely oriented
        REQUIRE((
            ggraph.check_eqangle(b, d, d, e, b, a, a, c) &&
            ggraph.check_eqangle(d, b, b, e, b, c, c, a)
        ));
        REQUIRE(ggraph.check_contri(a, b, c, d, e, b));
        REQUIRE_FALSE(ggraph.check_contri(a, b, c, d, b, e));

        Triangle* t1 = ggraph.get_or_add_triangle(a, b, c, base_pred);
        Triangle* t2 = ggraph.get_or_add_triangle(b, c, d, base_pred);
        Triangle* t3 = ggraph.get_or_add_triangle(b, d, e, base_pred);

        std::array<int, 3> perm1 = t1->get_perm({c, a, b});
        std::array<int, 3> perm2 = t2->get_perm({d, b, c});
        std::array<int, 3> perm3 = t3->get_perm({b, d, e});
        REQUIRE((perm1 == perm2 && perm2 == perm3));
    }
    TEST_CASE("Setting similarity") {
        GeometricGraph ggraph;
        DDEngine dd;
        AREngine ar;
        Predicate* base_pred = dd.base_pred.get();

        Point* a = ggraph.__add_new_point("a", {-8, 0});
        Point* b = ggraph.__add_new_point("b", {-4, 0});
        Point* c = ggraph.__add_new_point("c", {0, 0});
        Point* d = ggraph.__add_new_point("d", {9, 0});
        Point* e = ggraph.__add_new_point("e", {18, 0});
        Point* f = ggraph.__add_new_point("f", {-4, 6});
        Point* g = ggraph.__add_new_point("g", {0, 6});
        Point* h = ggraph.__add_new_point("h", {9, 6});
        Point* i = ggraph.__add_new_point("i", {0, 12});

        ggraph.__make_contri(c, g, f, i, g, f, nullptr, dd, ar);
        ggraph.__make_contri(c, g, h, i, g, h, nullptr, dd, ar);

        ggraph.__make_simtri(c, f, g, h, c, g, nullptr, dd, ar);
        Triangle* ifg = ggraph.get_or_add_triangle(i, f, g, base_pred);
        Triangle* ihg = ggraph.get_or_add_triangle(i, h, g, base_pred);
        REQUIRE(ggraph.check_simtri(ifg, ihg));

        Triangle* cfg = ggraph.get_or_add_triangle(c, f, g, base_pred);
        Triangle* chg = ggraph.get_or_add_triangle(c, h, g, base_pred);
        Dimension* dim_cfg = cfg->get_dimension();
        Dimension* dim_chg = chg->get_dimension();
        Shape* shp = dim_cfg->get_shape();
        REQUIRE((
            dim_chg->get_shape() == shp &&
            ifg->get_shape() == shp &&
            ihg->get_shape() == shp &&
            cfg->get_shape() == shp &&
            chg->get_shape() == shp
        ));
        REQUIRE((
            shp->root_obj2s.contains(dim_cfg) &&
            shp->root_obj2s.contains(dim_chg)
        ));
        REQUIRE((ggraph.root_shapes.contains(shp) && ggraph.root_shapes.size() == 1));
        std::array<int, 3> perm1 = cfg->get_perm({c, f, g});
        REQUIRE((
            ifg->get_perm({i, f, g}) == perm1 &&
            ihg->get_perm({h, i, g}) == perm1 &&
            cfg->get_perm({c, f, g}) == perm1 &&
            chg->get_perm({h, c, g}) == perm1
        ));

        ggraph.__make_contri(a, b, f, c, b, f, nullptr, dd, ar);
        ggraph.__make_simtri(c, b, f, f, c, h, nullptr, dd, ar);
        Triangle* abf = ggraph.get_or_add_triangle(a, b, f, base_pred);
        Triangle* cbf = ggraph.get_or_add_triangle(c, b, f, base_pred);
        Triangle* fch = ggraph.get_or_add_triangle(f, c, h, base_pred);

        ggraph.__make_simtri(a, f, b, h, c, d, nullptr, dd, ar);
        Triangle* hcd = ggraph.get_or_add_triangle(h, c, d, base_pred);
        REQUIRE(ggraph.check_simtri(h, c, f, c, d, h));
        REQUIRE((ggraph.root_shapes.contains(fch->get_shape()) && ggraph.root_shapes.size() == 2));
        REQUIRE((ggraph.root_dimensions.size() == 5));

        ggraph.__make_contri(a, b, f, f, g, i, nullptr, dd, ar);
        REQUIRE(!shp->is_root());
        shp = NodeUtils::get_root(shp);
        REQUIRE((!dim_cfg->is_root() && dim_cfg->root == abf->dimension));
        dim_cfg = NodeUtils::get_root(dim_cfg);

        REQUIRE((
            ifg->get_shape() == shp &&
            ihg->get_shape() == shp &&
            cfg->get_shape() == shp &&
            chg->get_shape() == shp &&
            abf->get_shape() == shp &&
            fch->get_shape() == shp &&
            hcd->get_shape() == shp
        ));
        REQUIRE((
            shp->root_obj2s.size() == 4 &&
            shp->root_obj2s.contains(dim_chg) && 
            shp->root_obj2s.contains(cbf->get_dimension()) &&
            shp->root_obj2s.contains(fch->get_dimension()) &&
            shp->root_obj2s.contains(hcd->get_dimension())
        ));
        REQUIRE((
            dim_cfg->root_triangles.size() == 4 &&
            abf->get_dimension() == dim_cfg && dim_cfg->root_triangles.contains(abf) &&
            cbf->get_dimension() == dim_cfg && dim_cfg->root_triangles.contains(cbf) &&
            cfg->get_dimension() == dim_cfg && dim_cfg->root_triangles.contains(cfg) &&
            ifg->get_dimension() == dim_cfg && dim_cfg->root_triangles.contains(ifg)
        ));

        std::array<int, 3> perm2 = abf->get_perm({f, a, b});
        REQUIRE((
            ifg->get_perm({i, f, g}) == perm2 &&
            ihg->get_perm({h, i, g}) == perm2 &&
            cfg->get_perm({c, f, g}) == perm2 &&
            chg->get_perm({h, c, g}) == perm2 &&
            cbf->get_perm({f, c, b}) == perm2 &&
            fch->get_perm({h, f, c}) == perm2 &&
            hcd->get_perm({c, h, d}) == perm2
        ));

        ggraph.__make_contri(h, d, e, h, d, c, nullptr, dd, ar);
        REQUIRE((
            ggraph.root_shapes.size() == 1 &&
            ggraph.root_dimensions.size() == 4
        ));
        int count = 0;
        for (Dimension* dim : ggraph.root_dimensions) {
            count += dim->root_triangles.size();
        }
        REQUIRE(count == 9);

        Triangle* hde = ggraph.get_or_add_triangle(h, d, e, base_pred);
        std::array<int, 3> perm3 = hde->get_perm({h, d, e});
        REQUIRE((
            ifg->get_perm({f, g, i}) == perm3 &&
            ihg->get_perm({i, g, h}) == perm3 &&
            cfg->get_perm({f, g, c}) == perm3 &&
            chg->get_perm({c, g, h}) == perm3 &&
            cbf->get_perm({c, b, f}) == perm3 &&
            fch->get_perm({f, c, h}) == perm3 &&
            hcd->get_perm({h, d, c}) == perm3 &&
            abf->get_perm({a, b, f}) == perm3
        ));
    }
    TEST_CASE("Setting isosceles and equilateral mask") {
        GeometricGraph ggraph;
        DDEngine dd;
        AREngine ar;
        Predicate* base_pred = dd.base_pred.get();

        Point* a = ggraph.__add_new_point("a", {0, -2});
        Point* b = ggraph.__add_new_point("b", {-1, -1});
        Point* c = ggraph.__add_new_point("c", {0, -1});
        Point* d = ggraph.__add_new_point("d", {1, -1});
        Point* e = ggraph.__add_new_point("e", {-2, 0});
        Point* f = ggraph.__add_new_point("f", {0, 0});
        Point* g = ggraph.__add_new_point("g", {2, 0});
        Point* h = ggraph.__add_new_point("h", {-1, std::sqrt(3)});
        Point* i = ggraph.__add_new_point("i", {1, std::sqrt(3)});
    }
}