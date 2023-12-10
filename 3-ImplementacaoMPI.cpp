#include <algorithm>
#include <fstream>
#include <iostream>
#include <mpi.h>
#include <vector>
#include "ReadGraph.cpp"

using namespace std;

tuple<vector<int>, vector<int>> CliqueCheck(
    const vector<vector<int>> &graph,
    const vector<int> &candidates,
    const vector<int> &currentClique,
    int vertex) {
    vector<int> newCandidates = candidates;
    newCandidates.erase(remove(newCandidates.begin(), newCandidates.end(), vertex), newCandidates.end());

    bool canAdd = true;
    for (int u : currentClique) {
        if (graph[u][vertex] == 0) {
            canAdd = false;
            break;
        }
    }

    vector<int> newClique = currentClique;
    if (canAdd) {
        newClique.push_back(vertex);

        vector<int> updatedCandidates;
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

    return make_tuple(newClique, newCandidates);
}

vector<int> FindMaximumClique(
    const vector<vector<int>> &graph,
    const vector<int> &candidates,
    const vector<int> &currentClique) {
    vector<int> maximumClique = currentClique;

    // Poda por limites
    if (currentClique.size() + candidates.size() <= maximumClique.size()) {
        return maximumClique;
    }

    for (int candidate : candidates) {
        vector<int> newCandidates;
        vector<int> newClique;
        tie(newClique, newCandidates) = CliqueCheck(graph, candidates, currentClique, candidate);

        if (!newCandidates.empty()) {
            newClique = FindMaximumClique(graph, newCandidates, newClique);
        }

        if (newClique.size() >= maximumClique.size()) {
            maximumClique = newClique;
        }
    }

    return maximumClique;
}

int main(int argc, char **argv) {

    int numVertices = 40;
    vector<vector<int>> graph;

    // Assuming ReadGraph does what LerGrafo does
    graph = ReadGraph("grafo_gerado.txt", numVertices);

    vector<int> candidates;
    for (int i = 0; i <= numVertices - 1; i++) {
        candidates.push_back(i);
    }

    int i;
    MPI_Status status;
    int myStart = 0, myEnd = 0;
    int candPerProc = 0;
    vector<int> MaximumClique;
    vector<int> PartialMaximumClique;
    vector<int> MaximumCliqueSizeVector;
    int MaximumCliqueSize;
    int size;

    MPI_Init(&argc, &argv);
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0) {
        candPerProc = candidates.size() / size;
        for (i = 1; i < size; i++) {
            MPI_Send(&candPerProc, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
        }
    } else {
        MPI_Recv(&candPerProc, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
    }

    myStart = rank * candPerProc;
    myEnd = myStart + candPerProc;
    for (i = myStart; i < myEnd; i++) {
        vector<int> newCandidates;
        vector<int> newClique;
        tie(newClique, newCandidates) = CliqueCheck(graph, candidates, newClique, i);

        if (!newCandidates.empty()) {
            newClique = FindMaximumClique(graph, newCandidates, newClique);
        }

        if (newClique.size() >= MaximumClique.size()) {
            MaximumClique = newClique;
        }
    }

    if (rank != 0) {
        MaximumCliqueSize = MaximumClique.size();
        MPI_Send(&MaximumCliqueSize, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    } else {
        for (i = 1; i < size; i++) {
            MPI_Recv(&MaximumCliqueSize, 1, MPI_INT, i, 0, MPI_COMM_WORLD, &status);
            MaximumCliqueSizeVector.push_back(MaximumCliqueSize);
        }
    }

    if (rank != 0) {
        MPI_Send(&MaximumClique[0], MaximumClique.size(), MPI_INT, 0, 0, MPI_COMM_WORLD);
    } else {
        for (i = 1; i < size; i++) {
            PartialMaximumClique.resize(MaximumCliqueSizeVector[i - 1]);
            MPI_Recv(&PartialMaximumClique[0], PartialMaximumClique.size(), MPI_INT, i, 0, MPI_COMM_WORLD, &status);
            if (PartialMaximumClique.size() >= MaximumClique.size()) {
                MaximumClique = PartialMaximumClique;
            }
        }
        sort(MaximumClique.begin(), MaximumClique.end());
        cout << "Maximum clique size: " << MaximumClique.size() << endl;
        cout << "Maximum clique: "
             << ": [";
        for (auto &v : MaximumClique) {
            if (&v == &MaximumClique.back()) {
                cout << v + 1;
            } else {
                cout << v + 1 << ", ";
            }
        }
        cout << "]" << endl;
    }

    MPI_Finalize();
    return 0;
}
