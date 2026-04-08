
#include <iomanip>

#include "OutputParser.hh"
#include "Common/Constants.hh"
#include "Geometry/Node.hh"
#include "DD/DDEngine.hh"

void OutputParser::format_problem_description(std::string problem_name, std::string problem_string, std::ostream &os) {
    os << std::endl;
    os << "==========================================" << std::endl;
    os << "Problem: " << problem_name << std::endl;
    os << problem_string << std::endl;
}





std::string OutputParser::__format_predicate(Predicate* pred) {
    std::string p1, p2, p3, p4, p5, p6, p7, p8, ang1, ang2;
    switch(pred->name) {
        case pred_t::COLL:
            return pred->args[0]->to_string() + ", " + pred->args[1]->to_string() 
            + ", " + pred->args[2]->to_string() + " collinear";
        case pred_t::CYCLIC:
            return pred->args[0]->to_string() + ", " + pred->args[1]->to_string() 
            + ", " + pred->args[2]->to_string() + ", " + pred->args[3]->to_string() + " cyclic";
        case pred_t::CIRCLE:
            return pred->args[0]->to_string() + " center of ⦿" + pred->args[1]->to_string() 
            + pred->args[2]->to_string() + pred->args[3]->to_string();
        case pred_t::PARA:
            return pred->args[0]->to_string() + pred->args[1]->to_string() 
            + " ∥ " + pred->args[2]->to_string() + pred->args[3]->to_string();
        case pred_t::PERP:
            return pred->args[0]->to_string() + pred->args[1]->to_string() 
            + " ⊥ " + pred->args[2]->to_string() + pred->args[3]->to_string();
        case pred_t::CONG:
            return pred->args[0]->to_string() + pred->args[1]->to_string() 
            + " = " + pred->args[2]->to_string() + pred->args[3]->to_string();
        case pred_t::EQANGLE:
            p1 = pred->args[0]->to_string(), p2 = pred->args[1]->to_string(), p3 = pred->args[2]->to_string(), p4 = pred->args[3]->to_string(),
                p5 = pred->args[4]->to_string(), p6 = pred->args[5]->to_string(), p7 = pred->args[6]->to_string(), p8 = pred->args[7]->to_string();
            if (p1 == p3) ang1 = "∠" + p2 + p1 + p4;
            else if (p1 == p4) ang1 = "∠" + p2 + p1 + p3;
            else if (p2 == p3) ang1 = "∠" + p1 + p2 + p4;
            else if (p2 == p4) ang1 = "∠" + p1 + p2 + p3;
            else ang1 = "∠(" + p1 + p2 + ", " + p3 + p4 + ")";
            if (p5 == p7) ang2 = "∠" + p6 + p5 + p8;
            else if (p5 == p8) ang2 = "∠" + p6 + p5 + p7;
            else if (p6 == p7) ang2 = "∠" + p5 + p6 + p8;
            else if (p6 == p8) ang2 = "∠" + p5 + p6 + p7;
            else ang2 = "∠(" + p5 + p6 + ", " + p7 + p8 + ")";
            return ang1 + " = " + ang2;
        case pred_t::EQRATIO:
            return pred->args[0]->to_string() + pred->args[1]->to_string() 
            + " / " + pred->args[2]->to_string() + pred->args[3]->to_string() + " = "
            + pred->args[4]->to_string() + pred->args[5]->to_string() 
            + " / " + pred->args[6]->to_string() + pred->args[7]->to_string();
        case pred_t::CONTRI:
            return "△" + pred->args[0]->to_string() + pred->args[1]->to_string() + pred->args[2]->to_string()
            + " ≅ △" + pred->args[3]->to_string() + pred->args[4]->to_string() + pred->args[5]->to_string();
        case pred_t::SIMTRI:
            return "△" + pred->args[0]->to_string() + pred->args[1]->to_string() + pred->args[2]->to_string()
            + " ~ △" + pred->args[3]->to_string() + pred->args[4]->to_string() + pred->args[5]->to_string();
        case pred_t::MIDP:
            return pred->args[0]->to_string() + " midpoint of " + pred->args[1]->to_string() + pred->args[2]->to_string();
        case pred_t::CONSTANGLE:
            p1 = pred->args[0]->to_string(), p2 = pred->args[1]->to_string(), p3 = pred->args[2]->to_string(), p4 = pred->args[3]->to_string();
            if (p1 == p3) ang1 = "∠" + p2 + p1 + p4;
            else if (p1 == p4) ang1 = "∠" + p2 + p1 + p3;
            else if (p2 == p3) ang1 = "∠" + p1 + p2 + p4;
            else if (p2 == p4) ang1 = "∠" + p1 + p2 + p3;
            else ang1 = "∠(" + p1 + p2 + ", " + p3 + p4 + ")";
            return ang1 + " = " + pred->frac_arg.to_string() + "°";
        case pred_t::CONSTRATIO:
            return pred->args[0]->to_string() + pred->args[1]->to_string()
            + " / " + pred->args[2]->to_string() + pred->args[3]->to_string() + " = " + pred->frac_arg.to_string();
        default:
            // reached by DIFF, NCOLL, NPARA, SAMECLOCK, DIFFCLOCK, SAMESIDE_P, DIFFSIDE_P, CONVEX, BASE
            return pred->to_string();
    }
}
std::string OutputParser::format_predicate_with_why(Predicate* pred, Predicate* base_pred) {
    std::string res;
    for (Predicate* why : pred->why.preds) {
        if (why == base_pred) continue;
        std::string pred_str = __format_predicate(why);
        if (!pred_str.empty()) {
            res += pred_str + " && ";
        }
    }
    if (!res.empty()) res.pop_back(), res.pop_back(), res.pop_back(), res.pop_back();
    if (!res.empty()) res += " => ";
    res += __format_predicate(pred);
    return res;
}


void OutputParser::format_solution_from_predset(
    std::map<int, std::set<Predicate*>>& predset, DDEngine& dd, std::ostream &os
) {
    Predicate* base_pred = dd.base_pred.get();
    os << "==========================================" << std::endl;
    for (const auto& [level, preds] : predset) {
        for (Predicate* p : preds) {
            if (p->source <= pred_src::GGRAPH) continue;
            std::string pred_str = format_predicate_with_why(p, base_pred);
            if (!pred_str.empty()) {
                os << "[ "
                    << std::right << std::setw(3) << level << " | "
                    << std::left << std::setw(2) << Utils::to_pred_src_str(p->source) << " ] "
                    << pred_str << "\n";
            }
        }
    }
}