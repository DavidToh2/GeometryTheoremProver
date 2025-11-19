#pragma once

#include <string>
#include <vector>
#include <memory>

#include "Predicate.hh"
#include "DDEngine.hh"
#include "Geometry/GeometricGraph.hh"
#include "Common/Generator.hh"

/* Construction templates. 

Note that all constructions currently only take points as their arguments. */
class Construction {

public:
    std::string name = "";
    std::vector<std::unique_ptr<Arg>> args_existing;
    std::vector<std::unique_ptr<Arg>> args_new;

    Clause preconditions;
    std::vector<std::unique_ptr<PredicateTemplate>> postconditions;

    /* Assemble a Construction object from the declaration, preconditions and postconditions strings.

    Declaration string format: "construction_name new_arg1 new_arg2 ... : arg1 arg2 ... "
    
    Precondition string format: "predicate1, predicate2, ..."
    
    Postcondition string format: "predicate1, predicate2, ..."
    */
    Construction(const std::string _c_decl, const std::string _c_pres, const std::string _c_posts);
    Construction(const std::vector<std::string> v) : Construction(v[0], v[1], v[2]) {}

    /* Fill a construction template with object arguments.
    
    Note that all constructions currently only take points as their arguments. */
    void __set_obj_args(std::vector<Object*> &objs_existing, std::vector<Object*> &objs_new);
    void __set_placeholder_args();
    void __clear_args();
    static std::array<std::string, 3> parse_decl_string(const std::string c_decl);
    std::string to_string();

    /* Perform a construction according to the template given in `c_template`.
    
    Lazily returns a list of all new predicates generated during the construction. */
    Generator<std::unique_ptr<Predicate>> __instantiate(
        std::vector<Object*> &objs_existing, std::vector<Object*> &objs_new, DDEngine &dd
    );
    bool __instantiation_check(
        std::vector<Object*> &objs_existing, DDEngine &dd
    );
    Generator<std::unique_ptr<Predicate>> __instantiate_no_checks(
        std::vector<Object*> &objs_existing, std::vector<Object*> &objs_new
    );

    static void construct(
        const std::string c_string, DDEngine &dd, GeometricGraph &ggraph
    );
    static void construct_no_checks(
        const std::string c_string, DDEngine &dd, GeometricGraph &ggraph
    );
};