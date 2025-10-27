// Test cases
#include "karger.hpp"
#include <iostream>
using namespace karger;

int main() {
    std::vector<karger::Edge> edges = {
        {0,1}, {0,2}, {1,2}, {1,3}, {2,3}
    };
    int n = 4;

    std::cout << "Running Randomised Karger:\n";
    karger::minCutRandomised(n, edges, 123);

    std::cout << "\nRunning Deterministic (Fixed Permutation):\n";
    karger::minCutFixedPermutation(n, edges);

    return 0;
}
