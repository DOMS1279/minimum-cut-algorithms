/* Dom S
I have chosen to create and implement a Randomistic Karger Algorithm. - Randomised Karger Min Cut
This algorithm contracts randomly chosen edges repeatedly until it identifies only 2 supernodes. 
The edges crossing the final supernodes signifies the minimum cut.

I chose this because it’s simple to implement, clearly shows how
randomisation is able to speed up graph algorithms, and allows direct
comparison with deterministic variants. It also aligns with the
course focus on randomised algorithms in the second half.
*/
#include "karger.hpp"
#include <random>
#include <iostream>
#include <algorithm>
#include <vector>

/* Dom S - Algorithm 1 - Randomised Karger Min Cut */

int karger::minCutRandomised(int n, const std::vector<Edge>& edges, std::uint64_t seed) {
    if (n <= 1 || edges.empty()) return 0;

    std::vector<int> parent(n), rank(n, 0);
    for (int i = 0; i < n; ++i) parent[i] = i;

    auto findParent = [&](int x) {
        while (parent[x] != x) {
            parent[x] = parent[parent[x]];
            x = parent[x];
        }
        return x;
    };

    auto unionSets = [&](int a, int b) {
        a = findParent(a);
        b = findParent(b);
        if (a == b) return false;
        if (rank[a] < rank[b]) std::swap(a, b);
        parent[b] = a;
        if (rank[a] == rank[b]) rank[a]++;
        return true;
    };

    std::mt19937_64 rng(seed);
    std::uniform_int_distribution<std::size_t> pick(0, edges.size() - 1);

    int supernodes = n;
    while (supernodes > 2) {
        const auto& e = edges[pick(rng)];
        int a = findParent(e.u);
        int b = findParent(e.v);
        if (a == b) continue;
        unionSets(a, b);
        --supernodes;
    }

    int repA = -1, repB = -1;
    for (int i = 0; i < n; ++i) {
        int r = findParent(i);
        if (repA == -1) repA = r;
        else if (r != repA) { repB = r; break; }
    }
    if (repB == -1) return 0;

    int cutSize = 0;
    for (const auto& e : edges) {
        int a = findParent(e.u);
        int b = findParent(e.v);
        if ((a == repA && b == repB) || (a == repB && b == repA))
            ++cutSize;
    }

    std::cout << "Final cut size (Randomised Karger, seed " << seed << "): "
              << cutSize << "\n";
    return cutSize;

/* Dom C
I have chosen to implement the Deterministic Karger – Fixed Permutation algorithm.

This algorithm is a derandomised version of the classic Karger's random contraction algorithm, for finding the minimunm cut of
an underirected graph. The algorithm works by contracting edges in a fixed order, rather than randomly selecting edges to contract.

I selected this algorithm because it:
- clearly demonstrates how randomness in algorithms can be removed through deterministic ordering
- retains the simplictiy of the original Karger algorithm (edge contradtion)
- is straightforward to implement and test using disjoint sets (union-find data structure)
- allows a direct comparison with the randomised version of Karger's algorithm

My though process for the implementation is as follows:
1) Represent the graph as a list of undirected edges
2) Sort these edges deterministically based on vertex IDs (this replaces randomness)
3) Repeatedly contract edges in that fixed order until only two supernodes remain
4) Count how many edges still connect the two remaining components - this is the cut size
*/
namespace karger {
    // find function for disjoint set
    int findParent(std::vector<int>& parent, int x) {
        if (parent[x] == x) return x; // if vertex is its own parent, its the root
        return parent[x] = findParent(parent, parent[x]);
    }

    // union function for disjoint set
    void unionSets(std::vector<int>& parent, std::vector<int>& rank, int a, int b) {
        a = findParent(parent, a);
        b = findParent(parent, b);
        if (a == b) return; // already in the same set

        if (rank[a] < rank[b]) std::swap(a, b); // attach smaller rank tree under root of higher rank tree
        parent[b] = a; // make a the parent of b
        if (rank[a] == rank[b]) rank[a]++; // if both trees had same rank, increment rank of new root
    }

    int minCutFixedPermutation(int n, const std::vector<Edge>& edges) {
        if (n <= 1) return 0; // no cut possible
        if (edges.empty()) return 0; // no edges, no cut

        // Step 1 - create disjoint set for all vertices
        std::vector<int> parent(n);
        std::vector<int> rank(n, 0);
        for (int i = 0; i < n; ++i) parent[i] = i; // each vertex is its own parent initially

        // Step 2 - create a fixed (input-derived) perumutation, and sort edges by (min(u,v), max(u,v)) to remove randomness
        std::vector<Edge> sortedEdges = edges;
        std::sort(sortedEdges.begin(), sortedEdges.end(), [](const Edge& a, const Edge& b) {
            int au = std::min(a.u, a.v);
            int av = std::max(a.u, a.v);
            int bu = std::min(b.u, b.v);
            int bv = std::max(b.u, b.v);
            if (au == bu) return av < bv;
            return au < bu;
        });

        // Step 3 - contract edges following the fixed order
        int vertices = n;
        for (const Edge& edge : sortedEdges) {
            if (vertices <= 2) break; // stop when only two supernodes remain
            int a = findParent(parent, edge.u);
            int b = findParent(parent, edge.v);
            if (a != b) {
                unionSets(parent, rank, a, b);
                vertices--;
            }
        }

        // Step 4 - determine which supernodes remain
        std::vector<int> supernodes;
        for (int i = 0; i < n; ++i) {
            int p = findParent(parent, i);
            if (std::find(supernodes.begin(), supernodes.end(), p) == supernodes.end()) {
                supernodes.push_back(p);
            }
        }

        if (supernodes.size() < 2) return 0; // no cut possible

        int supernodeA = supernodes[0];
        int supernodeB = supernodes[1];

        // Step 5 - count crossing edges between the two remaining supernodes
        int cutSize = 0;
        for (const auto& e : edges) {
            int a = findParent(parent, e.u);
            int b = findParent(parent, e.v);
            if (a == supernodeA && b == supernodeB) cutSize++;
            else if (a == supernodeB && b == supernodeA) cutSize++;
        }

        std::cout << "Final cut size (deterministic Karger - fixed permutation): " << cutSize << "\n";
        return cutSize;
    }
}
