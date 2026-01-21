#pragma once

#include "Common/Constants.hh"
#include "Common/Utils.hh"
#include "Common/Arg.hh"
#include "Geometry/Object.hh"

class Numeric;

class NumericTemplate {
    std::string id;

public:
    num_t name;
    std::vector<Arg*> args;
    std::vector<Arg*> outs;

    NumericTemplate(const num_t name) : name(name) {}
    NumericTemplate(const std::string name) : name(Utils::to_num_t(name)) {}

    NumericTemplate(const num_t name, std::vector<Arg*> &args, std::vector<Arg*> &outs) 
    : name(name), args(std::move(args)), outs(std::move(outs)) {}
    NumericTemplate(const std::string name, std::vector<Arg*> &args, std::vector<Arg*> &outs) 
    : name(Utils::to_num_t(name)), args(std::move(args)), outs(std::move(outs)) {}

    // This constructor is used to parse Numeric strings from textual input.
    NumericTemplate(const std::string outs, const std::string num, std::map<std::string, Arg*> &argmap);
    
    char set_arg(int i, Node* node) noexcept;
    bool arg_empty(int i) const noexcept;
    void clear_arg(int i) noexcept;
    Point* get_arg_point(int i) const;

    char set_args(std::vector<Node*> nodes);
    bool args_filled() const;
    void clear_args();

    bool no_args();

    char set_out(int i, Node* node) noexcept;
    bool out_empty(int i) const noexcept;
    void clear_out(int i) noexcept;
    Point* get_out_point(int i) const;

    char set_outs(std::vector<Node*> nodes);
    bool outs_filled() const;
    void clear_outs();

    std::unique_ptr<Numeric> instantiate();

    std::string to_string() const;
    std::string to_hash_with_args() const;
};

class Numeric {
public:
    std::string hash;

    num_t name;
    std::vector<Point*> args;
    std::vector<Point*> outs;

    Numeric(const std::string s, std::map<std::string, std::unique_ptr<Point>> &global_point_map);
    Numeric(const num_t name, std::vector<Point*> &&args, std::vector<Point*> &&outs) : name(name), args(std::move(args)), outs(std::move(outs)) {
        hash = "";
        for (auto out : outs) {
            hash = hash + out->name + " ";
        }
        hash = hash + "= " + Utils::to_num_str(name);
        for (auto arg : args) {
            hash = hash + " " + arg->name;
        }
    }
    Numeric(const NumericTemplate &nt);

    bool is_base_numeric();
    std::string to_string() const;
};