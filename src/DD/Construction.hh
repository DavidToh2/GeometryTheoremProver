#pragma once

#include <string>
#include <vector>
#include <memory>

#include "Predicate.hh"
#include "Common/Generator.hh"

class DDEngine;
class GeometricGraph;

/* Construction templates. 

Note that all constructions currently only take points as their arguments. */
class Construction {

public:
    std::string name = "";
    std::vector<std::unique_ptr<Arg>> args_existing;
    std::vector<std::unique_ptr<Arg>> args_new;

    ClauseTemplate preconditions;
    std::vector<std::unique_ptr<PredicateTemplate>> postconditions;

    /* Assemble a Construction Node from the declaration, preconditions and postconditions strings.

    Declaration string format: "construction_name new_arg1 new_arg2 ... : arg1 arg2 ... "
    
    Precondition string format: "predicate1, predicate2, ..."
    
    Postcondition string format: "predicate1, predicate2, ..."
    */
    Construction(const std::string _c_decl, const std::string _c_pres, const std::string _c_posts);
    Construction(const std::vector<std::string> v) : Construction(v[0], v[1], v[2]) {}

    /* Fill a construction template with Node arguments.
    
    Note that all constructions currently only take points as their arguments. */
    void __set_node_args(std::vector<Node*> &nodes_existing, std::vector<Node*> &nodes_new);
    void __set_placeholder_args();
    void __clear_args();
    static std::array<std::string, 3> parse_decl_string(const std::string c_decl);

    /* Perform a construction according to the template given in `c_template`.
    
    Lazily returns a list of all new predicates generated during the construction. */
    Generator<std::unique_ptr<Predicate>> __instantiate(
        std::vector<Node*> &nodes_existing, std::vector<Node*> &nodes_new, DDEngine &dd
    );
    bool __instantiation_check(
        std::vector<Node*> &nodes_existing, DDEngine &dd
    );
    Generator<std::unique_ptr<Predicate>> __instantiate_no_checks(
        std::vector<Node*> &nodes_existing, std::vector<Node*> &nodes_new, Predicate* base_pred
    );

    static void construct(
        const std::string c_string, DDEngine &dd, GeometricGraph &ggraph
    );
    static void construct_no_checks(
        const std::string c_string, DDEngine &dd, GeometricGraph &ggraph
    );

    std::string to_string();
    std::string to_string_with_placeholders();
};