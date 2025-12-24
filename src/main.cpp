#include <getopt.h>
#include <iostream>

#include "GTPEngine.hh"

int main(int argc, char** argv) {

    // Parse input arguments

    if ( (argc <= 1) || (argv[argc-1] == 0) || (argv[argc-1][0] == '-') ) {
        std::cerr << "Error: No argument provided!" << std::endl;
        return 1;
    }

    static struct option options[] = {
        {"problem_file", required_argument, 0, 'f'},
        {"problem_name", required_argument, 0, 'p'},
        {"rule_file", required_argument, 0, 'r'},
        {"construction_file", required_argument, 0, 'c'},
        {"output_file", required_argument, 0, 'o'},
        {0, 0, 0, 0}
    };

    std::string input_filepath="", 
        problem_name="", 
        rule_filepath="rules.txt", 
        construction_filepath="constructions.txt", 
        output_filepath="";

    int opt, optindex;
    while ( (opt = getopt_long(argc, argv, "f:p:r:c:o:", options, &optindex)) != -1 ) {
        fprintf(stderr, "%s\n", optarg);
        switch(opt) {
            case 'f':
                input_filepath = std::string(optarg);
                break;
            case 'p':
                problem_name = std::string(optarg);
                break;
            case 'r':
                rule_filepath = std::string(optarg);
                break;
            case 'c':
                construction_filepath = std::string(optarg);
                break;
            case 'o':
                output_filepath = std::string(optarg);
                break;
            default:
                std::cerr << "Error: Invalid argument found!" << std::endl;
                return 1;
        }
    }

    if (input_filepath.empty() || output_filepath.empty()) {
        std::cerr << "Error: --problem_file and --output_file must be provided." << std::endl;
        return 1;
    } else {
        std::cerr << "Inputs received!\n";
    }

    GTPEngine gtp(
        rule_filepath,
        construction_filepath
    );

    if (problem_name.empty()) {
        // Iterate through every single problem in the input file
        std::vector<std::string> problem_names = gtp.inputParser.extract_all_problem_names_from_file(input_filepath);
        for (std::string problem_name : problem_names) {
            gtp.load_problem(
                input_filepath,
                problem_name
            );

            gtp.solve(10);

            gtp.output(
                output_filepath
            );

            gtp.clear_problem();
        }
    } else {
        // Solve the specified problem
        gtp.load_problem(
            input_filepath,
            problem_name
        );

        gtp.solve(10);

        gtp.output(
            output_filepath
        );
    }
}