
#include <vector>
#include <string>
#include <memory>
#include <map>
#include <iostream>

#include "Construction.hh"
#include "Common/StrUtils.hh"
#include "Common/Exceptions.hh"
#include "Predicate.hh"
#include "Common/Generator.hh"
#include "Geometry/GeometricGraph.hh"
#include "DD/DDEngine.hh"
#include "Numerics/NumEngine.hh"

Construction::Construction(const std::string _c_decl, const std::string _c_pres, const std::string _c_posts, const std::string _c_nums) {

    std::map<std::string, Arg*> argmap;

    // Populate arguments from declaration string
    auto [_n, _args_new, _args_existing] = parse_decl_string(_c_decl);
    name = _n;
    Arg::populate_args_and_argmap(_args_new, args_new, argmap);
    Arg::populate_args_and_argmap(_args_existing, args_existing, argmap);

    // Create precondition template
    preconditions = ClauseTemplate(_c_pres, argmap);

    // Create postcondition templates
    std::vector<std::string> c_posts = StrUtils::split(_c_posts, ", ");
    for (std::string _c_post : c_posts) {
        postconditions.emplace_back(std::make_unique<PredicateTemplate>(_c_post, argmap));
    }

    // Create numeric templates
    std::vector<std::string> c_nums = StrUtils::split(_c_nums, ";");
    for (std::string _c_num : c_nums) {
        auto [outs, nums] = StrUtils::split_first(_c_num, "=");
        StrUtils::trim(outs);
        StrUtils::trim(nums);
        std::vector<std::string> num_list = StrUtils::split(nums, ",");
        for (std::string num : num_list) {
            StrUtils::trim(num);
            numerics.emplace_back(std::make_unique<NumericTemplate>(outs, num, argmap));
        }
    }
}

void Construction::__set_node_args(std::vector<Node*> &nodes_existing, std::vector<Node*> &nodes_new) {
    if (nodes_existing.size() != args_existing.size()) {
        throw DDInternalError("Error: Mismatch in number of existing object arguments when setting construction args for construction " + name);
    }
    if (nodes_new.size() != args_new.size()) {
        throw DDInternalError("Error: Mismatch in number of new object arguments when setting construction args for construction " + name);
    }

    for (int i=0; i<nodes_existing.size(); i++) {
        args_existing.at(i).get()->set(nodes_existing.at(i));
    }
    for (int i=0; i<nodes_new.size(); i++) {
        args_new.at(i).get()->set(nodes_new.at(i));
    }
}

void Construction::__set_placeholder_args() {
    char c1 = 'a';
    for (auto& argptr : args_existing) {
        argptr.get()->set(c1);
        c1++;
    }
    char c2 = 'p';
    for (auto& argptr : args_new) {
        argptr.get()->set(c2);
        c2++;
    }
}

void Construction::__clear_args() {
    for (auto& argptr : args_existing) { argptr.get()->clear(); }
    for (auto& argptr : args_new) { argptr.get()->clear(); }
}

std::tuple<std::string, std::string, std::string> Construction::parse_decl_string(const std::string c_decl) {
    int i = c_decl.find(" "), j = c_decl.find(":");
    if (i == 0 || i == std::string::npos || (j >= 0 && j <= i)) {
        throw InvalidTextualInputError("Error: Invalid construction declaration string: " + c_decl);
    }
    std::string name = c_decl.substr(0, i), args_new = c_decl.substr(i+1, j-i-1); 
    StrUtils::trim(args_new);
    std::string args_existing = "";
    if (j != std::string::npos) {
        args_existing = c_decl.substr(j+1);
        StrUtils::trim(args_existing);
    }
    return std::make_tuple(name, args_new, args_existing);
}

Generator<std::unique_ptr<Predicate>> Construction::__instantiate_preds(
    DDEngine &dd
) {
    // Not implemented
    co_return;
}

bool Construction::__instantiation_check(
    DDEngine &dd
) {
    // Not implemented
    return true;
}

Generator<std::unique_ptr<Predicate>> Construction::__instantiate_preds_no_checks(
    Predicate* base_pred
) {
    auto preconditions_ = preconditions.instantiate();
    while (preconditions_) {
        auto pre = std::move(preconditions_());
        pre.get()->why = {base_pred};
        co_yield std::move(pre);
    }

    for (auto& postptr : postconditions) {
        auto post = std::move(postptr.get()->instantiate());
        post.get()->why = {base_pred};
        co_yield std::move(post);
    }
}

Generator<std::unique_ptr<Numeric>> Construction::__instantiate_numerics() {
    for (auto& numptr : numerics) {
        co_yield std::move(numptr.get()->instantiate());
    }
    co_return;
}

void Construction::construct(
    const std::string cstage_string, DDEngine &dd, NumEngine &nm, GeometricGraph &ggraph
) {
    throw std::runtime_error("Construction::construct not implemented");
}

void Construction::construct_no_checks(
    const std::string cstage_string, DDEngine &dd, NumEngine &nm, GeometricGraph &ggraph
) {
    auto [_ps, _cs] = StrUtils::split_first(cstage_string, "=");
    StrUtils::trim(_ps);
    StrUtils::trim(_cs);

    std::vector<std::string> c_new_nodes_all = StrUtils::split(_ps, " ");   // unused

    // Text processing of construction stages

    std::vector<std::string> _construction_stage = StrUtils::split(_cs, ",");

    for (std::string _c : _construction_stage) {
        StrUtils::trim(_c);
        auto [c_name, c_new_nodes_str, c_existing_nodes_str] = parse_decl_string(_c);

        std::vector<std::string> c_existing_nodes = StrUtils::split(c_existing_nodes_str, " ");
        std::vector<std::string> c_new_nodes = StrUtils::split(c_new_nodes_str, " ");
        
        // Extract existing and create new Nodes from the GeometricGraph as arguments
        std::vector<Node*> nodes_existing;
        std::vector<Node*> nodes_new;
        for (std::string obj_ : c_existing_nodes) {
            nodes_existing.push_back(ggraph.get_or_add_point(obj_));
        }
        for (std::string obj_ : c_new_nodes) {
            nodes_new.push_back(ggraph.get_or_add_point(obj_));
        }

        Construction* c_template = dd.constructions.at(c_name).get();

        // Populate args with given nodes
        c_template->__set_node_args(nodes_existing, nodes_new);

        // Instantiate a new construction by generating its predicates and numerics
        auto pred_gen = c_template->__instantiate_preds_no_checks(
            dd.base_pred.get()
        );
        while (pred_gen) {
            dd.insert_predicate(std::move(pred_gen()));
        }
        auto num_gen = c_template->__instantiate_numerics();
        while (num_gen) {
            nm.insert_numeric(std::move(num_gen()));
        }

        // Clear args, ready for the next instantiation
        c_template->__clear_args();
    }
}

std::string Construction::to_string() {

    std::string s = "Construction: " + name + " ";

    for (auto& argptr : args_new) { s += argptr.get()->to_string() + " ";}
    s += ": ";
    for (auto& argptr : args_existing) { s += argptr.get()->to_string() + " ";}
    s += "\n";

    s += preconditions.to_string() + "\n";
    for (auto& postptr : postconditions) { s += postptr.get()->to_string() + ", "; }

    s = s.substr(0, s.size() - 2); // Remove trailing ", "

    return s;
}

std::string Construction::to_string_with_placeholders() {

    __set_placeholder_args();
    std::string s = to_string();
    __clear_args();

    return s;
}