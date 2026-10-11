#include <iostream>
#include <vector>
#include <algorithm>
#include <stack>
using namespace std;

const int MAXN = 100005;
const int MAXM = 300005;

vector<int> adj[MAXN];
int tin[MAXN], low[MAXN];
bool visited[MAXN];
int timer;
vector<vector<int>> biconnected_components;
vector<int> current_component;
stack<int> vertex_stack;
int component_id[MAXN];
int current_component_id;

void find_biconnected_components(int v, int parent = -1) {
    visited[v] = true;
    tin[v] = low[v] = timer++;
    vertex_stack.push(v);
    int children = 0;
    
    for (int to : adj[v]) {
        if (to == parent) continue;
        if (visited[to]) {
            low[v] = min(low[v], tin[to]);
        } else {
            children++;
            find_biconnected_components(to, v);
            low[v] = min(low[v], low[to]);
            
            if (low[to] >= tin[v]) {
                // Found a biconnected component
                current_component.clear();
                int u;
                do {
                    u = vertex_stack.top();
                    vertex_stack.pop();
                    current_component.push_back(u);
                } while (u != to);
                current_component.push_back(v);
                biconnected_components.push_back(current_component);
            }
        }
    }
    
    // Handle root vertex
    if (parent == -1 && children == 0) {
        current_component.clear();
        current_component.push_back(v);
        biconnected_components.push_back(current_component);
    }
}

bool can_be_invited[MAXN];

void check_component(const vector<int>& component) {
    if (component.size() < 3) return;
    
    // Build subgraph for this component
    vector<int> local_id(MAXN, -1);
    for (int i = 0; i < component.size(); i++) {
        local_id[component[i]] = i;
    }
    
    vector<vector<int>> sub_adj(component.size());
    for (int v : component) {
        for (int to : adj[v]) {
            if (local_id[to] != -1) {
                sub_adj[local_id[v]].push_back(local_id[to]);
            }
        }
    }
    
    // Check if this component is non-bipartite
    vector<int> color(component.size(), -1);
    bool is_non_bipartite = false;
    
    for (int i = 0; i < component.size() && !is_non_bipartite; i++) {
        if (color[i] == -1) {
            vector<int> q;
            q.push_back(i);
            color[i] = 0;
            
            while (!q.empty() && !is_non_bipartite) {
                int v = q.front();
                q.erase(q.begin());
                
                for (int to : sub_adj[v]) {
                    if (color[to] == -1) {
                        color[to] = color[v] ^ 1;
                        q.push_back(to);
                    } else if (color[to] == color[v]) {
                        is_non_bipartite = true;
                        break;
                    }
                }
            }
        }
    }
    
    // If non-bipartite, all vertices in this component can be invited
    if (is_non_bipartite) {
        for (int v : component) {
            can_be_invited[v] = true;
        }
    }
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    
    int n, m;
    cin >> n >> m;
    
    for (int i = 0; i < m; i++) {
        int x, y;
        cin >> x >> y;
        adj[x].push_back(y);
        adj[y].push_back(x);
    }
    
    // Initialize
    fill(can_be_invited, can_be_invited + n + 1, false);
    timer = 0;
    fill(visited, visited + n + 1, false);
    biconnected_components.clear();
    while (!vertex_stack.empty()) vertex_stack.pop();
    
    // Find all biconnected components
    for (int i = 1; i <= n; i++) {
        if (!visited[i]) {
            find_biconnected_components(i);
            // Clear remaining vertices in stack
            while (!vertex_stack.empty()) {
                vertex_stack.pop();
            }
        }
    }
    
    // Check each biconnected component
    for (const auto& component : biconnected_components) {
        check_component(component);
    }
    
    // Count vertices that cannot be invited
    int answer = 0;
    for (int i = 1; i <= n; i++) {
        if (!can_be_invited[i]) {
            answer++;
        }
    }
    
    cout << answer << endl;
    
    return 0;
}