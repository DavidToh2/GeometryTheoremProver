
#pragma once

#include <vector>
#include <map>
#include <string>

class Profiler {

    struct NumEngineProfile {
        int num_params = 0;
        long duration;
    };

    struct DDEngineProfile {
        std::map<std::string, std::vector<int>> theorem_matches;
        std::map<std::string, std::vector<long>> theorem_duration;

        std::vector<long> duration;
        std::vector<int> total_preds;
    };
    struct AREngineProfile {
        std::vector<int> angle_table_eqs;
        std::vector<int> ratio_table_eqs;
        std::vector<int> displacement_table_eqs;

        std::vector<long> duration;
        std::vector<int> total_cols;
        std::vector<int> total_rows;
    };
    struct GeometricGraphProfile {
        std::vector<int> num_preds_dd;
        std::vector<int> num_preds_ar;

        std::vector<long> duration_dd;
        std::vector<long> duration_ar;
        std::vector<int> total_nodes;
        long total_duration;
        int iterations;
    };

    struct TracebackEngineProfile {
        int solution_depth;
        int solution_length;
        long duration;
    };

public:
    NumEngineProfile nm_p;
    AREngineProfile ar_p;
    DDEngineProfile dd_p;
    GeometricGraphProfile ggraph_p;
    TracebackEngineProfile tr_p;

    bool num_success = false;
    bool solved = false;
    bool extracted_solution = false;
};