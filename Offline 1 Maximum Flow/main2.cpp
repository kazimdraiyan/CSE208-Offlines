#include <bits/stdc++.h>
#include "edmonds_karp.cpp"

using namespace std;

const int turnaround_time = 3 * 60; // 3 hours

struct Flight {
    string id;
    string from_city;
    string to_city;
    int from_time;
    int to_time;
    // Time in minutes

    Flight(string id, string from_city, string to_city, int from_time, int to_time) {
        this->id = id;
        this->from_city = from_city;
        this->to_city = to_city;
        this->from_time = from_time;
        this->to_time = to_time;
    }

    // An aircraft can perform this flight after [other]
    bool after(Flight other) {
        return other.to_city == from_city && other.to_time + turnaround_time <= from_time;
    }

    void print() {
        cout << id << " " << from_city << " " << to_city << " " << from_time << " " << to_time << endl;
    }
};

int main() {
    // * Take input
    int flight_count;
    cin >> flight_count;

    vector<Flight> flights;
    for (int i = 0; i < flight_count; i++) {
        string id;
        string from_city, to_city;
        int from_hour, from_min, to_hour, to_min;
        char dump; // Dump the colon
        cin >> id >> from_city >> to_city;
        cin >> from_hour >> dump >> from_min;
        cin >> to_hour >> dump >> to_min;
        flights.push_back(Flight(id, from_city, to_city, from_hour * 60 + from_min, to_hour * 60 + to_min));
    }

    // * Create bipartite graph
    vector<Edge> edges;

    // Insert possible sequence pairs
    for (int i = 0; i < flight_count; i++) {
        int left_flight_id = i;
        for (int j = 0; j < flight_count; j++) {
            int right_flight_id = flight_count + j;

            if (flights[j].after(flights[i])) {
                // Edge id is the index of it in edges
                edges.push_back(Edge(edges.size(), left_flight_id, right_flight_id, 1));
            }
        }
    }
    int possible_pair_count = edges.size();

    // * Create flow network
    // Arbitrary source and sink id
    int source = flight_count * 2;
    int sink = flight_count * 2 + 1;
    int node_count = flight_count * 2 + 2;

    // Add edges from "source" to each "left_flight" and from each "right_flight" to "sink"
    for (int i = 0; i < flight_count; i++) {
        int left_flight_id = i;
        edges.push_back(Edge(edges.size(), source, left_flight_id, 1));

        int right_flight_id = flight_count + i;
        edges.push_back(Edge(edges.size(), right_flight_id, sink, 1));
    }

    FlowNetwork flow_network(node_count, source, sink, edges);
    int max_matching = flow_network.max_flow();
    int min_aircraft_count = flight_count - max_matching;
    cout << "Number of Aircraft: " << min_aircraft_count << endl;

    // * Backtracking
    vector<Edge> forward_edges = flow_network.get_edges();
    vector<int> next_flight(flight_count, -1);
    vector<int> previous_flight(flight_count, -1);

    // Track used edges
    for (int i = 0; i < possible_pair_count; i++) {
        Edge &edge = forward_edges[i];
        if (edge.flow == 1) {
            // This edge is used in minimum cover generation
            int from_flight = edge.from;
            int to_flight = edge.to - flight_count;
            next_flight[from_flight] = to_flight;
            previous_flight[to_flight] = from_flight;
        }
    }

    // Print aircraft routes
    int aircraft_count = 0;
    for (int flight = 0; flight < flight_count; flight++) {
        if (previous_flight[flight] == -1) {
            // No previous flight, an aircraft starts from here
            cout << "Aircraft " << ++aircraft_count << ": " << flights[flight].id;
            int next = next_flight[flight];
            while (next != -1) {
                cout << " -> " << flights[next].id;
                next = next_flight[next];
            }
            cout << endl;
        }
    }

    return 0;
}
