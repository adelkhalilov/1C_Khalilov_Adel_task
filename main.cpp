#include <bits/stdc++.h>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wc++17-extensions"

using namespace std;

template<typename T = int>
struct Subway {
    void init(int n_, vector<vector<int>> const &paths, vector<vector<T>> const &lengths, vector<T> const &time) {
        if (paths.size() != lengths.size() || paths.size() != time.size())
            throw new runtime_error("paths, lengths, time sizes must be equal");
        n = n_;
        path = paths;
        times = time;
        dist.resize(n);
        path_list.resize(n, {});
        edges.resize(paths.size(), {});
        for (int i = 0; i < (int)paths.size(); i++) {
            for (int j = 0; j < (int) paths[i].size(); j++)
                path_list[paths[i][j]].emplace_back(i, j);
            edges[i].push_back(0);
            if (paths[i].size() != lengths[i].size() + 1)
                throw new runtime_error(to_string(i + 1) + "-th path's number of stations differs from number of edges");
            for (int j = 0; j < (int) lengths[i].size(); j++) {
                edges[i].push_back(edges[i][j] + lengths[i][j]);
            }
        }
    }

    pair<T, int> get(int start, int finish, T start_time) {
        fill(dist.begin(), dist.end(), make_pair(INF, 0));
        set<pair<T, int>> dijkstra_set;
        dist[start] = {start_time, 0};
        dijkstra_set.insert({start_time, start});
        while (!dijkstra_set.empty()) {
            auto it = dijkstra_set.begin();
            int v = it->second;
            dijkstra_set.erase(it);
            if (v == finish) {
                break;
            }
            auto update = [&](int i, pair<T, int> nw) {
                if (dist[i] > nw) {
                    dijkstra_set.erase({dist[i].first, i});
                    dist[i] = nw;
                    dijkstra_set.insert({dist[i].first, i});
                }
            };
            for (auto [id, i] : path_list[v]) {
                T w = wait(dist[v].first, id, i, 0);
                for (int j = i + 1; j < (int)path[id].size(); j++)
                    update(path[id][j], make_pair(dist[v].first + w + edges[id][j] - edges[id][i], dist[v].second + 1));
                w = wait(dist[v].first, id, i, 1);
                for (int j = i - 1; j >= 0; j--)
                    update(path[id][j], make_pair(dist[v].first + w - edges[id][j] + edges[id][i], dist[v].second + 1));
            }
        }
        if (dist[finish].first == INF)
            dist[finish].first = -1;
        return dist[finish];
    }

    pair<T, int> get_lazy(int start, int finish, T start_time) {
        auto smallest = get(start, finish, start_time);
        if (smallest.first == -1)
            return {-1, 0};
        int bound = smallest.second;
        ds.resize(bound + 1, vector<T>(n, INF));
        ds[0][start] = start_time;
        for (int it = 0; it < bound; it++) {
            for (int i = 0; i < (int)path.size(); i++) {
                T val = INF;
                for (int j = 0; j < (int)path[i].size(); j++) {
                    if (val != INF)
                        ds[it + 1][path[i][j]] = min(ds[it + 1][path[i][j]], val + edges[i][j]);
                    T &o = ds[it][path[i][j]];
                    if (o != INF)
                        val = min(val, o + wait(o, i, j, 0) - edges[i][j]);
                }
                val = INF;
                for (int j = (int)path[i].size() - 1; j >= 0; j--) {
                    if (val != INF)
                        ds[it + 1][path[i][j]] = min(ds[it + 1][path[i][j]], val - edges[i][j]);
                    T &o = ds[it][path[i][j]];
                    if (o != INF)
                        val = min(val, o + wait(o, i, j, 1) + edges[i][j]);
                }
            }
        }
        for (int i = 0; i <= bound; i++)
            if (ds[i][finish] != INF && (ds[i][finish] - start_time) * 2 <= 3 * (smallest.first - start_time))
                return {ds[i][finish], i};
        return {-1, 0};
    }

private:

    T wait(T w, int id, int i, bool rev) {
        T rem = (w % times[id]);
        T cur_rem = (rev ? (edges[id].back() - edges[id][i]) : edges[id][i] % times[id]);
        if (cur_rem >= rem)
            cur_rem = cur_rem - rem;
        else
            cur_rem = times[id] - rem + cur_rem;
        return cur_rem;
    }

    int n;
    vector<vector<int>> path;
    vector<vector<T>> edges;
    vector<vector<pair<int, int>>> path_list;
    vector<T> times;
    vector<pair<T, int>> dist;
    vector<vector<T>> ds;
    const T INF = numeric_limits<T>::max();
};

int main() {
#ifdef ONPC
    freopen("../a.in", "r", stdin);
    freopen("../a.out", "w", stdout);
#endif
    ios::sync_with_stdio(0);
    cin.tie(0);
    int n, m;
    cin >> n >> m;
    vector<vector<int>> p(m), e(m);
    vector<int> t(m);
    for (int i = 0; i < m; i++) {
        int len;
        cin >> len;
        assert(len > 0);
        p[i].resize(len);
        e[i].resize(len - 1);
        for (int &j : p[i]) {
            cin >> j;
            j--;
        }
        for (int &j : e[i])
            cin >> j;
        cin >> t[i];
    }
    Subway<int> S;
    S.init(n, p, e, t);
    auto x = S.get(0, n - 1, 0), y = S.get_lazy(0, n - 1, 0);
    cout << x.first << ' ' << x.second << '\n' << y.first << ' ' << y.second << '\n';
}

#pragma clang diagnostic pop
