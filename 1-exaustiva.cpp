#include <algorithm>
#include <fstream>
#include <iostream>
#include <omp.h>
#include <vector>
#include "ReadGraph.cpp"

// Function to check if a node belongs to the clique
std::tuple<std::vector<int>, std::vector<int>> CliqueCheck(
    const std::vector<std::vector<int>> &graph,
    const std::vector<int> &candidates,
    const std::vector<int> &currentClique,
    int vertex) {
    std::vector<int> newCandidates = candidates;
    newCandidates.erase(std::remove(newCandidates.begin(), newCandidates.end(), vertex), newCandidates.end());

    bool canAdd = true;
    for (int u : currentClique) {
        if (graph[u][vertex] == 0) {
            canAdd = false;
            break;
        }
    }

    std::vector<int> newClique = currentClique;
    if (canAdd) {
        newClique.push_back(vertex);

        std::vector<int> updatedCandidates;
        for (int u : newCandidates) {
            bool adjacentToAll = true;
            for (int c : newClique) {
                if (graph[u][c] == 0) {
                    adjacentToAll = false;
                    break;
                }
            }
            if (adjacentToAll) {
                updatedCandidates.push_back(u);
            }
        }
        newCandidates = updatedCandidates;
    }

    return std::make_tuple(newClique, newCandidates);
}

// Function to find the maximum clique in a graph
std::vector<int> FindMaximumClique(
    const std::vector<std::vector<int>> &graph,
    const std::vector<int> &candidates,
    const std::vector<int> &currentClique) {
    std::vector<int> maximumClique = currentClique;

    for (int candidate : candidates) {
        std::vector<int> newCandidates;
        std::vector<int> newClique;
        std::tie(newClique, newCandidates) = CliqueCheck(graph, candidates, currentClique, candidate);

        if (!newCandidates.empty()) {
            newClique = FindMaximumClique(graph, newCandidates, newClique);
        }

        if (newClique.size() >= maximumClique.size()) {
            maximumClique = newClique;
        }
    }

    return maximumClique;
}

int main() {
    int numVertices = 45;
    std::vector<std::vector<int>> graph;
    graph = ReadGraph("grafo.txt", numVertices);

    std::vector<int> allVertices;
    for (int i = 0; i < numVertices; ++i) {
        allVertices.push_back(i);
    }

    std::vector<int> initialClique;

    double startTime = omp_get_wtime();

    std::vector<int> maximumClique = FindMaximumClique(graph, allVertices, initialClique);

    double duration = omp_get_wtime() - startTime;

    std::cout << "Maximum clique: ";
    for (int u : maximumClique) {
        std::cout << u + 1 << " ";
    }
    std::cout << std::endl;

    std::cout << "Duration: " << duration << " s";

    return 0;
}
