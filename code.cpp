#include <bits/stdc++.h>


using namespace std;

class Map {
    int L; // Number of Locations
    list<pair<int, int>>* adj; // Adjacency list

public:
    Map(int L);
    void addDist(int u, int v, int Distance);
    void dijkstra(int src, int dest);
    void bfs(int src, int dest);
 

private:
    void displayPath(vector<int>& parent, int j);
    
    void printShortestPath(int src, int dest, vector<int>& dist, vector<int>& parent);
    
};

Map::Map(int L) {
    this->L = L;
    adj = new list<pair<int, int>>[L];
}

void Map::addDist(int u, int v, int Distance) {
    adj[u].push_back(make_pair(v, Distance));
    adj[v].push_back(make_pair(u, Distance));
}

void Map::displayPath(vector<int>& parent, int j) {
    if (parent[j] == -1)
        return;

    displayPath(parent, parent[j]);

    cout << j << " ";
}

void Map::printShortestPath(int src, int dest, vector<int>& dist, vector<int>& parent) {
    cout << "Shortest path length is: " << dist[dest] << "\nPath is: ";
    cout << src << " ";
    displayPath(parent, dest);
    cout << endl;
}

void Map::dijkstra(int src, int dest) {
    priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq;
    vector<int> dist(L, INT_MAX);
    vector<int> parent(L, -1);

    pq.push(make_pair(0, src));
    dist[src] = 0;

    while (!pq.empty()) {
        int u = pq.top().second;
        pq.pop();

        for (auto& neighbor : adj[u]) {
            int v = neighbor.first;
            int Distance = neighbor.second;

            if (dist[v] > dist[u] + Distance) {
                dist[v] = dist[u] + Distance;
                pq.push(make_pair(dist[v], v));
                parent[v] = u;
            }
        }
    }

   // cout << "Dijkstra's Algorithm:\n";
    printShortestPath(src, dest, dist, parent);
}

// void Map::bfs(int src, int dest) {
//     vector<int> dist(L, INT_MAX);
//     vector<int> parent(L, -1);
//     queue<int> q;

//     q.push(src);
//     dist[src] = 0;

//     while (!q.empty()) {
//         int u = q.front();
//         q.pop();

//         for (auto& neighbor : adj[u]) {
//             int v = neighbor.first;

//             if (dist[v] == INT_MAX) {
//                 dist[v] = dist[u] + 1;
//                 parent[v] = u;
//                 q.push(v);

//                 if (v == dest) {
//                     cout << "Breadth-First Search (BFS):\n";
//                     printShortestPath(src, dest, dist, parent);
//                     return;
//                 }
//             }
//         }
//     }
// }




int main() {
    int L = 9; // Number of Locations in the graph
    Map g(L);

    g.addDist(0, 1, 4);
    g.addDist(0, 7, 8);
    g.addDist(1, 2, 8);
    g.addDist(1, 7, 11);
    g.addDist(2, 3, 7);
    g.addDist(2, 8, 2);
    g.addDist(2, 5, 4);
    g.addDist(3, 4, 9);
    g.addDist(3, 5, 14);
    g.addDist(4, 5, 10);
    g.addDist(5, 6, 2);
    g.addDist(6, 7, 1);
    g.addDist(6, 8, 6);
    g.addDist(7, 8, 7);

    int src = 1;
    int dest = 7;

    g.dijkstra(src, dest);
    //g.bfs(src, dest);
    

    return 0;
}
