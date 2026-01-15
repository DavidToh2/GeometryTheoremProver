#pragma once

#include <string>
#include <vector>
#include <memory>

#include "Predicate.hh"
#include "Common/Arg.hh"
#include "Common/Generator.hh"
#include "Numerics/Numerics.hh"

class DDEngine;
class NumEngine;
class GeometricGraph;

/* Construction templates. 

Note that all constructions currently only take points as their arguments. */
class Construction {

    std::vector<std::unique_ptr<Arg>> args_existing;
    std::vector<std::unique_ptr<Arg>> args_new;

    ClauseTemplate preconditions;
    std::vector<std::unique_ptr<PredicateTemplate>> postconditions;
    std::vector<std::unique_ptr<NumericTemplate>> numerics;

    /* Assemble a Construction Node from the declaration, preconditions and postconditions strings.

    Declaration string format: "construction_name new_arg1 new_arg2 ... : arg1 arg2 ... "
    
    Precondition string format: "predicate1, predicate2, ..."
    
    Postcondition string format: "predicate1, predicate2, ..."

    Numeric string format: "points = num1, num2, ...; ..."
    */
    Construction(const std::string _c_decl, const std::string _c_pres, const std::string _c_posts, const std::string _c_nums);

    /* Fill a construction template with Node arguments.
    Note that all constructions currently only take points as their arguments. */
    void __set_node_args(std::vector<Node*> &nodes_existing, std::vector<Node*> &nodes_new);
    void __set_placeholder_args();
    void __clear_args();

    /* Static method. Given a construction declaration string of the form `construction_name new_arg1 new_arg2 ... : arg1 arg2 ...`,
    parse it into its components `[construction_name, new_args, existing_args]` */
    static std::tuple<std::string, std::string, std::string> parse_decl_string(const std::string c_decl);

    /* Perform a construction according to the template given in `cstage_string`.
    Lazily returns all new predicates generated during the construction. */
    Generator<std::unique_ptr<Predicate>> __instantiate_preds(DDEngine &dd);

    bool __instantiation_check(DDEngine &dd);

    /* Perform a construction according to the template given in `cstage_string`.
    Lazily returns all new predicates generated during the construction. 
    This variant of the instantiation method does not perform any precondition checks. Rather, it also instantiates
    the preconditions themselves as new predicates. */
    Generator<std::unique_ptr<Predicate>> __instantiate_preds_no_checks(Predicate* base_pred);

    /* Perform a construction according to the template given in `cstage_string`.
    Lazily returns all new numerics generated during the construction. */
    Generator<std::unique_ptr<Numeric>> __instantiate_numerics();

public:
    std::string name = "";

    Construction(const std::tuple<std::string, std::string, std::string, std::string> v) 
    : Construction(std::get<0>(v), std::get<1>(v), std::get<2>(v), std::get<3>(v)) {}

    static void construct(
        const std::string cstage_string, DDEngine &dd, NumEngine &nm, GeometricGraph &ggraph
    );
    /* Static method. Perform a construction stage as described in `cstage_string`. The construction stage may contain
    multiple constructions: refer to `Outline.md` for more information.
    This variant of the construct method does not perform any precondition checks.
    
    Note: In reality, this method does not treat newly-added points and already-existing points any differently. Both
    groups are matched to their corresponding `Object*` s via `ggraph.get_or_add_point()`. */
    static void construct_no_checks(
        const std::string cstage_string, DDEngine &dd, NumEngine &nm, GeometricGraph &ggraph
    );

    std::string to_string();
    std::string to_string_with_placeholders();
};