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
        p = paths;
        q = time;
        d.resize(n);
        pt.resize(n, {});
        e.resize(paths.size(), {});
        for (int i = 0; i < (int)paths.size(); i++) {
            for (int j = 0; j < (int) paths[i].size(); j++)
                pt[paths[i][j]].emplace_back(i, j);
            e[i].push_back(0);
            if (paths[i].size() != lengths[i].size() + 1)
                throw new runtime_error(to_string(i + 1) + "-th path's number of stations differs from number of edges");
            for (int j = 0; j < (int) lengths[i].size(); j++) {
                e[i].push_back(e[i][j] + lengths[i][j]);
            }
        }
    }

    pair<T, int> get(int start, int finish, T start_time) {
        fill(d.begin(), d.end(), make_pair(INF, 0));
        set<pair<T, int>> g;
        d[start] = {start_time, 0};
        g.insert({start_time, start});
        while(!g.empty()) {
            auto it = g.begin();
            int v = it->second;
            g.erase(it);
            if (v == finish) {
                break;
            }
            auto update = [&](int i, pair<T, int> nw) {
                if (d[i] > nw) {
                    g.erase({d[i].first, i});
                    d[i] = nw;
                    g.insert({d[i].first, i});
                }
            };
            for (auto [id, i] : pt[v]) {
                T w = wait(d[v].first, id, i, 0);
                for (int j = i + 1; j < (int)p[id].size(); j++)
                    update(p[id][j], make_pair(d[v].first + w + e[id][j] - e[id][i], d[v].second + 1));
                w = wait(d[v].first, id, i, 1);
                for (int j = i - 1; j >= 0; j--)
                    update(p[id][j], make_pair(d[v].first + w - e[id][j] + e[id][i], d[v].second + 1));
            }
        }
        if (d[finish].first == INF)
            d[finish].first = -1;
        return d[finish];
    }

    pair<T, int> get_lazy(int start, int finish, T start_time) {
        auto smallest = get(start, finish, start_time);
        if (smallest.first == -1)
            return {-1, 0};
        int bound = smallest.second;
        ds.resize(bound + 1, vector<T>(n, INF));
        ds[0][start] = start_time;
        for (int it = 0; it < bound; it++) {
            for (int i = 0; i < (int)p.size(); i++) {
                T val = INF;
                for (int j = 0; j < (int)p[i].size(); j++) {
                    if (val != INF)
                        ds[it + 1][p[i][j]] = min(ds[it + 1][p[i][j]], val + e[i][j]);
                    T &o = ds[it][p[i][j]];
                    if (o != INF)
                        val = min(val, o + wait(o, i, j, 0) - e[i][j]);
                }
                val = INF;
                for (int j = (int)p[i].size() - 1; j >= 0; j--) {
                    if (val != INF)
                        ds[it + 1][p[i][j]] = min(ds[it + 1][p[i][j]], val - e[i][j]);
                    T &o = ds[it][p[i][j]];
                    if (o != INF)
                        val = min(val, o + wait(o, i, j, 1) + e[i][j]);
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
        T rem = (w % q[id]);
        T cur_rem = (rev ? (e[id].back() - e[id][i]) : e[id][i] % q[id]);
        if (cur_rem >= rem)
            cur_rem = cur_rem - rem;
        else
            cur_rem = q[id] - rem + cur_rem;
        return cur_rem;
    }

    int n;
    vector<vector<int>> p;
    vector<vector<T>> e;
    vector<vector<pair<int, int>>> pt;
    vector<T> q;
    vector<pair<T, int>> d;
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
