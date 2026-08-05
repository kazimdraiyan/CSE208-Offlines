#include <bits/stdc++.h>
#include "edmonds_karp.cpp"

using namespace std;

int main() {
    int node_count;
    int edge_count;

    cin >> node_count >> edge_count;

    int source = 0;
    int sink = node_count - 1;

    vector<Edge> edges;
    for (int i = 0; i < edge_count; i++) {
        int from, to, capacity;
        cin >> from >> to >> capacity;
        edges.push_back(Edge(i, from, to, capacity));
    }

    FlowNetwork flow_network(node_count, source, sink, edges);

    int max_flow = flow_network.max_flow();
    cout << max_flow << endl;

    return 0;
}