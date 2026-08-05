#include <bits/stdc++.h>

using namespace std;

struct Edge
{
    int id;
    int from, to;
    int capacity, flow;

    Edge() : id(-1), from(-1), to(-1), capacity(-1), flow(-1) {}

    Edge(int id, int from, int to, int capacity) : id(id), from(from), to(to), capacity(capacity), flow(0) {}

    int residue_capacity()
    {
        return capacity - flow;
    }
};

struct AugmentingPath
{
    vector<int> path; // Stores edge ids
    int min_residue_capacity;

    AugmentingPath()
    {
        min_residue_capacity = INT_MAX;
    }

    bool exists()
    {
        return path.size() > 0;
    }
};

class FlowNetwork
{
    int node_count, original_edge_count;
    int source, sink;
    vector<Edge> edges;
    vector<vector<int>> adj_list; // Stores edge ids from each vertex

    int reverse_of(int edge_id)
    {
        if (edge_id < original_edge_count)
        {
            return original_edge_count + edge_id;
        }
        else
        {
            return edge_id - original_edge_count;
        }
    }

    // parent_edge must contain a path between source and target
    void build_augmenting_path(AugmentingPath &aug_path, vector<int> &parent_edge, int to)
    {
        if (to == source)
            return;

        Edge &edge = edges[parent_edge[to]];
        int parent = edge.from;
        build_augmenting_path(aug_path, parent_edge, parent);

        aug_path.min_residue_capacity = min(aug_path.min_residue_capacity, edge.residue_capacity());
        aug_path.path.push_back(parent_edge[to]);
    }

    AugmentingPath find_augmenting_path_bfs()
    {
        vector<bool> is_visited(node_count, false); // Is in queue
        vector<int> parent_edge(node_count, -1);    // Stores parent edge id

        queue<int> q;
        q.push(source);
        is_visited[source] = true;

        bool sink_found = false;
        while (!(q.empty() || sink_found))
        {
            int current_node = q.front();
            q.pop();

            for (int edge_id : adj_list[current_node])
            {
                int neighbor = edges[edge_id].to;
                if (edges[edge_id].residue_capacity() > 0 && !is_visited[neighbor])
                {
                    // Edge capacity being 0 is equivalent to the edge not existing
                    parent_edge[neighbor] = edge_id;
                    q.push(neighbor);
                    is_visited[neighbor] = true;

                    if (neighbor == sink)
                    {
                        sink_found = true;
                        break;
                    }
                }
            }
        }

        AugmentingPath aug_path;
        if (sink_found)
        {
            build_augmenting_path(aug_path, parent_edge, sink);
        }
        return aug_path;
    }

public:
    FlowNetwork(int node_count, int source, int sink, vector<Edge> &original_edges)
    {
        this->node_count = node_count;
        this->original_edge_count = original_edges.size();
        this->source = source;
        this->sink = sink;
        this->edges = original_edges;

        // Add reverse edges
        for (int i = 0; i < original_edge_count; i++)
        {
            Edge &edge = edges[i];
            edges.push_back(Edge(original_edge_count + i, edge.to, edge.from, 0));
        }

        // Build adjacency list
        adj_list.assign(node_count, vector<int>());
        for (Edge &edge : edges)
        {
            adj_list[edge.from].push_back(edge.id);
        }
    }

    // Using the Edmonds-Karp algorithm
    int max_flow()
    {
        AugmentingPath aug_path = find_augmenting_path_bfs();

        while (aug_path.exists())
        {
            // Augment the path
            for (int edge_id : aug_path.path)
            {
                Edge &edge = edges[edge_id];
                edge.flow += aug_path.min_residue_capacity;

                Edge &reverse_edge = edges[reverse_of(edge_id)];
                reverse_edge.flow -= aug_path.min_residue_capacity;
            }
            aug_path = find_augmenting_path_bfs();
        }

        int total_flow = 0;
        for (int edge_id : adj_list[source])
        {
            if (edge_id < original_edge_count)
                total_flow += edges[edge_id].flow;
        }
        return total_flow;
    }

    vector<Edge> get_edges() {
        return edges;
    }
};
