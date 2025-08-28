#include <bits/stdc++.h>
using namespace std;

struct PowerLine {
    string cityA;
    string cityB;
    PowerLine() {}
    PowerLine(const string& a, const string& b) : cityA(a), cityB(b) {}
};

// Students can define new classes here
struct vertex {
    int city;
    int level;
    vertex* parent;
    int highPoint;
    bool isLeaf;
    vertex(int city_) : city(city_), level(1), parent(nullptr), highPoint(1), isLeaf(false) {}
};

class PowerGrid {
public:
    int numCities;
    int numLines;
    vector<string> cityNames;
    vector<PowerLine> powerLines;

private:
    vector<vector<int>> adjList;
    vector<vertex*> vertices;

    deque<vertex*> leaves;
    deque<vertex*> leaves_copy;
    deque<vertex*> leaves_copy2;
    deque<vertex*> leaves_copy3;

    unordered_map<string,int> cities;

    vector<vector<vertex*>> parent;

    vector<int> numBridge;

    bool highPointSet;

    // Students can define private variables here

public:
    PowerGrid(const string& filename) {
        ifstream in(filename);
        if (!in) {
            throw runtime_error("Cannot open input file: " + filename);
        }
        in >> numCities;
        in >> numLines;

        cityNames.resize(numCities);
        for (int i = 0; i < numCities; i++) {
            in >> cityNames[i];
        }

        powerLines.resize(numLines);
        for (int i = 0; i < numLines; i++) {
            string a, b;
            in >> a >> b;
            powerLines[i] = PowerLine(a, b);
        }

        // TO be completed by students

        adjList.assign(numCities, {});
        cities.reserve(numCities * 2);

        for (int i = 0; i < numCities; i++) {
            cities[cityNames[i]] = i;
        }

        for (int i = 0; i < numLines; i++) {
            int j = cities[powerLines[i].cityA];
            int k = cities[powerLines[i].cityB];
            adjList[j].push_back(k);
            adjList[k].push_back(j);
        }

        vertices.assign(numCities, nullptr);

        parent.clear(); // will be sized later
        numBridge.assign(numCities, 0);

        highPointSet = false;
    }

private:
    void DFShelper(vertex* city, vector<int>& visited) {
        visited[city->city] = 1;
        bool isLeafHelper = true;

        for (int nxt : adjList[city->city]) {
            if (visited[nxt] == 0) {
                isLeafHelper = false;
                vertex* v = new vertex(nxt);
                vertices[v->city] = v;
                v->parent = city;
                v->highPoint = 1000000000; // large
                v->level = city->level + 1;
                DFShelper(v, visited);
            }
        }
        if (isLeafHelper) {
            city->isLeaf = true;
            leaves.push_back(city);
            leaves_copy.push_back(city);
            leaves_copy2.push_back(city);
            leaves_copy3.push_back(city);
        }
    }

    void DFS() {
        vector<int> visited(numCities, 0);
        vertex* city = new vertex(0);
        vertices[0] = city;
        DFShelper(city, visited);
    }

    void highPoint() {
        highPointSet = true;
        DFS();
        while (!leaves.empty()) {
            vertex* v = leaves.front(); leaves.pop_front();
            for (int nei : adjList[v->city]) {
                vertex* neiV = vertices[nei];
                if (neiV->parent != nullptr && neiV->parent->city == v->city) {
                    v->highPoint = min(v->highPoint, neiV->highPoint);
                } else if (v->parent != nullptr && v->parent->city != nei) {
                    v->highPoint = min(v->highPoint, neiV->level);
                } else if (v->parent == nullptr) {
                    v->highPoint = 1;
                }
            }
            if (v->parent != nullptr) {
                leaves.push_back(v->parent);
            }
        }
    }

    int LogBase2(int x) {
        int ans = 0;
        int mul = 1;
        while (x > mul) {
            mul = mul * 2;
            ans = ans + 1;
        }
        return ans;
    }

    int LogBase2_Floor(int x) {
        int ans = 0;
        int mul = 1;
        while (x > mul) {
            mul = mul * 2;
            ans = ans + 1;
        }
        if (x == mul) {
            ans = ans + 1;
        }
        return ans - 1;
    }

public:
    vector<PowerLine> criticalLines() {
        vector<int> alreadyVisited(numCities, 0);

        // DFS();  // Java commented out; it calls highPoint() instead
        highPoint();

        vector<PowerLine> ans;
        while (!leaves_copy.empty()) {
            vertex* leaf = leaves_copy.front(); leaves_copy.pop_front();
            if (leaf->parent != nullptr && alreadyVisited[leaf->city] == 0) {
                leaves_copy.push_back(leaf->parent);
                alreadyVisited[leaf->city] = 1;
                if (leaf->highPoint >= leaf->level) {
                    PowerLine helper(cityNames[leaf->city], cityNames[leaf->parent->city]);
                    ans.push_back(helper);
                }
            }
        }
        return ans;
    }

private:
    void numBridgeEdgesHelper(vertex* v) {
        if (v->parent != nullptr) {
            if (v->highPoint >= v->level) {
                numBridge[v->city] = 1 + numBridge[v->parent->city];
            } else {
                numBridge[v->city] = numBridge[v->parent->city];
            }
        } else {
            numBridge[v->city] = 0;
        }
        for (int l : adjList[v->city]) {
            vertex* child = vertices[l];
            if (child->parent != nullptr) {
                if (child->parent->city == v->city) {
                    numBridgeEdgesHelper(child);
                }
            }
        }
    }

    void numBridgeEdges() {
        if (!highPointSet) {
            highPoint();
        }
        numBridgeEdgesHelper(vertices[0]);
    }

    vector<vector<vertex*>> parentAtI() {
        if (!highPointSet) {
            highPoint();
        }
        int s = 0;
        for (int i = 0; i < numCities; i++) {
            s = max(vertices[i]->level, s);
        }
        int size = LogBase2(s - 1);
        vector<vector<vertex*>> parents(numCities, vector<vertex*>(size + 1, nullptr));

        deque<vertex*> p;
        p.push_back(vertices[0]);

        while (!p.empty()) {
            vertex* v = p.front(); p.pop_front();
            int level = v->level;
            int a = LogBase2(level);

            for (int nei : adjList[v->city]) {
                vertex* child = vertices[nei];
                if (child->parent != nullptr) {
                    if (child->parent->city == v->city) {
                        p.push_back(child);
                        parents[child->city][0] = v;
                        int j = 1;
                        while (j <= a) {
                            vertex* mid = parents[child->city][j - 1];
                            if (mid == nullptr) break;
                            parents[child->city][j] = parents[mid->city][j - 1];
                            j++;
                        }
                    }
                }
            }
        }
        return parents;
    }

    vertex* LCA(int a, int b) {
        if (!highPointSet) {
            highPoint();
        }
        vector<vector<vertex*>>& par = this->parent;
        vertex* A = vertices[a];
        vertex* B = vertices[b];
        vertex* ans = nullptr;

        if (a == b) {
            ans = vertices[a];
            return ans;
        } else if (a == 0 || b == 0) {
            ans = vertices[0];
            return ans;
        } else {
            vertex* helper = nullptr;

            if (B->level > A->level) {
                A = vertices[b];
                B = vertices[a];
            }
            if (A->level > B->level) {
                int diff = A->level - B->level;
                int k = LogBase2_Floor(diff);
                int a1 = A->city;

                while (diff != 0) {
                    if (k < 0) break;
                    vertex* up = par[a1][k];
                    if (up == nullptr) {
                        // if cannot jump that far, reduce k
                        k--;
                        continue;
                    }
                    helper = vertices[up->city];
                    a1 = helper->city;
                    diff = helper->level - B->level;
                    k = LogBase2_Floor(diff);
                }
            }

            if (helper == nullptr) {
                helper = A;
            }
            if (helper->city == B->city) {
                ans = B;
                return ans;
            }

            int j = 0;
            if (B->parent == nullptr || helper->parent == nullptr) {
                ans = vertices[0];
                return ans;
            } else {
                int helper1 = B->city;
                int helper2 = helper->city;
                while (true) {
                    while (true) {
                        vertex* p1 = (j < (int)par[helper1].size() ? par[helper1][j] : nullptr);
                        vertex* p2 = (j < (int)par[helper2].size() ? par[helper2][j] : nullptr);
                        if (p1 != nullptr && p2 != nullptr && p1->city != p2->city) {
                            j++;
                        } else {
                            break;
                        }
                    }
                    if (j == 0) {
                        ans = par[helper1][j];
                        if (ans == nullptr) ans = vertices[0];
                        return ans;
                    } else {
                        vertex* h1 = par[helper1][j - 1];
                        vertex* h2 = par[helper2][j - 1];
                        if (h1 == nullptr || h2 == nullptr) {
                            // if either is null, try a smaller jump
                            j = max(0, j - 1);
                            continue;
                        }
                        helper1 = h1->city;
                        helper2 = h2->city;
                        j = 0;
                    }
                }
            }
        }
    }

public:
    void preprocessImportantLines() {
        this->parent = parentAtI();
        numBridgeEdges();
        return;
    }

    int numImportantLines(const string& cityA, const string& cityB) {
        // In the Java code, preprocessImportantLines() is (re)called here.
        preprocessImportantLines();
        int n1 = this->cities[cityA];
        int n2 = this->cities[cityB];
        vertex* lca = LCA(n1, n2);
        int ans = this->numBridge[n1] + this->numBridge[n2] - 2 * this->numBridge[lca->city];
        return ans;
    }
};

int main() {
    try {
        PowerGrid pg("test2.txt");
        vector<PowerLine> cl = pg.criticalLines();
        cout << cl.size() << "\n";
        // Example usage:
        // cout << pg.numImportantLines("Delhi","Chennai") << "\n";
    } catch (const exception& e) {
        cerr << e.what() << "\n";
    }
    return 0;
}
