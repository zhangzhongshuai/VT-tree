#ifndef SEGMENTTREE_H
#define SEGMENTTREE_H

#include <vector>
#include <algorithm>
#include "DetectorState.h"

using namespace std;

struct Segtree {
  int cover;
  int length;
  int max_length;
};

struct Edge {
  int x, idx, state;
};

class ScanLine {
public:
  int rectangleArea(vector<DetectorState>& rectangles) {
    int n = rectangles.size();
    for (const auto& rect : rectangles) {
      hbound.push_back(rect.bottom);
      hbound.push_back(rect.top);
    }
    sort(hbound.begin(), hbound.end());
    // cout << "hbound1:" << hbound.size() << endl;
    hbound.erase(unique(hbound.begin(), hbound.end()), hbound.end());
    int m = hbound.size();
    // cout << "hbound2:" << hbound.size() << endl;
    tree.resize(m * 4 + 1);
    init(1, 1, m - 1);

    vector<Edge> sweep;
    for (int i = 0; i < n; ++i) {
      sweep.push_back({ rectangles[i].left, i, 1 });  // 左边界
      sweep.push_back({ rectangles[i].right, i, -1 }); // 右边界
    }
    sort(sweep.begin(), sweep.end(), [](const Edge& a, const Edge& b) {
      return a.x < b.x;
      });
    long long ans = 0;
    // 每个扫描线事件
    for (int i = 0; i < sweep.size();) {
      int j = i;
      while (j + 1 < sweep.size() && sweep[i].x == sweep[j + 1].x) {
        ++j;
      }
      if (j + 1 == sweep.size()) {
        break;
      }
      for (int k = i; k <= j; ++k) {
        int idx = sweep[k].idx;
        int left = lower_bound(hbound.begin(), hbound.end(), rectangles[idx].top) - hbound.begin() + 1;
        int right = lower_bound(hbound.begin(), hbound.end(), rectangles[idx].bottom) - hbound.begin();
        update(1, 1, m - 1, left, right, sweep[k].state);
      }
      ans += static_cast<long long>(tree[1].length) * (sweep[j + 1].x - sweep[j].x);
      i = j + 1;
    }
    return ans % static_cast<int>(1e9 + 7);
  }

private:
  void init(int idx, int l, int r) {
    tree[idx].cover = tree[idx].length = 0;
    if (l == r) {
      tree[idx].max_length = hbound[l] - hbound[l - 1];
      return;
    }
    int mid = (l + r) / 2;
    init(idx * 2, l, mid);
    init(idx * 2 + 1, mid + 1, r);
    tree[idx].max_length = tree[idx * 2].max_length + tree[idx * 2 + 1].max_length;
  }

  void update(int idx, int l, int r, int ul, int ur, int diff) {
    if (l > ur || r < ul) {
      return;
    }
    if (ul <= l && r <= ur) {
      tree[idx].cover += diff;
      pushup(idx, l, r);
      return;
    }
    int mid = (l + r) / 2;
    update(idx * 2, l, mid, ul, ur, diff);
    update(idx * 2 + 1, mid + 1, r, ul, ur, diff);
    pushup(idx, l, r);
  }

  void pushup(int idx, int l, int r) {
    if (tree[idx].cover > 0) {
      tree[idx].length = tree[idx].max_length;
    }
    else if (l == r) {
      tree[idx].length = 0;
    }
    else {
      tree[idx].length = tree[idx * 2].length + tree[idx * 2 + 1].length;
    }
  }
  vector<Segtree> tree;
  vector<int> hbound;
};


#endif