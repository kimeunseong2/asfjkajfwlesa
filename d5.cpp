#include <iostream>
#include <queue>
#include <tuple>
#include <vector>
#include <algorithm>

#define SMAX 2000
#define PMAX 1000001
#define endl "\n"

using namespace std;

typedef long long ll;

int N, M, Q;

ll soldier[PMAX * 2];
int territory[PMAX * 2];
int parent[PMAX * 2];

pair<int, int> visited[SMAX][SMAX];
queue<pair<int, int> > q;

int dx[] = { 0, 0, 1, -1 };
int dy[] = { 1, -1, 0, 0 };

int day = 0;
int dayController[PMAX * 2];

int Find(int x, int r) {
		if (parent[x] == x) return x;
		return parent[x] = Find(parent[x], ++r);
}

void Attack_And_Union(int x1, int y1, int x2, int y2) {
		int a, b;
		if (visited[x1][y1].second >= 0) a = Find(visited[x1][y1].first, 0);
		else a = -visited[x1][y1].second;
		if (visited[x2][y2].second >= 0) b = Find(visited[x2][y2].first, 0);
		else b = -visited[x2][y2].second;

		if (soldier[a] == soldier[b]) return;

		if (soldier[a] > soldier[b]) {
				soldier[a] -= soldier[b];
				soldier[b] = 0;

				bool is_B_Capital = territory[b] == 1;

				territory[a] += territory[b] - 1;
				territory[b] = 1;

				if (Find(b, 0) == b) {
						if (is_B_Capital) return;
						if (Find(a, 0) != a) {
								if (Find(a, 0) == b) { parent[a] = a, parent[b] = a; }
								else {
										// 패배한 국가가 새로 다른 나라를 소유하는 경우
										// 패배한 국가를 소유한 국가의 관계 무시
										// 새로운 국가 ID를 더미 값(N + PMAX)로 업데이트
									
										if (a + PMAX < 2 * PMAX) {
												a += PMAX;
												visited[x1][y1].second = -a;
												territory[a] = territory[a - PMAX];
												soldier[a] = soldier[a - PMAX];
												dayController[a] = dayController[a - PMAX];

												parent[a] = a;
												parent[b] = a;
										}
								}
						}
						else parent[b] = a;
				}
		}
		else {

				soldier[b] -= soldier[a];
				soldier[a] = 0;

				bool is_A_Capital = territory[a] == 1;

				territory[b] += territory[a] - 1;
				territory[a] = 1;

				if (Find(a, 0) == a) {
						if (is_A_Capital) return;
						if (Find(b, 0) != b) {
								if (Find(b, 0) == a) { parent[b] = b, parent[a] = b; }
								else if (b + PMAX<= 2 * PMAX) {
										b += PMAX;

										visited[x2][y2].second = -b;
										territory[b] = territory[b - PMAX];
										soldier[b] = soldier[b - PMAX];
										dayController[b] = dayController[b - PMAX];

										parent[b] = b;
										parent[a] = b;
								}
						}
						else parent[a] = b;
				}

		}

}


void BFS(ll t, int x1, int y1, int x2, int y2) {
		static bool flag = false;
	
		// 하루 = 큐의 사이즈만큼만 반복
	
		while (day < t && !q.empty()) { 
				for (int i = 1; i <= M; i++) soldier[i] += territory[i];

				int size = q.size();
				while (size--)
				{
						int x = q.front().first, y = q.front().second;
						q.pop();

						for (int i = 0; i < 4; i++) {
								int nx = x + dx[i], ny = y + dy[i];
								if (nx < 0 || ny < 0 || nx >= N || ny >= N) continue;

								if (visited[nx][ny].first) {
										if (visited[nx][ny].second < 0) continue;

										if (visited[nx][ny].second != day) continue;

										if (soldier[Find(visited[nx][ny].first, 0)] >= soldier[Find(visited[x][y].first, 0)]) continue;

										territory[Find(visited[nx][ny].first, 0)]--;
								}

								visited[nx][ny].first = visited[x][y].first, visited[nx][ny].second = day;
								territory[Find(visited[x][y].first, 0)]++;
								q.push({ nx, ny });

						}
				}
				day++;
		}

		if (!q.empty()) return;

		if (!flag) {
				for (int i = 1; i <= M; i++) dayController[i] = day;
				flag = true;
		}

		if (x2 != -1 && y2 != -1) {
				int a, b;
				if (visited[x1][y1].second >= 0) a = Find(visited[x1][y1].first, 0);
				else a = -visited[x1][y1].second;
				if (visited[x2][y2].second >= 0) b = Find(visited[x2][y2].first, 0);
				else b = -visited[x2][y2].second;

				if (a != b)
				{
						soldier[a] += territory[a] * (t - dayController[a]);
						soldier[b] += territory[b] * (t - dayController[b]);
						dayController[a] = t, dayController[b] = t;
				}
				else {
						soldier[a] += territory[a] * (t - dayController[a]);
						dayController[a] = t;
				}
		}
		else {
				int a;
				if (visited[x1][y1].second >= 0) a = Find(visited[x1][y1].first, 0);
				else a = -visited[x1][y1].second;

				soldier[a] += territory[a] * (t - dayController[a]);
				dayController[a] = t;
		}

}

void Set_Info(vector<tuple<int, int, int, int> > order) {
		sort(order.begin(), order.end());
		for (int i = 1; i <= M; i++) {
				int x, y, k;
				tie(ignore, x, y, k) = order[i - 1];

				q.push({ x, y });
				visited[x][y].first = i, visited[x][y].second = -i;

				territory[i] = 1;
				soldier[i] = k;

				parent[i] = i;
		}
}

void Input() {
		vector<tuple<int, int, int, int> > order;

		cin >> N >> M >> Q;

		for (int i = 1; i <= M; i++) {
				int x, y, k; cin >> x >> y >> k;
				int num = x * N + y;
				order.push_back({ num, x - 1, y - 1, k });
		}

		Set_Info(order);

}

void Query() {
		for (int i = 0; i < Q; i++) {
				int o; cin >> o;
				if (o == 1) {
						int t, x1, y1, x2, y2;
						cin >> t >> x1 >> y1 >> x2 >> y2;
						x1--, y1--, x2--, y2--;

						BFS(t, x1, y1, x2, y2);
						Attack_And_Union(x1, y1, x2, y2);
				}
				else if (o == 2) {
						int t, xa, ya;
						cin >> t >> xa >> ya;
						xa--, ya--;

						BFS(t, xa, ya, -1, -1);

						if (visited[xa][ya].second >= 0) cout << soldier[Find(visited[xa][ya].first, 0)] << endl;
						else cout << soldier[-visited[xa][ya].second] << endl;
				}
		}
}

int main() {
		ios_base::sync_with_stdio(false); cin.tie(0); cout.tie(0);

		Input();
		Query();
		return 0;
}
