#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

const int MAXN = 100005;
const int MAXM = 300005;

vector<int> adj[MAXN];
int tin[MAXN], low[MAXN];
bool visited[MAXN];
int timer;
vector<pair<int, int>> bridges;

void find_bridges(int v, int parent = -1) {
    visited[v] = true;
    tin[v] = low[v] = timer++;
    
    for (int to : adj[v]) {
        if (to == parent) continue;
        if (visited[to]) {
            low[v] = min(low[v], tin[to]);
        } else {
            find_bridges(to, v);
            low[v] = min(low[v], low[to]);
            if (low[to] > tin[v]) {
                bridges.push_back({min(v, to), max(v, to)});
            }
        }
    }
}

int component_id[MAXN];
int current_component;

void dfs_component(int v) {
    component_id[v] = current_component;
    for (int to : adj[v]) {
        if (component_id[to] == -1) {
            // Check if this edge is a bridge
            bool is_bridge = false;
            int a = min(v, to), b = max(v, to);
            for (auto &bridge : bridges) {
                if (bridge.first == a && bridge.second == b) {
                    is_bridge = true;
                    break;
                }
            }
            if (!is_bridge) {
                dfs_component(to);
            }
        }
    }
}

int color[MAXN];
vector<int> component_vertices;

bool is_bipartite_component(int start_v) {
    component_vertices.clear();
    
    // BFS to check bipartite and collect vertices
    vector<int> q;
    q.push_back(start_v);
    color[start_v] = 0;
    component_vertices.push_back(start_v);
    
    while (!q.empty()) {
        int v = q.front();
        q.erase(q.begin());
        
        for (int to : adj[v]) {
            // Skip if different component or bridge
            if (component_id[to] != component_id[start_v]) continue;
            
            int a = min(v, to), b = max(v, to);
            bool is_bridge = false;
            for (auto &bridge : bridges) {
                if (bridge.first == a && bridge.second == b) {
                    is_bridge = true;
                    break;
                }
            }
            if (is_bridge) continue;
            
            if (color[to] == -1) {
                color[to] = color[v] ^ 1;
                component_vertices.push_back(to);
                q.push_back(to);
            } else if (color[to] == color[v]) {
                // Found odd cycle, but we still need to collect all vertices
                // Continue BFS to get all vertices in this component
                continue;
            }
        }
    }
    
    // Check if component is bipartite
    for (int v : component_vertices) {
        for (int to : adj[v]) {
            if (component_id[to] != component_id[start_v]) continue;
            
            int a = min(v, to), b = max(v, to);
            bool is_bridge = false;
            for (auto &bridge : bridges) {
                if (bridge.first == a && bridge.second == b) {
                    is_bridge = true;
                    break;
                }
            }
            if (is_bridge) continue;
            
            if (color[v] == color[to]) {
                return false; // Not bipartite
            }
        }
    }
    
    return true; // Bipartite
}

void solve() {
    int n, m;
    cin >> n >> m;
    
    // Clear data structures
    for (int i = 1; i <= n; i++) {
        adj[i].clear();
    }
    bridges.clear();
    timer = 0;
    fill(visited, visited + n + 1, false);
    fill(component_id, component_id + n + 1, -1);
    fill(color, color + n + 1, -1);
    current_component = 0;
    
    for (int i = 0; i < m; i++) {
        int x, y;
        cin >> x >> y;
        adj[x].push_back(y);
        adj[y].push_back(x);
    }
    
    cout << "Input: n=" << n << ", m=" << m << endl;
    
    // Find all bridges
    for (int i = 1; i <= n; i++) {
        if (!visited[i]) {
            find_bridges(i);
        }
    }
    
    cout << "Bridges found: " << bridges.size() << endl;
    for (auto &bridge : bridges) {
        cout << "  " << bridge.first << " " << bridge.second << endl;
    }
    
    // Find bridge-connected components
    for (int i = 1; i <= n; i++) {
        if (component_id[i] == -1) {
            current_component++;
            dfs_component(i);
        }
    }
    
    cout << "Components: " << current_component << endl;
    for (int comp = 1; comp <= current_component; comp++) {
        cout << "  Component " << comp << ": ";
        for (int i = 1; i <= n; i++) {
            if (component_id[i] == comp) {
                cout << i << " ";
            }
        }
        cout << endl;
    }
    
    // For each component, check if it's bipartite
    int answer = 0;
    
    for (int comp = 1; comp <= current_component; comp++) {
        // Find a vertex in this component
        int start_v = -1;
        for (int i = 1; i <= n; i++) {
            if (component_id[i] == comp) {
                start_v = i;
                break;
            }
        }
        
        if (start_v == -1) continue;
        
        bool bipartite = is_bipartite_component(start_v);
        cout << "Component " << comp << " is " << (bipartite ? "bipartite" : "non-bipartite") << endl;
        
        if (bipartite) {
            // This component is bipartite, so all vertices in it cannot be invited
            for (int i = 1; i <= n; i++) {
                if (component_id[i] == comp) {
                    answer++;
                }
            }
        }
    }
    
    cout << "Answer: " << answer << endl;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    
    solve();
    
    return 0;
}