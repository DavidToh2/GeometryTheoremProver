
#include <vector>
#include <string>
#include <memory>
#include <map>
#include <array>
#include <iostream>

#include "Construction.hh"
#include "Common/StrUtils.hh"
#include "Common/Exceptions.hh"
#include "Predicate.hh"
#include "Common/Generator.hh"
#include "Geometry/GeometricGraph.hh"
#include "DD/DDEngine.hh"

Construction::Construction(const std::string _c_decl, const std::string _c_pres, const std::string _c_posts) {

    std::map<std::string, Arg*> argmap;

    std::array<std::string, 3> parsed_decl = parse_decl_string(_c_decl);
    name = parsed_decl[0];
    Arg::populate_args_and_argmap(parsed_decl[1], args_new, argmap);
    Arg::populate_args_and_argmap(parsed_decl[2], args_existing, argmap);

    preconditions = ClauseTemplate(_c_pres, argmap);

    std::vector<std::string> c_posts = StrUtils::split(_c_posts, ", ");
    for (std::string _c_post : c_posts) {
        postconditions.emplace_back(std::make_unique<PredicateTemplate>(_c_post, argmap));
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

std::array<std::string, 3> Construction::parse_decl_string(const std::string c_decl) {
    int i = c_decl.find(" "), j = c_decl.find(" : ");
    if (i == 0 || i == std::string::npos || j == 0 || j == std::string::npos || j <= i) {
        throw InvalidTextualInputError("Error: Invalid construction declaration string: " + c_decl);
    }
    return std::array<std::string, 3>{c_decl.substr(0, i), c_decl.substr(i+1, j-i-1), c_decl.substr(j+3)};
}

Generator<std::unique_ptr<Predicate>> Construction::__instantiate(
    std::vector<Node*> &nodes_existing, std::vector<Node*> &nodes_new, DDEngine &dd
) {
    __set_node_args(nodes_existing, nodes_new);
    if (!__instantiation_check(nodes_existing, dd)) {
        __clear_args();
        co_return;
    }
    
    for (auto& postptr : postconditions) {
        co_yield postptr.get()->instantiate();
    }

    __clear_args();
    co_return;
}

bool Construction::__instantiation_check(
    std::vector<Node*> &nodes_existing, DDEngine &dd
) {
    std::cerr << "Construction: Instantiation checks for degeneracy conditions not implemented. Assuming success." << std::endl;
    return true;
}

Generator<std::unique_ptr<Predicate>> Construction::__instantiate_no_checks(
    std::vector<Node*> &nodes_existing, std::vector<Node*> &nodes_new, Predicate* base_pred
) {
    __set_node_args(nodes_existing, nodes_new);

    auto preconditions_ = preconditions.instantiate();
    while (preconditions_) {
        auto pre = std::move(preconditions_());
        pre.get()->why.insert(base_pred);
        co_yield std::move(pre);
    }
    for (auto& postptr : postconditions) {
        auto post = std::move(postptr.get()->instantiate());
        post.get()->why.insert(base_pred);
        co_yield std::move(post);
    }

    __clear_args();
    co_return;
}

void Construction::construct(
    const std::string c_string, DDEngine &dd, GeometricGraph &ggraph
) {

    // Parse the construction string to extract the construction name and arguments.
    std::array<std::string, 3> parsed = parse_decl_string(c_string);

    std::string c_name = parsed[0];
    std::vector<std::string> c_existing_nodes = StrUtils::split(parsed[2], " ");
    std::vector<std::string> c_new_nodes = StrUtils::split(parsed[1], " ");
    std::vector<Node*> nodes_existing;
    std::vector<Node*> nodes_new;

    for (std::string obj_ : c_existing_nodes) {
        nodes_existing.push_back(ggraph.get_or_add_point(obj_));
    }
    for (std::string obj_ : c_new_nodes) {
        nodes_new.push_back(ggraph.get_or_add_point(obj_));
    }

    Construction* c_template = dd.constructions.at(c_name).get();

    // Create a new Construction object based on the template.
    auto gen = c_template->__instantiate(
        nodes_existing, nodes_new, dd
    );
    while (gen) {
        dd.insert_predicate(std::move(gen()));
    }
}

void Construction::construct_no_checks(
    const std::string c_string, DDEngine &dd, GeometricGraph &ggraph
) {

    // Parse the construction string to extract the construction name and arguments.
    std::array<std::string, 3> parsed = parse_decl_string(c_string);

    std::string c_name = parsed[0];
    std::vector<std::string> c_existing_nodes = StrUtils::split(parsed[2], " ");
    std::vector<std::string> c_new_nodes = StrUtils::split(parsed[1], " ");
    std::vector<Node*> nodes_existing;
    std::vector<Node*> nodes_new;

    for (std::string obj_ : c_existing_nodes) {
        nodes_existing.push_back(ggraph.get_or_add_point(obj_));
    }
    for (std::string obj_ : c_new_nodes) {
        nodes_new.push_back(ggraph.get_or_add_point(obj_));
    }

    Construction* c_template = dd.constructions.at(c_name).get();

    // Create a new Construction object based on the template.
    auto gen = c_template->__instantiate_no_checks(
        nodes_existing, nodes_new, dd.base_pred.get()
    );
    int i=0;
    while (gen) {
        i++;
        dd.insert_predicate(std::move(gen()));
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