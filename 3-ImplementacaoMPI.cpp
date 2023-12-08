#include "ReadGraph.cpp"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <mpi.h>
#include <tuple>
#include <vector>

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

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size != 2) {
        std::cerr << "This example assumes 2 MPI processes." << std::endl;
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    int numVertices = 50;
    std::vector<std::vector<int>> graph;
    std::vector<int> allVertices;
    std::vector<int> initialClique;

    if (rank == 0) {
        graph = ReadGraph("grafo.txt", numVertices);

        for (int i = 0; i < numVertices; ++i) {
            allVertices.push_back(i);
        }
    }

    MPI_Bcast(&numVertices, 1, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank == 1) {
        graph.resize(numVertices, std::vector<int>(numVertices, 0));
    }

    MPI_Bcast(graph.data(), numVertices * numVertices, MPI_INT, 0, MPI_COMM_WORLD);

    double startTime = MPI_Wtime();

    std::vector<int> localClique = FindMaximumClique(graph, allVertices, initialClique);

    double duration = MPI_Wtime() - startTime;

    if (rank == 0) {
        std::vector<int> remoteClique(size);

        MPI_Recv(remoteClique.data(), size, MPI_INT, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        // Combine results from different processes
        if (remoteClique.size() > localClique.size()) {
            localClique = remoteClique;
        }

        std::cout << "Maximum clique: ";
        for (int u : localClique) {
            std::cout << u + 1 << " ";
        }
        std::cout << std::endl;

        std::cout << "Duration: " << duration << " s" << std::endl;
    } else if (rank == 1) {
        MPI_Send(localClique.data(), localClique.size(), MPI_INT, 0, 0, MPI_COMM_WORLD);
    }

    MPI_Finalize();

    return 0;
}
