//
// Created by zcc on 2021/6/23.
//

#ifndef QUADTRANS_QUADTREE_H
#define QUADTRANS_QUADTREE_H

#include <memory>
#include <vector>
#include <cassert>
#include <algorithm>
#include "Box.h"
#include "Point.h"
#include <cmath>
#include <algorithm>
#include "Point.h"
// #include "B+ tree.h"
#include <vector>
#include <queue>
#include <set>
#include <unordered_map>
#include <map>
#include<limits>
#include "DetectorState.h"
#include "InvertedIndex.h"
#include "SegmentTree.h"
// #include "CSEtree.h"
#include <unordered_set>
#include "our_method.h"

const double eps = 1e-10;

// 阈值
double Alpha = 0.7;
  ///四叉树的最大深度
auto MaxDepth = std::size_t(6);

class Leaf{
public:
    double left;
    double top;
    double width; // Must be positive
    double height; // Must be positive
    int track_id;
    int frameIndex;
    Leaf(double Left, double Top, double Width, double Height,int Track_id,int FrameIndex) :
            left(Left), top(Top), width(Width), height(Height) , track_id(Track_id) ,frameIndex(FrameIndex){

    }
    friend class BPTree;
};
class BNode {
    /*
		Generally size of the this node should be equal to the block size. Which will limit the number of disk access and increase the accesssing time.
		Intermediate nodes only hold the Tree pointers which is of considerably small size(so they can hold more Tree pointers) and only Leaf nodes hold
		the data pointer directly to the disc.

		IMPORTANT := All the data has to be present in the leaf node
	*/
public:
    bool isLeaf;
    vector<int> keys;
    //Node* ptr2parent; //Pointer to go to parent node CANNOT USE check https://stackoverflow.com/questions/57831014/why-we-are-not-saving-the-parent-pointer-in-b-tree-for-easy-upward-traversal-in
    BNode* ptr2next;              //Pointer to connect next node for leaf nodes
    union ptr {                  //to make memory efficient Node
        vector<BNode*> ptr2Tree;  //Array of pointers to Children sub-trees for intermediate Nodes
        vector<Leaf> dataPtr;   // Data-Pointer for the leaf node

        ptr();   // To remove the error !?
        ~ptr();  // To remove the error !?
    } ptr2TreeOrData;

    friend class BPTree;  // to access private members of the Node and hold the encapsulation concept
public:
    BNode();
};

struct Nodes
{
  Box box;
};

// edit by wmz 
// 轨迹段结构，后续需要替换成新的树
struct Tracks
{
  int framestart;
  int frameend;
  int trackid;
//  Box location;
  std::vector<DetectorState> value;
  Tracks() = default;
  Tracks(int framestart, int frameend, int trackid, const std::vector<DetectorState> value) :framestart(framestart), frameend(frameend), trackid(trackid), value(value)
  {
  }
};



template<typename T, typename GetBox, typename Equal = std::equal_to<T>>
class Quadtree {

public:
  Quadtree(const Box& box, const GetBox& getBox = GetBox(),
    const Equal& equal = Equal()) :
    mBox(box), mRoot(std::make_unique<Node>()), mGetBox(getBox), mEqual(equal) {}

  void makeFullTree()
  {
    makeFullTree(mRoot.get(), mBox, 0, MaxDepth);
  }

  void add(const T& value)
  {
    add(mRoot.get(), 0, mBox, value);
  }

  // edit by wmz 
// 主函数：将轨迹段插入四叉树
  void buildTree(std::map<int, std::vector<DetectorState>>& tracks) {
    // 按照trackid插入轨迹段，每层循环插入一个trackid的轨迹
    int count = 0;
    for (auto item : tracks)
    {
      std::vector<DetectorState>& track = item.second;
      int trackid = item.first;
      int cnt = 0;
      insert(mRoot.get(), mBox, track, cnt);
      count += cnt;
      //std::cout << "trackid:" << trackid << " size:" << cnt << std::endl;
    }
    std::cout << "整个四叉树共计存了:" << count << "条轨迹" << std::endl;
  }

  void remove(const T& value)
  {
    remove(mRoot.get(), nullptr, mBox, value);
  }

  std::vector<T> query(const Box& box) const
  {
    auto values = std::vector<T>();
    query(mRoot.get(), mBox, box, values);
    return values;
  }

  std::vector<T> query(vector<Point> hull) const {
    auto values = std::vector<T>();
    query(mRoot.get(), mBox, hull, values);
    return values;
  }


  // 面积数据类型
  typedef int CATYPE;

  // 节点的结构
  struct Node
  {
    std::array<std::unique_ptr<Node>, 4> children;

//      std::vector<Tracks> values;
        // cse::CSEtree<Tracks> values;
        our_method<Tracks> values;
        // Node():values([](Tracks a){return a.framestart;}, [](Tracks a){return a.frameend;}){
        // }
    //    本节点对应的四叉树编码
    std::string code;
    //   std::vector<T> values_id;//　按照ｔｒａｃｋ　ＩＤ排序
    //    BPTree* bPTree = new BPTree(1000, 1000);
  };
  

  // 平面坐标点
  struct PPoint
  {
    int i; int j;
    PPoint() = default;
    PPoint(int i, int j) :i(i), j(j) {}
    bool operator == (const PPoint& a) const {
      return i == a.i && j == a.j;
    }
    PPoint operator + (const int a) const {
      return PPoint(i + a, j + a);
    }
  };

  // 实际坐标 (x, y) 至平面坐标 (i, j) 的转换
  PPoint getIdxByPoint(Point point) {
    CATYPE width = mBox.width / QPlaneN, height = mBox.height / QPlaneN;
    int j = (point.x - mBox.left) / width;
    int i = (point.y - mBox.top) / height;
    return PPoint {i, j};
  }

  // 平面坐标 (i, j) 至实际坐标 Box 的转换
  Box getBoxByIdx(PPoint idx) {
    CATYPE width = mBox.width / QPlaneN, height = mBox.height / QPlaneN;
    return Box { mBox.left + idx.j * width, mBox.top + idx.i * height, static_cast<double>(width), static_cast<double>(height) };
  }

  // 平面四叉树叶节点 (j, i) 坐标至 CArea 的映射
  std::vector<std::vector<CATYPE>> mIdxCArea;

  // 初始化面积计算（仅在四叉树结构或意义变化时调用）
  void initArea() {
    mIdxCArea = std::vector<std::vector<CATYPE>>(QPlaneN, std::vector<CATYPE>(QPlaneN, 0));
  }

  // 清除 CArea （在每次重计算前调用）
  void clearCArea() {
    for (int j = 0; j < QPlaneN; ++j) {
      for (int i = 0; i < QPlaneN; ++i) {
        mIdxCArea[j][i] = 0;
      }
    }
  }

  // 扫描线事件
  enum class EventType: int { Start, End };
  struct Event {
    int id; // 标识
    CATYPE x; // x 坐标
    CATYPE y1, y2; // ymin; ymax
    EventType type;
    Event() = default;
    bool operator < (const Event& a) const {
      return x < a.x || (x == a.x && type > a.type);
    }
  };
  std::vector<Event> events;

  // 竖直扫描段
  struct Segment {
    CATYPE s, e; // start; end
    Segment() = default;
    bool operator < (const Segment& a) const {
      return s < a.s || (s == a.s && e < a.e);
    }
  };

  // 计算 CArea 面积贡献
  void calcCArea(const std::vector<DetectorState>& track) {
    if (track.empty()) return;
    CATYPE width = mBox.width / QPlaneN, height = mBox.height / QPlaneN;

    // 按列差分贡献（对 i 差分）
    auto addsum = [&] (int j, int i1, int i2, CATYPE val) -> void {
      if (i1 > i2) std::swap(i1, i2);
      mIdxCArea[j][i1] += val;
      if (i2 + 1 < QPlaneN) mIdxCArea[j][i2 + 1] -= val;
    };

    /*  →j x
     i↓ +----+----+----+----+
     y  |    |    |    |    |
        | +--|----|----|--+ |
        | |11|2222|2222|33| |
        +----+----+----+----+
        | |44|    |    |66| |
        | |44| 5  | 5  |66| |
        | |44|    |    |66| |
        +----+----+----+----+
        | |44|    |    |66| |
        | |44| 5  | 5  |66| |
        | |44|    |    |66| |
        +----+----+----+----+
        | |77|8888|8888|99| |
        | +--|----|----|--+ |
        |    |    |    |    |
        +----+----+----+----+
    */
   // 单个轨迹矩形贡献面积
    auto contribute = [&] (Point lu, Point rd) -> void {
      PPoint plu = getIdxByPoint(lu), prd = getIdxByPoint(rd);
      Box blu = getBoxByIdx(plu), bld = getBoxByIdx(PPoint(prd.i, plu.j)),
        bru = getBoxByIdx(PPoint(plu.i, prd.j)), brd = getBoxByIdx(prd);
      //std::cout << "contr " << lu.x << "," << lu.y << " <-> " << rd.x << "," << rd.y << std::endl;
      //std::cout << "contri " << plu.i << "," << plu.j << " <-> " << prd.i << "," << prd.j << std::endl;
      for (int j = plu.j; j <= prd.j; ++j) {
        CATYPE bottom = std::min(rd.y, blu.getBottom()) - lu.y;
        if (j == plu.j) { // (1, 4, 7)
          CATYPE right = std::min(rd.x, blu.getRight()) - lu.x;
          // 1: (lu, rd) vs blu, incl.: 1+3 or 1+7
          addsum(j, plu.i, plu.i, bottom * right);
          // 4
          if (plu.i + 1 < prd.i)
            addsum(j, plu.i + 1, prd.i - 1, height * right);
          // 7
          if (plu.i < prd.i)
            addsum(j, prd.i, prd.i, (rd.y - bld.top) * right);
        } else if (j == prd.j) { // (3, 6, 9)
          CATYPE right = rd.x - bru.left;
          // 3: (lu, rd) vs bru, incl.: 3+9
          addsum(j, plu.i, plu.i, bottom * right);
          // 6
          if (plu.i + 1 < prd.i)
            addsum(j, plu.i + 1, prd.i - 1, height * right);
          // 9
          if (plu.i < prd.i)
            addsum(j, prd.i, prd.i, (rd.y - brd.top) * right);
        } else { // (2, 5, 8)
          // 2: incl.: 2+8
          addsum(j, plu.i, plu.i, bottom * width);
          // 5
          if (plu.i + 1 < prd.i)
            addsum(j, plu.i + 1, prd.i - 1, height * width);
          // 8
          if (plu.i < prd.i)
            addsum(j, prd.i, prd.i, (rd.y - brd.top) * width);
        }
      }
    };

    // 对 track 轨迹进行扫描线
    events.clear();
    events.reserve(track.size() * 2);
    for (int i = 0; i < track.size(); ++i) {
      auto const& box = track[i];
      events.emplace_back(Event { i, static_cast<CATYPE>(box.left), static_cast<CATYPE>(box.top), static_cast<CATYPE>(box.bottom), EventType::Start });
      events.emplace_back(Event { i, static_cast<CATYPE>(box.right), static_cast<CATYPE>(box.top), static_cast<CATYPE>(box.bottom), EventType::End });
    }
    std::sort(events.begin(), events.end());
// define 是线段树，否则是集合
    #define SEGTREE
#ifndef SEGTREE
    std::multiset<Segment> yseg;
    yseg.insert(Segment { events[0].y1, events[0].y2 });
    for (int i = 1; i < events.size(); ++i) {
      // 贡献矩形面积
      if (!yseg.empty() && events[i-1].x != events[i].x) {
        CATYPE lx = events[i-1].x, rx = events[i].x;
        // 循环更新每段 yseg（复杂度劣化！）
        auto it = yseg.begin();
        while (it != yseg.end()) {
          CATYPE ly = it->s, ry = it->e;
          auto it2 = it; ++it2;
          while (it2 != yseg.end() && it2->s <= ry) {
            // 拓展
            ry = std::max(ry, it2->e);
            it = it2++;
          }
          it = it2;
          contribute(Point(lx, ly), Point(rx, ry)); // 贡献合并段
        }
      }
      auto const& e = events[i];
      if (e.type == EventType::Start) {
        yseg.insert(Segment { e.y1, e.y2 });
      } else {
        yseg.erase(yseg.find(Segment { e.y1, e.y2 }));
      }
    }
#else
    // 离散化
    std::vector<CATYPE> unidx;
    unidx.reserve(events.size() * 2);
    for (int i = 0; i < events.size(); ++i) {
      unidx.emplace_back(events[i].y1);
      unidx.emplace_back(events[i].y2);
    }
    std::sort(unidx.begin(), unidx.end());
    unidx.erase(std::unique(unidx.begin(), unidx.end()), unidx.end());
    // 映射
    std::unordered_map<CATYPE, int> idx;
    std::unordered_map<int, CATYPE> ridx;
    for (int i = 0; i < unidx.size(); ++i) {
      idx[unidx[i]] = i;
      ridx[i] = unidx[i];
    }
    // 线段树（统计覆盖次数）
    std::vector<int> seg(unidx.size() * 8, 0); // 当前线段覆盖次数
    std::vector<bool> full(unidx.size() * 8, false); // 全覆盖
    std::vector<bool> empty(unidx.size() * 8, true); // 空覆盖
    int L, R; // global for _update
    auto _update = [&] (auto&& _update, int l, int r, int rt, int val) -> void {
      int lc = rt << 1, rc = rt << 1 | 1;
      if (L <= l && r <= R) {
        // 覆盖该区间
        //std::cout << "update for l=" << l << "  r=" << r << "  val=" << val << std::endl;
        seg[rt] += val;
      pushup:
        full[rt] = (seg[rt] > 0) | (full[lc] & full[rc]);
        empty[rt] = (seg[rt] == 0) & (empty[lc] & empty[rc]);
        return;
      }
      int mid = (l + r) >> 1;
      if (L <= mid) _update(_update, l, mid, lc, val);
      if (R > mid) _update(_update, mid + 1, r, rc, val);
      goto pushup;
    };
    auto update = [&] (int l, int r, int val) -> void {
      //std::cout << "update  l=" << l << "  r=" << r << "  val=" << val << std::endl;
      L = l, R = r;
      _update(_update, 0, unidx.size() - 1, 1, val);
    };
    int LX = 0, RX = 0; // global for _query
    int YA = -1, YB = -1, LLX = 0, LRX = 0; // global for lazy_contribute
    auto lazy_contribute = [&] (int ya, int yb) -> void {
      if (YA == -1) {
      reset:
        YA = ya, YB = yb;
        LLX = LX, LRX = RX;
        return;
      } else if (YB == ya && LX == LLX && RX == LRX) {
        YB = yb;
      } else {
        contribute(Point(LLX, ridx[YA]), Point(LRX, ridx[YB]));
        goto reset;
      }
    };
    auto _query = [&] (auto&& _query, int l, int r, int rt) -> void {
      if (full[rt]) {
        // 全覆盖，可以更新
        //std::cout << "full[rt]  l=" << l << "  r=" << r << std::endl;
        //contribute(Point(LX, ridx[l]), Point(RX, ridx[r+1]));
        lazy_contribute(l, r + 1);
        return;
      }
      int mid = (l + r) >> 1, lc = rt << 1, rc = rt << 1 | 1;
      if (l<r && !empty[lc]) _query(_query, l, mid, lc);
      if (l<r && !empty[rc]) _query(_query, mid + 1, r, rc);
    };
    auto query = [&] (int xl, int xr) -> void {
      LX = xl, RX = xr;
      _query(_query, 0, unidx.size() - 1, 1);
    };
    // 扫描线
    for (int i = 0; i < events.size(); ++i) {
      auto const& e = events[i];
      update(idx[e.y1], idx[e.y2] - 1, (e.type == EventType::Start) ? 1 : -1);
      if (i + 1 < events.size() && events[i].x != events[i + 1].x) {
        query(static_cast<CATYPE>(events[i].x), static_cast<CATYPE>(events[i + 1].x));
      }
    }
    LX=RX=-1, lazy_contribute(-1, -1);
#endif

    // 前缀和
    for (int j = 0; j < QPlaneN; ++j) {
      for (int i = 1; i < QPlaneN; ++i) {
        mIdxCArea[j][i] += mIdxCArea[j][i-1];
      }
    }

    // 二维前缀和 (i)
    for (int j = 0; j < QPlaneN; ++j) {
      for (int i = 1; i < QPlaneN; ++i) {
        mIdxCArea[j][i] += mIdxCArea[j][i-1];
      }
    }

    // 二维前缀和 (j)
    for (int j = 1; j < QPlaneN; ++j) {
      for (int i = 0; i < QPlaneN; ++i) {
        mIdxCArea[j][i] += mIdxCArea[j-1][i];
      }
    }
  }

  // 根据 CArea 插入
  void insertByCArea(const std::vector<DetectorState>& track, int trackid) {
    CATYPE width = mBox.width / QPlaneN, height = mBox.height / QPlaneN;

    // 根据扫描线临时数据 events 找到 (lu, rd) 所包含的所有轨迹段（复杂度 O(k*√n)
    auto insert = [&] (Node* node, PPoint lu, PPoint rd, const std::vector<DetectorState>& cur) -> void {
//      Box box = getMinBox(cur);
//      node->values.emplace_back(Tracks { cur.front().frameIndex, cur.back().frameIndex, trackid, box });
        node ->values.add(cur.front().frameIndex, cur.back().frameIndex,Tracks { cur.front().frameIndex, cur.back().frameIndex, trackid, cur});
    };

    // 自顶向下插入
    auto f = [&] (auto&& f, Node* node, PPoint lu, PPoint rd, const std::vector<DetectorState>& cur) -> void {
      if (node == nullptr) return;
      // 优化：面积和，现计算
      CATYPE carea = mIdxCArea[rd.j][rd.i] - (lu.i > 0 ? mIdxCArea[rd.j][lu.i - 1] : 0) -
          (lu.j > 0 ? mIdxCArea[lu.j - 1][rd.i] : 0) + (lu.i > 0 && lu.j > 0 ? mIdxCArea[lu.j - 1][lu.i - 1] : 0);
      if (carea == 0) return; // 优化：不再向下
      std::vector<DetectorState> tmp;
      Box blu = this->getBoxByIdx(lu), brd = this->getBoxByIdx(rd);
      Box box = Box { blu.left, blu.top, brd.getRight() - blu.left, brd.getBottom() - blu.top };
      // 检测相交（复杂度劣化！）
      for (auto const& v : cur) {
        DetectorState tmpv = this->intersection(box, v);
        // 兼容现有代码
        if (tmpv.frameIndex != -1) {
          tmp.emplace_back(tmpv);
        }
      }
      if (this->isLeaf(node)) { // 叶子结点，即使比率不及 Alpha 也插入
        insert(node, lu, rd, tmp);
        return;
      }
      // 比率足够，插入
      //std::cout << node->code << " -> " << carea << " / " << (rd.i - lu.i + 1) * height << " " << (rd.j - lu.j + 1) * width << std::endl;
      if (carea >= Alpha * box.width * box.height) {
        insert(node, lu, rd, tmp);
        return;
      }
      // 比率不足，继续向下
      auto mid = PPoint((lu.i + rd.i) >> 1, (lu.j + rd.j) >> 1); // 中心点
      f(f, node->children[0].get(), lu, mid, tmp);
      f(f, node->children[1].get(), PPoint(lu.i, mid.j + 1), PPoint(mid.i, rd.j), tmp);
      f(f, node->children[2].get(), PPoint(mid.i + 1, lu.j), PPoint(rd.i, mid.j), tmp);
      f(f, node->children[3].get(), mid + 1, rd, tmp);
    };

    f(f, mRoot.get(), PPoint(0, 0), PPoint(QPlaneN - 1, QPlaneN - 1), track);
  }

  // 按照面积贡献，插入轨迹段
  void buildTreeCA(const std::map<int, std::vector<DetectorState>>& tracks) {
    // 初始化
    initArea();
    
    // 根据 trackid 插入（兼容现有代码）
    for (auto const& item : tracks)
    {
      const std::vector<DetectorState>& track = item.second;
      int trackid = item.first;
      //std::cout << "trackid:" << trackid << " size:" << track.size() << std::endl;
      clearCArea();
      calcCArea(track);
      insertByCArea(track, trackid);
    }
  }

  // 调试输出
  void debugPrintTrack() {
    auto f = [&] (auto&& f, Node* node, PPoint lu, PPoint rd) -> void {
      if (node == nullptr) return;
      if (isLeaf(node)) {
        return;
      }
      Box blu = getBoxByIdx(lu), brd = getBoxByIdx(rd);
      int left = blu.left, right = brd.getRight(), top = blu.top, bottom = brd.getBottom();
      std::cout << "["<<node->code << "]  ("<<lu.i<<","<<lu.j<<") <-> "<<"("<<rd.i<<","<<rd.j<<")  "<<"l:"<<left << " t:"<< top << " r:"<<right<<" b:"<<bottom<<"  ->  " << node->carea<<std::endl;
      std::cout << "    -> " << node->values.size() << " (";
      for (auto const& v : node->values) {
        std::cout << v.framestart << "," << v.frameend << " ";
      }
      std::cout << ")" << std::endl;
      auto mid = PPoint((lu.i + rd.i) >> 1, (lu.j + rd.j) >> 1); // 中心点
      f(f, node->children[0].get(), lu, mid);
      f(f, node->children[1].get(), PPoint(lu.i, mid.j + 1), PPoint(mid.i, rd.j));
      f(f, node->children[2].get(), PPoint(mid.i + 1, lu.j), PPoint(rd.i, mid.j));
      f(f, node->children[3].get(), mid + 1, rd);
    };
    f(f, mRoot.get(), PPoint(0, 0), PPoint(QPlaneN - 1, QPlaneN - 1));
  }

  void  bianli2()
  {
    int num = 0, cnt = 0;
    vector<int>vec;
    cout << " size : " << sizeof(Node) << endl;
    query2(mRoot.get(), num, cnt, vec);
    // cout << "索引的大小 ： " << num << "-->>" << num * sizeof(Node) / 1024.0 << " kb" << endl;
    // cout << "有数据的节点数量：" << cnt << endl;
    cout << "非叶子节点的轨迹数量:" << num << " 叶子节点的轨迹数量:" << cnt << endl;
    // set<int> res(vec.begin(), vec.end());
    // cout << "车辆总数：" << res.size() << endl;
  }

  std::vector<T> query_time(vector<Point> hull, int st, int ed) const {
    auto values = std::vector<T>();
    query(mRoot.get(), mBox, hull, values, st, ed);
    return values;
  }

  void query_stop_pointer(vector<Point> hull1, int st, int ed) const {
    auto values = std::vector<pair<Node*, pair<int, int>>>();
    auto baohan = std::vector<bool>();
    query_pointer(mRoot.get(), mBox, hull1, values, st, ed, baohan);
    int len1 = values.size();
    unordered_set<int>answer;
    for (int i = 0;i < len1;i++) {
      while (values[i].second.first <= values[i].second.second && values[i].first->values[values[i].second.first]->box.frameIndex + 45 < values[i].first->values[values[i].second.second]->box.frameIndex) {
        int l = values[i].second.first, r = values[i].second.second;
        int tmp = values[i].first->values[values[i].second.first]->box.frameIndex + 45;
        int res_st = l;
        while (l <= r) {
          int mid = l + ((r - l) >> 1);
          int res_time = values[i].first->values[mid]->box.frameIndex;
          if (res_time < tmp)
            l = mid + 1;
          else
            r = mid - 1;
        }
        res_st = l;
        if (res_st > values[i].second.second || values[i].first->values[res_st]->box.frameIndex < tmp)
          break;
        unordered_map<int, int>hash;
        while (values[i].second.first <= values[i].second.second && values[i].first->values[values[i].second.first]->box.frameIndex == tmp - 45) {
          hash[values[i].first->values[values[i].second.first]->box.track_id] = 1;
          values[i].second.first++;
        }
        for (int j = res_st;j <= values[i].second.second;j++) {
          if (tmp != values[i].first->values[j]->box.frameIndex)
            break;
          if (hash[values[i].first->values[j]->box.track_id] == 1) {
            answer.insert(values[i].first->values[j]->box.track_id);
          }
        }
        hash.clear();
      }

    }
    for (auto it = answer.begin();it != answer.end();it++) {
      cout << *it << endl;
    }
  }

  void query_stop_pointer2(vector<Point> hull1, int st, int ed) const {
    auto values = std::vector<pair<Node*, pair<int, int>>>();
    auto baohan = std::vector<bool>();
    query_pointer(mRoot.get(), mBox, hull1, values, st, ed, baohan);
    int len1 = values.size();
    unordered_set<int>answer;
    int tag = false;
    for (int i = 0;i < len1;i++) {
      if (values[i].second.first <= values[i].second.second && values[i].first->values[values[i].second.first]->box.frameIndex + 100 < values[i].first->values[values[i].second.second]->box.frameIndex) {
        tag = true;
        break;
      }
    }
    while (tag) {
      int tmp = 0;
      int min_time = 0x3f3f3f3f, min_loc;
      for (int i = 0;i < len1;i++) {

        if (values[i].second.first <= values[i].second.second) {
          min_time = values[i].first->values[values[i].second.first]->box.frameIndex;
          min_loc = i;
          tmp = i;
          break;
        }
      }

      for (int i = tmp + 1;i < len1;i++) {
        if (values[i].second.first <= values[i].second.second) {
          if (min_time > values[i].first->values[values[i].second.first]->box.frameIndex) {
            min_time = values[i].first->values[values[i].second.first]->box.frameIndex;
            min_loc = i;
          }
        }
      }
      unordered_map<int, int>hash;
      for (int i = 0;i < len1;i++) {
        while (values[i].second.first <= values[i].second.second && values[i].first->values[values[i].second.first]->box.frameIndex == min_time) {
          hash[values[i].first->values[values[i].second.first]->box.track_id] = 1;
          values[i].second.first++;
        }
      }


      for (int i = 0;i < len1;i++) {
        int l = values[i].second.first, r = values[i].second.second;
        int res_st = l;
        while (l <= r) {
          int mid = l + ((r - l) >> 1);
          int res_time = values[i].first->values[mid]->box.frameIndex;
          if (res_time < min_time + 100)
            l = mid + 1;
          else
            r = mid - 1;
        }
        res_st = l;
        if (res_st > values[i].second.second || values[i].first->values[res_st]->box.frameIndex < min_time + 100) {
          continue;
        }
        for (int j = res_st;j <= values[i].second.second;j++) {
          if (min_time + 100 != values[i].first->values[j]->box.frameIndex)
            break;
          if (hash[values[i].first->values[j]->box.track_id] == 1) {
            answer.insert(values[i].first->values[j]->box.track_id);
          }
        }

      }
      hash.clear();
      tag = false;
      for (int i = 0;i < len1;i++) {
        if (values[i].second.first <= values[i].second.second && values[i].first->values[values[i].second.second]->box.frameIndex > min_time + 100) {
          tag = true;
          break;
        }
      }
    }
    cout << "???????" << endl;
    for (auto it = answer.begin();it != answer.end();it++) {
      cout << *it << endl;
    }
  }



  void query_time_pointer(vector<Point> hull1, vector<Point> hull2, int st, int ed) const {
    auto values1 = std::vector<pair<Node*, pair<int, int>>>();
    auto baohan1 = std::vector<bool>();
    query_pointer(mRoot.get(), mBox, hull1, values1, st, ed, baohan1);
    auto values2 = std::vector<pair<Node*, pair<int, int>>>();
    auto baohan2 = std::vector<bool>();
    query_pointer(mRoot.get(), mBox, hull2, values2, st, ed, baohan2);

    int len1 = values1.size(), len2 = values2.size();
    /// 排序有问题
    priority_queue< cmp_tarckid > que1, que2;
    for (int i = 0;i < len1;i++) {
      sort(values1[i].first->values.begin() + values1[i].second.first, values1[i].first->values.begin() + values1[i].second.second + 1, compTrackid);
      cmp_tarckid tmp;
      tmp.st = values1[i].second.first;
      tmp.ed = values1[i].second.second;
      tmp.node = values1[i].first;
      que1.push(tmp);
    }

    for (int i = 0;i < len2;i++) {
      sort(values2[i].first->values.begin() + values2[i].second.first, values2[i].first->values.begin() + values2[i].second.second + 1, compTrackid);
      cmp_tarckid tmp;
      tmp.st = values2[i].second.first;
      tmp.ed = values2[i].second.second;
      tmp.node = values2[i].first;
      que2.push(tmp);
    }

    set<int> zhuanwanA_C, zhuanwanC_A;
    while (!que1.empty() && !que2.empty()) {
      cmp_tarckid now1 = que1.top();
      cmp_tarckid now2 = que2.top();
      int is1 = IsPointInPolygon(Point(now1.node->values[now1.st]->box.left, now1.node->values[now1.st]->box.top), hull1);
      int is2 = IsPointInPolygon(Point(now2.node->values[now2.st]->box.left, now2.node->values[now2.st]->box.top), hull2);
      if (!is1 && !is2) {
        que1.pop();
        if (now1.st < now1.ed) {
          now1.st++;
          que1.push(now1);
        }
        que2.pop();
        if (now2.st < now2.ed) {
          now2.st++;
          que2.push(now2);
        }
        continue;
      }
      else if (!is2) {
        que2.pop();
        if (now2.st < now2.ed) {
          now2.st++;
          que2.push(now2);
        }
        continue;
      }
      else if (!is1) {
        que1.pop();
        if (now1.st < now1.ed) {
          now1.st++;
          que1.push(now1);
        }
        continue;
      }
      if (now1.node->values[now1.st]->box.track_id < now2.node->values[now2.st]->box.track_id) {
        if (now1.st < now1.ed) {
          now1.st++;
          que1.pop();
          que1.push(now1);
        }
        else
          que1.pop();
      }
      else if (now1.node->values[now1.st]->box.track_id > now2.node->values[now2.st]->box.track_id) {
        if (now2.st < now2.ed) {
          now2.st++;
          que2.pop();
          que2.push(now2);
        }
        else
          que2.pop();
      }
      else {
        que1.pop();
        if (now1.st < now1.ed) {
          now1.st++;
          que1.push(now1);
        }
        que2.pop();
        if (now2.st < now2.ed) {
          now2.st++;
          que2.push(now2);
        }
        if (now1.node->values[now1.st]->box.frameIndex < now2.node->values[now2.st]->box.frameIndex)
          zhuanwanA_C.insert(now1.node->values[now1.st]->box.track_id);
        else
          zhuanwanC_A.insert(now1.node->values[now1.st]->box.track_id);
      }
    }
    cout << "-----------------------------------------" << endl;
    for (auto it = zhuanwanA_C.begin();it != zhuanwanA_C.end();it++) {
      cout << *it << endl;
    }
    cout << "-----------------------------------------" << endl;
    cout << zhuanwanA_C.size() << " zhuanwan  ::  " << zhuanwanC_A.size() << endl;
  }

  std::vector<Leaf> query_time_B(vector<Point> hull, int st, int ed) const {
    auto values = std::vector<Leaf>();
    query_BT(mRoot.get(), mBox, hull, values, st, ed);
    return values;
  }

  double dcmp(double x) const {
    if (fabs(x) < eps) return 0;
    else return x < 0 ? -1 : 1;
  }
  bool SegmentProperIntersection(const Point& a1, const Point& a2, const Point& b1, const Point& b2) const {
    double c1 = Cross(a2 - a1, b1 - a1), c2 = Cross(a2 - a1, b2 - a1),
      c3 = Cross(b2 - b1, a1 - b1), c4 = Cross(b2 - b1, a2 - b1);
    return dcmp(c1) * dcmp(c2) < 0 && dcmp(c3) * dcmp(c4) < 0;
  }
  bool OnSegment(const Point& p, const Point& a1, const Point& a2) const {
    return dcmp(Cross(a1 - p, a2 - p)) == 0 && dcmp(Dot(a1 - p, a2 - p)) < 0;
  }
  // 点集凸包
  vector<Point> ConvexHull(vector<Point> q) const {
    // 预处理，删除重复点
    vector<Point> p = q;
    sort(p.begin(), p.end());
    p.erase(unique(p.begin(), p.end()), p.end());

    int n = p.size();
    int m = 0;
    vector<Point> ch(n + 1);
    for (int i = 0; i < n; i++) {
      while (m > 1 && Cross(ch[m - 1] - ch[m - 2], p[i] - ch[m - 2]) <= 0) m--;
      ch[m++] = p[i];
    }
    int k = m;
    for (int i = n - 2; i >= 0; i--) {
      while (m > k && Cross(ch[m - 1] - ch[m - 2], p[i] - ch[m - 2]) <= 0) m--;
      ch[m++] = p[i];
    }
    if (n > 1) m--;
    ch.resize(m);
    return ch;
  }
  //点是否在凸包内
  int IsPointInPolygon(const Point& p, const vector<Point>& poly) const {
    int wn = 0;
    int n = poly.size();
    for (int i = 0; i < n; ++i) {
      const Point& p1 = poly[i];
      const Point& p2 = poly[(i + 1) % n];
      if (p1 == p || p2 == p || OnSegment(p, p1, p2)) return 2;//在边界上
      int k = dcmp(Cross(p2 - p1, p - p1));
      int d1 = dcmp(p1.y - p.y);
      int d2 = dcmp(p2.y - p.y);
      if (k > 0 && d1 <= 0 && d2 > 0) wn++;
      if (k < 0 && d2 <= 0 && d1 > 0) wn--;
    }
    return wn != 0;
  }
  ///判断2个凸包是否相交
  bool ConvexPolygonDisjoint(const vector<Point> ch1, const vector<Point> ch2) const {
    int c1 = ch1.size();
    int c2 = ch2.size();
    for (int i = 0; i < c1; ++i)
      if (IsPointInPolygon(ch1[i], ch2) != 0) return true;
    for (int i = 0; i < c2; ++i)
      if (IsPointInPolygon(ch2[i], ch1) != 0) return true;
    for (int i = 0; i < c1; ++i)
      for (int j = 0; j < c2; ++j)
        if (SegmentProperIntersection(ch1[i], ch1[(i + 1) % c1], ch2[j], ch2[(j + 1) % c2])) return true;
    return false;
  }

  /// 判断ch1是否被ch2包含
  /// \param ch1 凸包
  /// \param ch2 凸包
  /// \return
  bool IsConvexInPolygon(const vector<Point> ch1, const vector<Point> ch2) const {
    int c1 = ch1.size();
    for (int i = 0; i < c1; i++) {
      if (!IsPointInPolygon(ch1[i], ch2))
        return false;
    }
    return true;
  }


  bool ConvexPolygonDisjoint(const vector<Point> ch1, const Box box) const {
    vector<Point> ch2;
    Point point = Point(box.left, box.top);
    ch2.push_back(point);
    point = Point(box.left, box.top + box.height);
    ch2.push_back(point);
    point = Point(box.left + box.width, box.top + box.height);
    ch2.push_back(point);
    point = Point(box.left + box.width, box.top);
    ch2.push_back(point);
    ch2 = ConvexHull(ch2);
    int c1 = ch1.size();
    int c2 = ch2.size();
    for (int i = 0; i < c1; ++i)
      if (IsPointInPolygon(ch1[i], ch2) != 0) return true;
    for (int i = 0; i < c2; ++i)
      if (IsPointInPolygon(ch2[i], ch1) != 0) return true;
    for (int i = 0; i < c1; ++i)
      for (int j = 0; j < c2; ++j)
        if (SegmentProperIntersection(ch1[i], ch1[(i + 1) % c1], ch2[j], ch2[(j + 1) % c2])) return true;
    return false;
  }

  static bool compTrackid(const T& value1, const T& value2) {
    return value1->box.track_id < value2->box.track_id;
  }

  struct cmp_tarckid {
    int st;
    int ed;
    Node* node;
    friend bool operator < (const  cmp_tarckid& a, const cmp_tarckid& b)
    {
      if (a.node->values[a.st]->box.track_id > b.node->values[b.st]->box.track_id)
        return true;
      return false;
    }
  };

  ///  edit by wmz 建立节点code和对应的四叉树box的映射关系
  void buildIndex(InvertedIndex<std::string, DetectorState>& myindex)
  {
    makeIndex(mRoot.get(), myindex);
  }


  //  edit by wmz建立track_id和对应的四叉树box的映射关系   
  void makeMap(std::map<int, std::vector<std::string>>& mmap)
  {
    /// 建立track_id和对应的四叉树box的映射关系
//    edit by wmz on 2023/11/14
    makeMap(mRoot.get(), mmap);
  }


  //    edit by wmz 输入：查询多边形框 输出：包含的叶子节点编码
  void querycode(vector<Point>& queryPull, std::vector<string>& values)
  {
    query(mRoot.get(), mBox, queryPull, values);
  }

    Node* getRoot() {
        return mRoot.get();
    }
    Box getmBox() {
        return mBox;
    }
    void queryT(Node* node, const Box& box, int st, int et, const Box& queryBox, std::vector<Tracks>& values) const
    {
        assert(node != nullptr);
        assert(queryBox.intersects(box));
//        for (const auto& value : node->values)
//        {
//            if (queryBox.intersects(mGetBox(value)))
//            {
//                if (queryBox.baohan(mGetBox(value)))
//                    values.push_back(value);
//            }
//
//        }
        // std::cout <<"box:"<< box.width << ' ' << box.height << std::endl;
        // std::cout << "queryBox:"<< queryBox.width << ' ' << queryBox.height << std::endl;

        if(box.left <= queryBox.left && box.top <= queryBox.top && box.left + box.width >= queryBox.left + queryBox.width && box.top + box.height >= queryBox.top + queryBox.height){
	        node->values.query(st, et, values);
        }
        else{
          node->values.query(st, et, values);
        }
        if (!isLeaf(node))
        {
            for (auto i = std::size_t(0); i < node->children.size(); ++i)
            {
                auto childBox = computeBox(box, static_cast<int>(i));

                if (queryBox.intersects(childBox)){
			queryT(node->children[i].get(), childBox, st, et, queryBox, values);
		}
                               }
        }
    }
    void queryT(int st, int et, std::vector<Tracks>& ans) {
        queryT(mRoot.get(), mBox, st, et, mBox, ans);
    }
    void queryT(const Box & queryBox, int st, int et, std::vector<Tracks>& ans) {
        queryT(mRoot.get(), mBox, st, et, queryBox, ans);
    }
private:
  ///阈值
  static constexpr auto Threshold = std::size_t(3);

   std::size_t QPlaneN = 1 << MaxDepth; // 平面四叉树的边长
  
  //    检测框
  Box mBox;
  //    节点指针
  std::unique_ptr<Node> mRoot;
  //    比较器
  Equal mEqual;
  GetBox mGetBox;

  bool isLeaf(const Node* node) const
  {
    return !static_cast<bool>(node->children[0]);
  }

  //    获得本节点的子节点对应的格子，根据0 1 2 3返回不同位置的子格子
  Box computeBox(const Box& box, int i) const
  {
    auto origin = box.getTopLeft();
    auto childSize = box.getSize() / static_cast<float>(2);
    switch (i)
    {
      // North West
    case 0:
      return Box(origin, childSize);
      // Norst East
    case 1:
      return Box(Vector2(origin.x + childSize.x, origin.y), childSize);
      // South West
    case 2:
      return Box(Vector2(origin.x, origin.y + childSize.y), childSize);
      // South East
    case 3:
      return Box(origin + childSize, childSize);
    default:
      assert(false && "Invalid child index");
      return Box();
    }
  }

  //确定valueBox应该插入到nodeBox的哪一个节点中去
  int getQuadrant(const Box& nodeBox, const Box& valueBox) const
  {
    auto center = nodeBox.getCenter();
    // West
    if (valueBox.getRight() < center.x)
    {
      // North West
      if (valueBox.getBottom() < center.y)
        return 0;
      // South West
      else if (valueBox.top >= center.y)
        return 2;
      // Not contained in any quadrant
      else
        return -1;
    }
    // East
    else if (valueBox.left >= center.x)
    {
      // North East
      if (valueBox.getBottom() < center.y)
        return 1;
      // South East
      else if (valueBox.top >= center.y)
        return 3;
      // Not contained in any quadrant
      else
        return -1;
    }
    // Not contained in any quadrant
    else
      return -1;
  }

  //add by wmz  判断valuebox都和nodebox中哪些子节点相交
  std::vector<int> getIntersectingQuadrants(const Box& nodeBox, const Box& valueBox) const
  {
    std::vector<int> intersectingQuadrants;
    int childleft, childright, childtop, childbottom;
    // Check North West quadrant
    childleft = nodeBox.left;
    childtop = nodeBox.top;
    childbottom = nodeBox.top + nodeBox.height / 2;
    childright = nodeBox.left + nodeBox.width / 2;
    if (!(childleft > valueBox.getRight() || childright < valueBox.left ||
      childtop > valueBox.getBottom() || childbottom < valueBox.top))
    {
      intersectingQuadrants.push_back(0);
    }
    // Check North East quadrant
    childleft = nodeBox.left + nodeBox.width / 2;
    childtop = nodeBox.top;
    childbottom = nodeBox.top + nodeBox.height / 2;
    childright = nodeBox.getRight();
    if (!(childleft > valueBox.getRight() || childright < valueBox.left ||
      childtop > valueBox.getBottom() || childbottom < valueBox.top))
    {
      intersectingQuadrants.push_back(1);
    }

    // Check South West quadrant
    childleft = nodeBox.left;
    childtop = nodeBox.top + nodeBox.height / 2;
    childbottom = nodeBox.getBottom();
    childright = nodeBox.left + nodeBox.width / 2;
    if (!(childleft > valueBox.getRight() || childright < valueBox.left ||
      childtop > valueBox.getBottom() || childbottom < valueBox.top))
    {
      intersectingQuadrants.push_back(2);
    }
    // Check South East quadrant
    childleft = nodeBox.left + nodeBox.width / 2;
    childtop = nodeBox.top + nodeBox.height / 2;
    childbottom = nodeBox.getBottom();
    childright = nodeBox.getRight();
    if (!(childleft > valueBox.getRight() || childright < valueBox.left ||
      childtop > valueBox.getBottom() || childbottom < valueBox.top))
    {
      intersectingQuadrants.push_back(3);
    }
    //        test错误,正常不可能有这种情况，一定是与某个象限相交的。
    if (intersectingQuadrants.size() == 0)
    {
      //            cout<<center.x<<" "<<center.y<<endl;
      cout << valueBox.left << " " << valueBox.top << " " << valueBox.getRight() << " " << valueBox.getBottom() << endl;
      //            break;
    }
    return intersectingQuadrants;
  }

  // edit by wmz 建立一个满四叉树
  void makeFullTree(Node* node, const Box& box, std::size_t depth, const std::size_t MaxDepth) {
    assert(node != nullptr);
    // 基本情况：如果当前深度等于最大深度，停止递归
    if (depth == MaxDepth) {
      return;
    }
    // 为当前节点创建子节点
    for (auto& child : node->children)
    {
      child = std::make_unique<Node>();
    }
    node->children[0]->code = node->code + "01";
    node->children[1]->code = node->code + "00";
    node->children[2]->code = node->code + "10";
    node->children[3]->code = node->code + "11";
    // 递归地为每个子节点创建子树
    for (std::size_t i = 0; i < node->children.size(); ++i) {
      auto childBox = computeBox(box, static_cast<int>(i));
      makeFullTree(node->children[i].get(), childBox, depth + 1, MaxDepth);
    }
  }

  // edit by wmz 获得轨迹集合的最小包围框
  Box getMinBox(const std::vector<DetectorState>& tracks) {
    if (tracks.empty()) {
      return Box{ 0, 0, 0, 0, -1, -1 }; // 如果没有轨迹框，则返回一个空的Box
    }

    double minLeft = std::numeric_limits<double>::max();
    double maxRight = std::numeric_limits<double>::lowest();
    double minTop = std::numeric_limits<double>::max();
    double maxBottom = std::numeric_limits<double>::lowest();

    // 遍历所有轨迹框，找到最小包围框的边界
    for (const auto& track : tracks) {
      minLeft = std::min(minLeft, static_cast<double>(track.left));
      maxRight = std::max(maxRight, static_cast<double>(track.right));
      minTop = std::min(minTop, static_cast<double>(track.top));
      maxBottom = std::max(maxBottom, static_cast<double>(track.bottom));
    }
    double width = maxRight - minLeft;
    double height = maxBottom - minTop;
    // 创建并返回最小包围框
    return Box(minLeft, minTop, width, height, -1, -1); // track_id 和 frameIndex 设置为-1，表示不特定
  }

  // edit by wmz求轨迹检测框和树节点包围框的交集，返回新的检测框
  DetectorState intersection(const Box& box, const DetectorState& track) {
    int left = std::max(box.left, static_cast<double>(track.left));
    int top = std::max(box.top, static_cast<double>(track.top));
    int right = std::min(box.getRight(), static_cast<double>(track.right));
    int bottom = std::min(box.getBottom(), static_cast<double>(track.bottom));

    // 如果没有交集或只是边界重合，返回空矩形框
    if (left >= right || top >= bottom) {
      return DetectorState(-1, 0, 0, 0, 0, -1);
    }
    return DetectorState(track.frameIndex, left, top, right, bottom, track.track_id);
  }

  // edit by wmz 求轨迹检测框集合的并集面积，扫描线算法+线段树+离散化（leetcode 850）
  int getTracksArea(std::vector<DetectorState>& tracks)
  {
    ScanLine func;
    return func.rectangleArea(tracks);
  }

  // 核心函数：向节点中插入每个轨迹段
  void insert(Node* node, const Box& box, std::vector<DetectorState>& track, int& cnt)
  {
    assert(node != nullptr);
    if (track.size() <= 0) {
      return;
    }
    int trackid = track.front().track_id;
    // ！！将原先轨迹检测框集合和当前节点做交集，得到新的轨迹检测框集合
    std::vector<DetectorState> preprocess;
    for (auto item : track) {
      DetectorState tmp = intersection(box, item);
      if (tmp.frameIndex != -1)
        preprocess.push_back(std::move(tmp));
    }
    // cout << "before:" << track.size() << " after:" << preprocess.size() << endl;
    if (preprocess.size() <= 0)
      return;
    auto minbox = getMinBox(preprocess);
    if (isLeaf(node)) {
      ++cnt;
      node->values.add(preprocess.front().frameIndex, preprocess.back().frameIndex, Tracks(preprocess.front().frameIndex, preprocess.back().frameIndex, trackid, track));
    }
    else {
      // 注释后两行将ratio=0为插入叶子节点
      double ratio = 0; //插入叶子节点
//      std::cout << ratio;
//       double tracks_area = static_cast<double>(getTracksArea(preprocess));
//       double node_area = box.getArea();
//       double ratio = tracks_area / node_area;
//std::cerr<<"yes";
      // cout << "preprocess.size():" << preprocess.size() << " track_area:" << tracks_area << " node_area:" << node_area << " ratio:" << ratio << endl;
      if (ratio >= Alpha) {
        ++cnt;
        node->values.add(preprocess.front().frameIndex, preprocess.back().frameIndex, Tracks(preprocess.front().frameIndex, preprocess.back().frameIndex, trackid, track));
      }
      else {
        for (int i = 0;i < 4;++i) {
          insert(node->children[static_cast<std::size_t>(i)].get(), computeBox(box, i), preprocess, cnt);
        }
      }
    }
  }

  //　四叉树插入一个节点 old version
  // modify by wmz 改成如果value与多个子节点相交则分别插入每个子节点
  void add(Node* node, std::size_t depth, const Box& box, const T& value)
  {
    assert(node != nullptr);
    //        判断是不是和当前节点对应区域相交
    assert(box.intersects(mGetBox(value)));
    //        如果是叶子节点
    if (isLeaf(node))
    {
      // Insert the value in this node if possible
//             深度达到阈值了，或者节点没有达到限制
      if (depth >= MaxDepth || node->values.size() < Threshold) {
        node->values.insert(value);
      }
      //             分裂节点，重新插入
      else
      {
        split(node, box);
        add(node, depth, box, value);
      }
    }
    //        非叶子节点
    else
    {
      auto vec = getIntersectingQuadrants(box, mGetBox(value));
      if (vec.size() > 0) {
        for (auto i : vec)
        {
          add(node->children[static_cast<std::size_t>(i)].get(), depth + 1, computeBox(box, i), value);
        }
      }
      //            出错了
      else {
        cout << "add error" << endl;
      }
    }
  }
  /// 四叉树的分裂操作
  // edit by wmz 加入了四叉树节点编码
  void split(Node* node, const Box& box)
  {
    assert(node != nullptr);
    assert(isLeaf(node) && "Only leaves can be split");
    // Create children
    for (auto& child : node->children)
    {
      child = std::make_unique<Node>();
    }
    //        edit by wmz
    //        注意要给所有的分裂后的字节点都分配编码,如果只分配一部分(在下边的循环内才分配,会出现问题);
    node->children[0]->code = node->code + "01";
    node->children[1]->code = node->code + "00";
    node->children[2]->code = node->code + "10";
    node->children[3]->code = node->code + "11";

    // Assign values to children
//        auto newValues = std::vector<T>(); // New values for this node
//        auto newValues_id = std::vector<T>();
    for (const auto& value : node->values)
    {
      //          edit by wmz
      //          改成了将轨迹插入到所有相交的子节点中
      auto vec = getIntersectingQuadrants(box, mGetBox(value));
      if (vec.size() > 0) {
        for (auto i : vec)
        {
          node->children[static_cast<std::size_t>(i)]->values.insert(value);
        }
      }
      //            出错
      else {
        cout << "split error" << endl;
      }
    }
    //        更新原来格子对应的value
    node->values.clear();
  }

  /// 遍历整棵树
  void query1(Node* node, int& noleaf, int& leaf, vector<int>& vec) {
    if (!isLeaf(node)) {
      noleaf++;
      for (auto i = std::size_t(0); i < node->children.size(); ++i)
        query1(node->children[i].get(), noleaf, leaf, vec);
    }
    else {
      leaf++;
      //node->bPTree->display(node->bPTree->getRoot());
      vec.push_back(node->values.size());
    }
  }

  /// 遍历整棵树
  void query2(Node* node, int& num, int& cnt, vector<int>& vec) {
    if (!isLeaf(node)) {
      if (node->values.size() > 0)
      {
        num += node->values.size();
        // std::cout << "node code:" << node->code << " tracksize:" << node->values.size() << std::endl;
        // for (int i = 0;i < node->values.size();++i)
        // {
        //   std::cout << node->values[i].trackid << " ";
        // }
        // std::cout << std::endl;
      }
      for (auto i = std::size_t(0); i < node->children.size(); ++i)
        query2(node->children[i].get(), num, cnt, vec);
    }
    else {
      if (node->values.size() > 0)
      {
        cnt += node->values.size();
        // std::cout << "node code:" << node->code << " tracksize:" << node->values.size() << std::endl;
        // for (int i = 0;i < node->values.size();++i)
        // {
        //   std::cout << node->values[i].trackid << " ";
        // }
        // std::cout << std::endl;
      }
    }
  }

  /// 遍历整棵树,建立节点track_id和对应四叉树节点编码的映射关系
//    edit by wmz on 2023/11/14
  void makeMap(Node* node, std::map<int, std::vector<std::string>>& mmap) {
    if (!isLeaf(node))
    {
      if (node->values.size() > 0)
      {
        for (auto item : node->values)
        {
          mmap[item.trackid].push_back(node->code);
        }
      }
      for (auto i = std::size_t(0); i < node->children.size(); ++i)
      {
        makeMap(node->children[i].get(), mmap);
      }
    }
    else
    {
      for (auto item : node->values)
      {
        mmap[item.trackid].push_back(node->code);
      }
    }
  }

  //    遍历整棵树,建立节点code和对应的轨迹box的映射关系
  //    edit by wmz
  void makeIndex(Node* node, InvertedIndex<std::string, DetectorState>& myindex) {
    if (!isLeaf(node)) {
      for (auto i = std::size_t(0); i < node->children.size(); ++i)
        makeIndex(node->children[i].get(), myindex);
    }
    else {
      //    edit by wmz on 2023/11/15
//            cout<<"leaf->code:"<<node->code<<endl;
      for (auto item : node->values)
      {
        myindex.add(node->code, std::move(DetectorState(item->box.frameIndex, item->box.left, item->box.top, item->box.getRight(), item->box.getBottom(), item->box.track_id)));
      }
    }
  }


  //    edit by wmz 根据查询框 返回对应的叶子节点编码
  void query(Node* node, const Box& box, vector<Point>& queryPull, std::vector<string>& values) const {
    assert(node != nullptr);
    if (!isLeaf(node))
    {
      for (auto i = std::size_t(0); i < node->children.size(); ++i)
      {
        auto childBox = computeBox(box, static_cast<int>(i));
        vector<Point> childBox1;
        Point point = Point(childBox.left, childBox.top);
        childBox1.push_back(point);
        point = Point(childBox.left, childBox.top + childBox.height);
        childBox1.push_back(point);
        point = Point(childBox.left + childBox.width, childBox.top);
        childBox1.push_back(point);
        point = Point(childBox.left + childBox.width, childBox.top + childBox.height);
        childBox1.push_back(point);
        //                如果和子节点相交
        if (ConvexPolygonDisjoint(ConvexHull(queryPull), ConvexHull(childBox1)))
          query(node->children[i].get(), childBox, queryPull, values);
      }
    }
    else {
      vector<Point> Box1;
      Point point = Point(box.left, box.top);
      Box1.push_back(point);
      point = Point(box.left, box.getBottom());
      Box1.push_back(point);
      point = Point(box.getRight(), box.top);
      Box1.push_back(point);
      point = Point(box.getRight(), box.getBottom());
      Box1.push_back(point);
      //          如果和当前叶子节点相交就放入结果集合
      if (ConvexPolygonDisjoint(ConvexHull(queryPull), ConvexHull(Box1))) {
        values.push_back(node->code);
      }
    }
  }

  ///　仅支持空间查询，且查询区域必须是矩形
  void query(Node* node, const Box& box, const Box& queryBox, std::vector<T>& values) const
  {
      assert(node != nullptr);
      assert(queryBox.intersects(box));
      for (const auto& value : node->values)
      {
          if (queryBox.intersects(mGetBox(value)))
          {
              if (queryBox.baohan(mGetBox(value)))
                  values.push_back(value);
          }

      }
      if (!isLeaf(node))
      {
          for (auto i = std::size_t(0); i < node->children.size(); ++i)
          {
              auto childBox = computeBox(box, static_cast<int>(i));

              if (queryBox.intersects(childBox))
                  query(node->children[i].get(), childBox, queryBox, values);
          }
      }
  }
  /// 仅支持空间查询　　
  void query(Node* node, const Box& box, vector<Point>& queryPull, std::vector<T>& values) const {
    assert(node != nullptr);
    if (!isLeaf(node))
    {
      for (auto i = std::size_t(0); i < node->children.size(); ++i)
      {
        auto childBox = computeBox(box, static_cast<int>(i));
        vector<Point> childBox1;
        Point point = Point(childBox.left, childBox.top);
        childBox1.push_back(point);
        point = Point(childBox.left, childBox.top + childBox.height);
        childBox1.push_back(point);
        point = Point(childBox.left + childBox.width, childBox.top);
        childBox1.push_back(point);
        point = Point(childBox.left + childBox.width, childBox.top + childBox.height);
        childBox1.push_back(point);
        //                如果和子节点相交
        if (ConvexPolygonDisjoint(ConvexHull(queryPull), ConvexHull(childBox1)))
          query(node->children[i].get(), childBox, queryPull, values);
      }
    }
    else {
      vector<Point> Box1;
      Point point = Point(box.left, box.top);
      Box1.push_back(point);
      point = Point(box.left, box.getBottom());
      Box1.push_back(point);
      point = Point(box.getRight(), box.top);
      Box1.push_back(point);
      point = Point(box.getRight(), box.getBottom());
      Box1.push_back(point);
      //          如果完全在查询多边形里面
      if (IsConvexInPolygon(Box1, queryPull)) {
        for (const auto& value : node->values)
          values.push_back(value);
      }
      else {
        for (const auto& value : node->values)
        {
          Box box = mGetBox(value);
          Point p = Point(box.left, box.top);
          //                    如果边界框的左上角顶点在查询框也加入结果集
          if (IsPointInPolygon(p, queryPull)) {
            values.push_back(value);
          }
        }
      }

    }
  }
  ///　Ｂ+树查询　　支持时空查询
  void query_BT(Node* node, const Box& box, vector<Point>& queryPull, std::vector<Leaf>& values, int st, int ed) const {
    assert(node != nullptr);
    // cout<< queryPull[2].y << endl;
//        assert(ConvexPolygonDisjoint(ConvexHull(queryPull),ConvexHull(box1)));

    if (!isLeaf(node))
    {
      for (auto i = std::size_t(0); i < node->children.size(); ++i)
      {
        auto childBox = computeBox(box, static_cast<int>(i));
        vector<Point> childBox1;
        Point point = Point(childBox.left, childBox.top);
        childBox1.push_back(point);
        point = Point(childBox.left, childBox.top + childBox.height);
        childBox1.push_back(point);
        point = Point(childBox.left + childBox.width, childBox.top);
        childBox1.push_back(point);
        point = Point(childBox.left + childBox.width, childBox.top + childBox.height);
        childBox1.push_back(point);
        if (ConvexPolygonDisjoint(ConvexHull(queryPull), ConvexHull(childBox1)))
          query_BT(node->children[i].get(), childBox, queryPull, values, st, ed);
      }
    }
    else {
      {
        BNode* STNode = node->bPTree->search_key(st);
        BNode* EdNode = node->bPTree->search_key(ed);
        if (STNode != nullptr) {
          int idx = std::lower_bound(STNode->keys.begin(), STNode->keys.end(), st) - STNode->keys.begin();
          int idy;
          if (STNode == EdNode)
            idy = std::upper_bound(STNode->keys.begin(), STNode->keys.end(), ed) - STNode->keys.begin();
          else
            idy = STNode->keys.size();
          if (!((idx == STNode->keys.size() || STNode->keys[idx] < st))) {
            for (int i = idx;i < idy;i++)
              if (IsPointInPolygon(Point(STNode->ptr2TreeOrData.dataPtr[i].left, STNode->ptr2TreeOrData.dataPtr[i].top), queryPull))
                values.push_back(STNode->ptr2TreeOrData.dataPtr[i]);
          }

          while (STNode->ptr2next != EdNode && STNode != EdNode) {
            for (int i = 0;i < STNode->ptr2next->keys.size();i++) {
              if (IsPointInPolygon(Point(STNode->ptr2TreeOrData.dataPtr[i].left, STNode->ptr2TreeOrData.dataPtr[i].top), queryPull))
                values.push_back(STNode->ptr2next->ptr2TreeOrData.dataPtr[i]);
            }
            STNode = STNode->ptr2next;
          }
          if (EdNode != nullptr && STNode != EdNode) {
            idy = std::upper_bound(EdNode->keys.begin(), EdNode->keys.end(), ed) - EdNode->keys.begin();
            for (int i = 0;i < idy;i++) {
              if (IsPointInPolygon(Point(STNode->ptr2TreeOrData.dataPtr[i].left, STNode->ptr2TreeOrData.dataPtr[i].top), queryPull))
                values.push_back(EdNode->ptr2TreeOrData.dataPtr[i]);
            }
          }
        }
      }

    }
  }

  ///　vector存储　支持时空查询　
  void query(Node* node, const Box& box, vector<Point>& queryPull, std::vector<T>& values, int st, int ed) const {
    assert(node != nullptr);
    // cout<< queryPull[2].y << endl;
//        assert(ConvexPolygonDisjoint(ConvexHull(queryPull),ConvexHull(box1)));

    if (!isLeaf(node))
    {
      for (auto i = std::size_t(0); i < node->children.size(); ++i)
      {
        auto childBox = computeBox(box, static_cast<int>(i));
        vector<Point> childBox1;
        Point point = Point(childBox.left, childBox.top);
        childBox1.push_back(point);
        point = Point(childBox.left, childBox.top + childBox.height);
        childBox1.push_back(point);
        point = Point(childBox.left + childBox.width, childBox.top);
        childBox1.push_back(point);
        point = Point(childBox.left + childBox.width, childBox.top + childBox.height);
        childBox1.push_back(point);
        if (ConvexPolygonDisjoint(ConvexHull(queryPull), ConvexHull(childBox1)))
          query(node->children[i].get(), childBox, queryPull, values, st, ed);
      }
    }
    else {
      ///　二分查找界限
      int res_st = -1, res_ed = -1;
      int l = 0, r = node->values.size() - 1;
      while (l <= r) {
        int mid = l + ((r - l) >> 1);
        int res_time = node->values[mid]->box.frameIndex;
        if (res_time <= ed) {
          l = mid + 1;
        }
        else {
          r = mid - 1;
        }
      }
      res_ed = l;
      l = 0, r = node->values.size() - 1;
      while (l <= r) {
        int mid = l + ((r - l) >> 1);
        int res_time = node->values[mid]->box.frameIndex;
        if (res_time < st)
          l = mid + 1;
        else
          r = mid - 1;
      }
      res_st = l;

      vector<Point> Box1;
      Point point = Point(box.left, box.top);
      Box1.push_back(point);
      point = Point(box.left, box.getBottom());
      Box1.push_back(point);
      point = Point(box.getRight(), box.top);
      Box1.push_back(point);
      point = Point(box.getRight(), box.getBottom());
      Box1.push_back(point);
      if (IsConvexInPolygon(Box1, queryPull)) {
        for (int i = res_st;i < res_ed;i++) {
          values.push_back(node->values[i]);
        }
      }
      else {
        for (int i = res_st;i < res_ed;i++) {
          Box box = mGetBox(node->values[i]);
          Point p = Point(box.left, box.top);
          if (IsPointInPolygon(p, queryPull)) {
            values.push_back(node->values[i]);
          }
        }
      }
    }
  }


  ///　查找符合空间范围的指针
  void query_pointer(Node* node, const Box& box, vector<Point>& queryPull, std::vector<pair<Node*, pair<int, int>>>& values, int st, int ed, vector<bool>& baohan) const {
    assert(node != nullptr);
    // cout<< queryPull[2].y << endl;
//        assert(ConvexPolygonDisjoint(ConvexHull(queryPull),ConvexHull(box1)));

    if (!isLeaf(node))
    {
      for (auto i = std::size_t(0); i < node->children.size(); ++i)
      {
        auto childBox = computeBox(box, static_cast<int>(i));
        vector<Point> childBox1;
        Point point = Point(childBox.left, childBox.top);
        childBox1.push_back(point);
        point = Point(childBox.left, childBox.top + childBox.height);
        childBox1.push_back(point);
        point = Point(childBox.left + childBox.width, childBox.top);
        childBox1.push_back(point);
        point = Point(childBox.left + childBox.width, childBox.top + childBox.height);
        childBox1.push_back(point);
        if (ConvexPolygonDisjoint(ConvexHull(queryPull), ConvexHull(childBox1)))
          query_pointer(node->children[i].get(), childBox, queryPull, values, st, ed, baohan);
      }
    }
    else {
      int res_st = -1, res_ed = -1;
      int l = 0, r = node->values.size() - 1;
      while (l <= r) {
        int mid = l + ((r - l) >> 1);
        int res_time = node->values[mid]->box.frameIndex;
        if (res_time <= ed) {
          l = mid + 1;
        }
        else {
          r = mid - 1;
        }
      }
      res_ed = l;
      l = 0, r = node->values.size() - 1;
      while (l <= r) {
        int mid = l + ((r - l) >> 1);
        int res_time = node->values[mid]->box.frameIndex;
        if (res_time < st)
          l = mid + 1;
        else
          r = mid - 1;
      }
      res_st = l;
      vector<Point> Box1;
      Point point = Point(box.left, box.top);
      Box1.push_back(point);
      point = Point(box.left, box.getBottom());
      Box1.push_back(point);
      point = Point(box.getRight(), box.top);
      Box1.push_back(point);
      point = Point(box.getRight(), box.getBottom());
      Box1.push_back(point);
      if (IsConvexInPolygon(Box1, queryPull)) {
        baohan.push_back(true);
      }
      else {
        baohan.push_back(false);
      }

      if (res_st < res_ed) {
        pair<Node*, pair<int, int>> pointer;
        pointer.first = node;
        pointer.second.first = res_st;
        pointer.second.second = res_ed - 1;
        values.push_back(pointer);
      }

    }
  }
};

#endif //QUADTRANS_QUADTREE_H

