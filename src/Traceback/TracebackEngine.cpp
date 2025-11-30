
#include "TracebackEngine.hh"

TracebackEngine::TracebackTree::TracebackTree(Predicate* goal) {
    nodes.emplace_back(std::make_unique<TracebackNode>(goal));
    root = nodes.back().get();
}

TracebackEngine::TracebackEngine(Predicate* goal) : traceback(goal) {};

