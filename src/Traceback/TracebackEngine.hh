
#pragma once

#include <memory>
#include <vector>

#include "DD/Predicate.hh"
#include "DD/DDEngine.hh"
#include "Geometry/GeometricGraph.hh"

class TracebackEngine {

private:
    struct TracebackNode {
        Predicate* node;
        std::vector<TracebackNode*> why;

        TracebackNode(Predicate* p) : node(p) {};
    };

    struct TracebackTree {
        std::vector<std::unique_ptr<TracebackNode>> nodes;
        TracebackNode* root;

        TracebackTree(Predicate* goal);
    };

public:
    TracebackEngine(Predicate* goal);
    TracebackTree traceback;
};