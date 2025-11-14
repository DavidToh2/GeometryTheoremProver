
#include <vector>
#include <string>
#include <memory>
#include <map>
#include <algorithm>

#include "Construction.hh"
#include <Common/StrUtils.hh>
#include "Predicate.hh"

Construction::Construction(std::string s) {

    std::vector<std::string> v = split(s, "\n");
    std::map<std::string, Arg*> argmap;

    std::string _c_decl = v[0];
    int i = _c_decl.find(" "), j = _c_decl.find(":");
    Arg::populate_args_and_argmap(_c_decl.substr(i+1, j-i-1), args_existing, argmap);
    Arg::populate_args_and_argmap(_c_decl.substr(j+1), args_new, argmap);

    std::string _c_pres = v[1];
    preconditions = Clause(_c_pres, argmap);

    std::vector<std::string> _c_posts = split(v[2], ", ");
    for (std::string _c_post : _c_posts) {
        postconditions.emplace_back(std::make_unique<Predicate>(_c_post, argmap));
    }
}