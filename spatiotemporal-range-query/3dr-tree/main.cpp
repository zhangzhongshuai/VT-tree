#include <iostream>
#include <cstring>
#include <unordered_set>
#include "DetectorState.h"
#include "JsonHelper.h"
#include "Box.h"
#include "Quadtree.h"
#include "3dr-tree.h"
#include <time.h>
#include <chrono>
#include <thread>
//#include "predicates.cpp"
#include "Scan.h"

using namespace std;
// #define DOUBLE
#define TIMES 1

struct node
{
  int x, y;
};

double dcmp(double x) {
  if (fabs(x) < eps) return 0;
  else return x < 0 ? -1 : 1;
}

struct trackInf {
  int trackId;
  int frameMin;
  int frameMax;
  trackInf(int trackid, int framemin, int framemax) {
    trackId = trackid;
    frameMin = framemin;
    frameMax = framemax;
  }
  trackInf() {}
};

void trackSort(vector< trackInf >& shibuya_FrameId) {
  for (int i = shibuya_FrameId.size() - 1; i > 0; i--) {
    for (int j = 0; j + 1 <= i; j++) {
      if (shibuya_FrameId[j].trackId > shibuya_FrameId[j + 1].trackId) {
        trackInf tmp;
        tmp = shibuya_FrameId[j];
        shibuya_FrameId[j] = shibuya_FrameId[j + 1];
        shibuya_FrameId[j + 1] = tmp;
      }
    }
  }
}


void printTrackInf(vector< trackInf > tracks) {
  for (int i = 0; i < tracks.size(); i++)
    cout << tracks[i].trackId << ": [" << tracks[i].frameMin << "," << tracks[i].frameMax << "]" << endl;
  cout << endl;
}

bool TrackidInSet(unordered_set<int> track_set, int trackid) {
  for (auto iter = track_set.begin(); iter != track_set.end(); ++iter) {
    if (*iter == trackid)
      return true;
  }
  return false;
}

void stayCount(unordered_map<int, int> track_stay, int& count) {
  for (auto it = track_stay.begin();it != track_stay.end();it++) {
    // cout<<it->first<<" 出现了几次 ： "<<track_stay[it->first]<<endl;
    if (track_stay[it->first] > 150) {
      //cout << it->first << endl;
      count++;
    }
  }
}
Point3D getMyDataCoordinates(const Nodes& data) {
    return {(double)data.box.top, (double)data.box.left, (double)data.box.frameIndex};
}
int main() {

    srand(time(NULL));
  // read json from disk, parse it and store into unorderedmap
  int sumTime = 0, sumCarId = 0;
  JsonHelper jsonHelper;
  //    jsonHelper.readSingleJson("/home/shuai/miris/data-splited/uav/0011-baseline-1000.json");
  //    jsonHelper.readSingleJson("/home/shuai/miris/data-splited/uav/0011-baseline-2000.json");
  //    jsonHelper.readSingleJson("/home/shuai/miris/data-splited/uav/0011-baseline-3000.json");
  //    jsonHelper.readSingleJson("/home/shuai/miris/data-splited/uav/0011-baseline-4000.json");
  //    jsonHelper.readSingleJson("/home/shuai/miris/data-splited/uav/0011-baseline-5000.json");
  //    jsonHelper.readSingleJson("/home/shuai/miris/data-splited/uav/0011-baseline-6000.json");
  //    jsonHelper.readSingleJson("/home/shuai/miris/data-splited/uav/0011-baseline-all.json");

  //    jsonHelper.readSingleJson("/home/shuai/miris/data-splited/shibuya/2-baseline-6000.json");
  //    jsonHelper.readSingleJson("/home/shuai/miris/data-splited/shibuya/2-baseline-12000.json");
  //    jsonHelper.readSingleJson("/home/shuai/miris/data-splited/shibuya/2-baseline-18000.json");
  //    jsonHelper.readSingleJson("/home/shuai/miris/data-splited/shibuya/2-baseline-24000.json");
  //jsonHelper.readSingleJson("./0-baseline.json");


  //    jsonHelper.readSingleJson("/home/shuai/miris/data-splited/shibuya/2-baseline-all.json");

  //    jsonHelper.readSingleJson("/home/shuai/miris/data-splited/warsaw/2-baseline-6000.json");
  //    jsonHelper.readSingleJson("/home/shuai/miris/data-splited/warsaw/2-baseline-12000.json");
  //    jsonHelper.readSingleJson("/home/shuai/miris/data-splited/warsaw/2-baseline-18000.json");
  //    jsonHelper.readSingleJson("/home/shuai/miris/data-splited/warsaw/2-baseline-24000.json");
  //    jsonHelper.readSingleJson("/home/shuai/miris/data-splited/warsaw/2-baseline-30000.json");
  //    jsonHelper.readSingleJson("/home/shuai/miris/data-splited/warsaw/2-baseline-all.json");

  //vector <DetectorState> res;
  //res.reserve(jsonHelper.root.size()); // 预留空间
  //将所有数据存到res中
  //jsonHelper.parseJsonForMiris(res);
//   jsonHelper.parseJsonForMiris(res);
//   jsonHelper.parseJsonForMiris(res);

//   std::cout << res.size() << "---\n";

 vector <DetectorState> res;
  //jsonHelper.readSingleJson("./data/warsaw/json/0-baseline.json");
  vector <DetectorState> ans;
  //ans.reserve(jsonHelper.root.size()); // 预留空间
  //将所有数据存到res中
  //jsonHelper.parseJsonForMiris(ans);
  int pre_frame = 0, pre_trackid = 0;
//   for(auto &tt : ans){
//     int x = tt.frameIndex, y = tt.track_id;
//     pre_frame = max(pre_frame, x);
//     pre_trackid = max(pre_trackid, y);
//     res.push_back(tt);
//   }
  ans.clear();


  int pre = res.size();
  //jsonHelper.readSingleJson("./data/warsaw/json/1-baseline.json");
  //ans.reserve(jsonHelper.root.size()); // 预留空间
  //将所有数据存到res中
  //jsonHelper.parseJsonForMiris(ans);
  int xx = pre_frame, yy = pre_trackid;
//   for(DetectorState tt : ans){
//     tt.frameIndex += xx;
//     tt.track_id += yy;
//     int x = tt.frameIndex, y = tt.track_id;
//     pre_frame = max(pre_frame, x);
//     pre_trackid = max(pre_trackid, y);
//     res.push_back(tt);
//   }
//   ans.clear();

//   pre = res.size();
//   jsonHelper.readSingleJson("./data/warsaw/json/2-baseline.json");
//   ans.reserve(jsonHelper.root.size()); // 预留空间
//   //将所有数据存到res中
//   jsonHelper.parseJsonForMiris(ans);
//   xx = pre_frame, yy = pre_trackid;
//   for(DetectorState tt : ans){
//     tt.frameIndex += xx;
//     tt.track_id += yy;
//     int x = tt.frameIndex, y = tt.track_id;
//     pre_frame = max(pre_frame, x);
//     pre_trackid = max(pre_trackid, y);
//     res.push_back(tt);
//   }
//   ans.clear();

//   pre = res.size();
//   jsonHelper.readSingleJson("./data/warsaw/json/3-baseline.json");
//   ans.reserve(jsonHelper.root.size()); // 预留空间
//   //将所有数据存到res中
//   jsonHelper.parseJsonForMiris(ans);
//   xx = pre_frame, yy = pre_trackid;
//   for(DetectorState tt : ans){
//     tt.frameIndex += xx;
//     tt.track_id += yy;
//     int x = tt.frameIndex, y = tt.track_id;
//     pre_frame = max(pre_frame, x);
//     pre_trackid = max(pre_trackid, y);
//     res.push_back(tt);
//   }
//   ans.clear();

//   pre = res.size();
//   jsonHelper.readSingleJson("./data/warsaw/json/4-baseline.json");
//   ans.reserve(jsonHelper.root.size()); // 预留空间
//   //将所有数据存到res中
//   jsonHelper.parseJsonForMiris(ans);
//   xx = pre_frame, yy = pre_trackid;
//   for(DetectorState tt : ans){
//     tt.frameIndex += xx;
//     tt.track_id += yy;
//     int x = tt.frameIndex, y = tt.track_id;
//     pre_frame = max(pre_frame, x);
//     pre_trackid = max(pre_trackid, y);
//     res.push_back(tt);
//   }
//   ans.clear();

//   pre = res.size();
//   jsonHelper.readSingleJson("./data/warsaw/json/5-baseline.json");
//   ans.reserve(jsonHelper.root.size()); // 预留空间
//   //将所有数据存到res中
//   jsonHelper.parseJsonForMiris(ans);
//   xx = pre_frame, yy = pre_trackid;
//   for(DetectorState tt : ans){
//     tt.frameIndex += xx;
//     tt.track_id += yy;
//     int x = tt.frameIndex, y = tt.track_id;
//     pre_frame = max(pre_frame, x);
//     pre_trackid = max(pre_trackid, y);
//     res.push_back(tt);
//   }
//   ans.clear();

  pre = res.size();
  jsonHelper.readSingleJson("./data/shibuya/json/0-baseline.json");
  ans.reserve(jsonHelper.root.size()); // 预留空间
  //将所有数据存到res中
  jsonHelper.parseJsonForMiris(ans);
  xx = pre_frame, yy = pre_trackid;
  for(DetectorState tt : ans){
    tt.frameIndex += xx;
    tt.track_id += yy;
    int x = tt.frameIndex, y = tt.track_id;
    pre_frame = max(pre_frame, x);
    pre_trackid = max(pre_trackid, y);
    res.push_back(tt);
  }
  ans.clear();

  pre = res.size();
  jsonHelper.readSingleJson("./data/shibuya/json/1-baseline.json");
  ans.reserve(jsonHelper.root.size()); // 预留空间
  //将所有数据存到res中
  jsonHelper.parseJsonForMiris(ans);
  xx = pre_frame, yy = pre_trackid;
  for(DetectorState tt : ans){
    tt.frameIndex += xx;
    tt.track_id += yy;
    int x = tt.frameIndex, y = tt.track_id;
    pre_frame = max(pre_frame, x);
    pre_trackid = max(pre_trackid, y);
    res.push_back(tt);
  }
  ans.clear();

  pre = res.size();
  jsonHelper.readSingleJson("./data/shibuya/json/2-baseline.json");
  ans.reserve(jsonHelper.root.size()); // 预留空间
  //将所有数据存到res中
  jsonHelper.parseJsonForMiris(ans);
  xx = pre_frame, yy = pre_trackid;
  for(DetectorState tt : ans){
    tt.frameIndex += xx;
    tt.track_id += yy;
    int x = tt.frameIndex, y = tt.track_id;
    pre_frame = max(pre_frame, x);
    pre_trackid = max(pre_trackid, y);
    res.push_back(tt);
  }
  ans.clear();

  pre = res.size();
  jsonHelper.readSingleJson("./data/shibuya/json/3-baseline.json");
  ans.reserve(jsonHelper.root.size()); // 预留空间
  //将所有数据存到res中
  jsonHelper.parseJsonForMiris(ans);
  xx = pre_frame, yy = pre_trackid;
  for(DetectorState tt : ans){
    tt.frameIndex += xx;
    tt.track_id += yy;
    int x = tt.frameIndex, y = tt.track_id;
    pre_frame = max(pre_frame, x);
    pre_trackid = max(pre_trackid, y);
    res.push_back(tt);
  }
  ans.clear();

  pre = res.size();
  jsonHelper.readSingleJson("./data/shibuya/json/4-baseline.json");
  ans.reserve(jsonHelper.root.size()); // 预留空间
  //将所有数据存到res中
  jsonHelper.parseJsonForMiris(ans);
  xx = pre_frame, yy = pre_trackid;
  for(DetectorState tt : ans){
    tt.frameIndex += xx;
    tt.track_id += yy;
    int x = tt.frameIndex, y = tt.track_id;
    pre_frame = max(pre_frame, x);
    pre_trackid = max(pre_trackid, y);
    res.push_back(tt);
  }
  ans.clear();

  pre = res.size();
  jsonHelper.readSingleJson("./data/shibuya/json/5-baseline.json");
  ans.reserve(jsonHelper.root.size()); // 预留空间
  //将所有数据存到res中
  jsonHelper.parseJsonForMiris(ans);
  xx = pre_frame, yy = pre_trackid;
  for(DetectorState tt : ans){
    tt.frameIndex += xx;
    tt.track_id += yy;
    int x = tt.frameIndex, y = tt.track_id;
    pre_frame = max(pre_frame, x);
    pre_trackid = max(pre_trackid, y);
    res.push_back(tt);
  }
  ans.clear();

  int fid_st = 0;
  int fid_ed = pre_frame;

  int maxTime = 0;
  for (int i = 0; i < res.size(); i++) {
    maxTime = max(maxTime, res[i].frameIndex);
  }

  int maxCarId = 0;
  for (int i = 0; i < res.size(); i++) {
    maxCarId = max(maxCarId, res[i].track_id);
  }
  sumCarId = sumCarId + maxCarId + 1;

  cout << "帧的个数：   " << maxTime + 1 << endl;
  cout << "车的个数：   " << sumCarId << endl;
  int vec_len = res.size();
  cout << " 轨迹点detection 个数   ： " << vec_len << endl;
  auto nodes = std::vector<Nodes>();

  // key:track_id,valuelist：车辆轨迹
//   map<int, vector<DetectorState>> tracks;
  //以检测框插入轨迹
  


  // cout << tracks.size() << endl;
  // for (auto item : tracks)
  // {
  //   cout << item.first << ":" << item.second.size() << endl;
  // }
//   for (auto item : tracks)
//   {
//     vector<DetectorState>& tmp = item.second;
//     sort(tmp.begin(), tmp.end(), [](DetectorState& lft, DetectorState& rft) {return lft.frameIndex < rft.frameIndex;});
//   }

  //std::cout << "Press any key to continue ......"; getchar(); // profiler 控制点

  clock_t startTime, endTime;
  startTime = clock();
  int maxLeft = 0, maxTop = 0;
  for (int i = 0; i < vec_len; i++) {
    // tracks[res[i].track_id].push_back(DetectorState(res[i].frameIndex, res[i].left, res[i].top, res[i].right, res[i].bottom, res[i].track_id));
    maxLeft = max(maxLeft, res[i].left);
    maxTop = max(maxTop, res[i].top);
  }
    
  TreeNode<Nodes> r_tree(0.0, maxTop, 0.0, maxLeft, 0.0, maxTime, 4); //建立3dr-tree
  for (int i = 0; i < vec_len; i++) {
    Nodes node;
    node.box.left = res[i].left;
    node.box.top = res[i].top;
    node.box.width = res[i].right - res[i].left;
    node.box.height = res[i].bottom - res[i].top;
    node.box.track_id = res[i].track_id;
    node.box.frameIndex = res[i].frameIndex;
    // r_tree.insert(node, getMyDataCoordinates);
    for(int j = 1; j <= TIMES; ++j){
        node.box.left = res[i].left;
        node.box.top = res[i].top;
        node.box.width = res[i].right - res[i].left;
        node.box.height = res[i].bottom - res[i].top;
        node.box.track_id = res[i].track_id + vec_len * (j - 1);
        node.box.frameIndex = res[i].frameIndex;
        r_tree.insert(node, getMyDataCoordinates);
    }
    #ifdef DOUBLE
    node.box.left = res[i].left;
    node.box.top = res[i].top;
    node.box.width = res[i].right - res[i].left;
    node.box.height = res[i].bottom - res[i].top;
    node.box.track_id = res[i].track_id + vec_len;
    node.box.frameIndex = res[i].frameIndex;
    r_tree.insert(node, getMyDataCoordinates);
    #endif
  }
//   auto box = Box(0.0, 0.0, 2048.0, 2048.0);
//   auto getBox = [](Nodes* node) {
//     return node->box;
//     };
//   auto quadtree = Quadtree<Nodes*, decltype(getBox)>(box, getBox);
//   // 建立一个满四叉树，默认层高为6 ，根节点是0层
//   quadtree.makeFullTree();
//   // 测试数据
//   //tracks = {{0, {{0, 1, 550, 1100, 1100, 0}, {0, 800, 800, 1200, 1200, 1}, {0, 1000, 1000, 1500, 1500, 2}}}};
//   //tracks = {{1, {{0,1252,531,1310,577,0},{0,1306,645,1408,718,1}}}};
//   // 将全部轨迹段插入四叉树
// //  quadtree.buildTree(tracks); //扫描线，有重复计算
//    quadtree.buildTreeCA(tracks);


  endTime = clock();
  cout << "The 建树 time is: " << (double)(endTime - startTime) / CLOCKS_PER_SEC << "  s" << endl;

  // map<int, vector<string>> track_to_box;
  // quadtree.makeMap(track_to_box);
  // cout << "track_to_box size is:" << track_to_box.size() << endl;
  // for (auto item1 : track_to_box)
  // {
  //   cout << "trackid:" << item1.first << " node size:" << item1.second.size() << endl;
  // }

  //    遍历一下四叉树
//   quadtree.bianli2();
//   cerr<<cse::MODIFY_TIMES;


  startTime = clock();


    //queryT(Node* node, const Box& box, int t1, int t2 const Box& queryBox, std::vector<T>& values)
    double findt = 0, cleart = 0;
#define MULTIQUERY
#ifdef MULTIQUERY
    int n;
    cin >> n;
    int cnt = 0;
    for(int i = 0; i < n; ++i){
        // std::vector<Tracks> ans;
        int x = rand(), y = rand(), t = rand();
        auto currentTime = chrono::steady_clock::now();
        auto a = chrono::time_point_cast<chrono::microseconds>(currentTime).time_since_epoch().count();
        auto ans = r_tree.range_query(0, x, 0, y, 0, t, getMyDataCoordinates);
        currentTime = chrono::steady_clock::now();
        auto b = chrono::time_point_cast<chrono::microseconds>(currentTime).time_since_epoch().count();
        for(Nodes node : ans){
            if(node.box.left + node.box.width <= x){
                ++cnt;
            }
        }
        currentTime = chrono::steady_clock::now();
        auto c = chrono::time_point_cast<chrono::microseconds>(currentTime).time_since_epoch().count();
        findt += (b - a);
        cleart += (c - b);
        // printf("%.2lf\n", (double)(b - a));
    }
#else

    auto ans = r_tree.range_query(0, maxTop, 0, maxLeft, 0, 10000, getMyDataCoordinates);
    cout << endl <<"查询结果“未去重”个数："<<ans.size() << endl;
#endif
    endTime = clock();
    cout << cnt << endl;
    cout << "findt = " << (double)findt / 1000000 << "s" << endl;
    cout << "cleart = " << (double)cleart / 1000000 << "s" << endl;
    cout << "The query time is: " << (double)(endTime - startTime) / CLOCKS_PER_SEC << "s" << endl;
  //     //shibuya
  //     //A路口
  //     vector<Point>hull_A;
  //     Point p = Point(0.0,0.0);
  //     hull_A.push_back(p);
  //     p = Point(0.0,525.0);
  //     hull_A.push_back(p);
  //     p = Point(550.0,525.0);
  //     hull_A.push_back(p);
  //     p = Point(1200.0,420.0);
  //     hull_A.push_back(p);
  //     p = Point(1200.0,0.0);
  //     hull_A.push_back(p);

  //     vector<string> leafcode;
  // //    返回区域对应的全部叶子节点编码
  //     quadtree.querycode(hull_A,leafcode);

  //     vector<DetectorState> res1;
  //     myindex.query(leafcode,res1);

  //     cout<<"路口A对应的节点共:"<<res1.size()<<"条轨迹"<<endl;
  // //    for(auto item: res1)
  // //    {
  // //        item.print();
  // //        cout<<endl;
  // //    }

  //
  //    //B
  //    vector<Point>hull_B;
  //    p = Point(678.0*2,209.0*2);
  //    hull_B.push_back(p);
  //    p = Point(795.0*2,148.0*2);
  //    hull_B.push_back(p);
  //    p = Point(906.0*2,182.0*2);
  //    hull_B.push_back(p);
  //    p = Point(843.0*2,251.0*2);
  //    hull_B.push_back(p);
  //
  //    //C路口
  //    vector<Point>hull_C;
  //    p = Point(1640.0,630.0);
  //    hull_C.push_back(p);
  //    p = Point(1040.0,1080.0);
  //    hull_C.push_back(p);
  //    p = Point(1920.0,1080.0);
  //    hull_C.push_back(p);
  //    p = Point(1920.0,630.0);
  //    hull_C.push_back(p);
  //
  //    //D
  //    vector<Point>hull_D;
  //    p = Point(10.0*2,312.0*2);
  //    hull_D.push_back(p);
  //    p = Point(193.0*2,300.0*2);
  //    hull_D.push_back(p);
  //    p = Point(428.0*2,539.0*2);
  //    hull_D.push_back(p);
  //    p = Point(10.0*2,539.0*2);
  //    hull_D.push_back(p);
  //
  //    startTime = clock();
  //
  //    /// 二分查找A C路口的车辆
  //    auto values_a= quadtree.query_time(hull_A,fid_st,fid_ed);//(hull_A,31000,92999);
  //    auto values_b = quadtree.query_time(hull_B,fid_st,fid_ed);
  //    auto values_c= quadtree.query_time(hull_C, fid_st, fid_ed);
  //    auto values_d = quadtree.query_time(hull_D,fid_st,fid_ed);
  //
  //
  //    unordered_map<int,int> track_time_shibuya_a;
  //    unordered_map<int,int> track_time_shibuya_b;
  //    unordered_map<int,int> track_time_shibuya_c;
  //
  //    unordered_set<int> track_b_a;
  //    unordered_set<int> track_c_a;
  //    unordered_set<int> track_b_c;
  //    unordered_set<int> track_b_d;
  //    unordered_set<int> track_c_d;
  //
  ////建立A路口轨迹ID和frame_id的映射
  //    for(int i=0;i<values_a.size();i++){
  //        track_time_shibuya_a[values_a[i]->box.track_id]=values_a[i]->box.frameIndex;
  //    }
  //
  ////    B向A转弯的车辆
  //    for(int i=0; i < values_b.size(); i++){
  //        if(track_time_shibuya_a[values_b[i]->box.track_id]){
  //            if(values_b[i]->box.frameIndex < track_time_shibuya_a[values_b[i]->box.track_id]){
  //                track_b_a.insert(values_b[i]->box.track_id);
  //            }
  //        }
  //    }
  //
  ////    C向A转弯的车辆
  //    for(int i=0; i < values_c.size(); i++){
  //        if(track_time_shibuya_a[values_c[i]->box.track_id]){
  //            if(values_c[i]->box.frameIndex < track_time_shibuya_a[values_c[i]->box.track_id]){
  //                track_c_a.insert(values_c[i]->box.track_id);
  //            }
  //        }
  //    }
  //
  ////    存B路口的车
  //    for(int i=0;i<values_b.size();i++){
  //        track_time_shibuya_b[values_b[i]->box.track_id]=values_b[i]->box.frameIndex;
  //    }
  //
  ////    B向C拐弯的车辆
  //    for(int i=0; i < values_c.size(); i++){
  //        if(track_time_shibuya_b[values_c[i]->box.track_id]){
  //            if(values_c[i]->box.frameIndex > track_time_shibuya_b[values_c[i]->box.track_id]){
  //
  //                track_b_c.insert(values_c[i]->box.track_id);
  //            }
  //        }
  //    }
  //
  ////    B向D拐弯
  //    for(int i=0; i < values_d.size(); i++){
  //        if(track_time_shibuya_b[values_d[i]->box.track_id]){
  //            if(values_d[i]->box.frameIndex > track_time_shibuya_b[values_d[i]->box.track_id]){
  //                track_b_d.insert(values_d[i]->box.track_id);
  //            }
  //        }
  //    }
  //// C路口的车辆映射
  //    for(int i=0;i<values_c.size();i++){
  //        track_time_shibuya_c[values_c[i]->box.track_id]=values_c[i]->box.frameIndex;
  //    }
  //
  ////    C向D拐弯的车辆
  //    for(int i=0; i < values_d.size(); i++){
  //        if(track_time_shibuya_c[values_d[i]->box.track_id]){
  //            if(values_d[i]->box.frameIndex > track_time_shibuya_c[values_d[i]->box.track_id]){
  //                track_c_d.insert(values_d[i]->box.track_id);
  //            }
  //        }
  //    }
  //
  //    endTime = clock();
  //
  //    cout << "The shibuya 查找拐弯车辆用时: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s" << endl;
  //    cout<<"shibuya B向A转弯的车辆:"<<track_b_a.size()<<endl;
  //    cout<<"shibuya C向A转弯的车辆:"<<track_c_a.size()<<endl;
  //    cout<<"shibuya B向C拐弯的车辆:"<<track_b_c.size()<<endl;
  //    cout<<"shibuya B向D拐弯的车辆:"<<track_b_d.size()<<endl;
  //    cout<<"shibuya C向D拐弯的车辆:"<<track_c_d.size()<<endl;
  //
  //
  //    // 查询指定区域内停留的车辆
  //    vector<Point>hull_S_1;
  //    p = Point(710.0*2,197.0*2);
  //    hull_S_1.push_back(p);
  //    p = Point(853.0*2,234.0*2);
  //    hull_S_1.push_back(p);
  //    p = Point(832.0*2,250.0*2);
  //    hull_S_1.push_back(p);
  //    p = Point(676.0*2,207.0*2);
  //    hull_S_1.push_back(p);
  //
  //    vector<Point>hull_S_2;
  //    p = Point(838.0*2,364.0*2);
  //    hull_S_2.push_back(p);
  //    p = Point(960.0*2,420.0*2);
  //    hull_S_2.push_back(p);
  //    p = Point(960.0*2,540.0*2);
  //    hull_S_2.push_back(p);
  //    p = Point(590.0*2,540.0*2);
  //    hull_S_2.push_back(p);
  //
  //    vector<Point>hull_S_3;
  //    p = Point(191.0*2,304.0*2);
  //    hull_S_3.push_back(p);
  //    p = Point(394.0*2,512.0*2);
  //    hull_S_3.push_back(p);
  //    p = Point(62.0*2,492.0*2);
  //    hull_S_3.push_back(p);
  //    p = Point(53.0*2,332.0*2);
  //    hull_S_3.push_back(p);
  //
  //    vector<Point>hull_S_4;
  //    p = Point(258.0*2,231.0*2);
  //    hull_S_4.push_back(p);
  //    p = Point(533.0*2,184.0*2);
  //    hull_S_4.push_back(p);
  //    p = Point(598.0*2,209.0*2);
  //    hull_S_4.push_back(p);
  //    p = Point(272.0*2,260.0*2);
  //    hull_S_4.push_back(p);
  //
  //    vector<Point>hull_S_5;
  //    p = Point(297.0*2,255.0*2);
  //    hull_S_5.push_back(p);
  //    p = Point(799.0*2,279.0*2);
  //    hull_S_5.push_back(p);
  //    p = Point(839.0*2,345.0*2);
  //    hull_S_5.push_back(p);
  //    p = Point(210.0*2,307.0*2);
  //    hull_S_5.push_back(p);
  //
  //    startTime = clock();
  //
  //    auto values_stay_1= quadtree.query_time(hull_S_1, fid_st, fid_ed);
  //    auto values_stay_2= quadtree.query_time(hull_S_2, fid_st, fid_ed);
  //    auto values_stay_3= quadtree.query_time(hull_S_3, fid_st, fid_ed);
  //    auto values_stay_4= quadtree.query_time(hull_S_4, fid_st, fid_ed);
  //    auto values_stay_5= quadtree.query_time(hull_S_5, fid_st, fid_ed);
  //
  //    //方法一： 即可转换成在这个区间内出现了多少次
  //    unordered_map<int,int> track_stay_1;
  //    for(int i=0; i < values_stay_1.size(); i++){
  //        track_stay_1[values_stay_1[i]->box.track_id]++;
  //    }
  //    unordered_map<int,int> track_stay_2;
  //    for(int i=0; i < values_stay_2.size(); i++){
  //        track_stay_2[values_stay_2[i]->box.track_id]++;
  //    }
  //    unordered_map<int,int> track_stay_3;
  //    for(int i=0; i < values_stay_3.size(); i++){
  //        track_stay_3[values_stay_3[i]->box.track_id]++;
  //    }
  //    unordered_map<int,int> track_stay_4;
  //    for(int i=0; i < values_stay_4.size(); i++){
  //        track_stay_4[values_stay_4[i]->box.track_id]++;
  //    }
  //    unordered_map<int,int> track_stay_5;
  //    for(int i=0; i < values_stay_5.size(); i++){
  //        track_stay_5[values_stay_5[i]->box.track_id]++;
  //    }
  //
  //    int weiguitingche = 0;
  //    stayCount(track_stay_1, weiguitingche);
  //    stayCount(track_stay_2, weiguitingche);
  //    stayCount(track_stay_3, weiguitingche);
  //    stayCount(track_stay_4, weiguitingche);
  //    stayCount(track_stay_5, weiguitingche);
  //    endTime = clock();
  //    cout << "The shibuya-parking time is: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s" << endl;
  //    cout<<"违规停车的个数 ： " << weiguitingche <<endl;


  /****************************************next is null********************************************************/



  //    startTime = clock();
  //
  //    endTime = clock();
  //    cout << "The 时空查询 time is: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s" << endl;

  //    //方法二： 进入和离开这个区域的时间帧
  //    unordered_map<int,int> track_stay_st;
  //    unordered_map<int,int> track_stay_ed;
  //    for(int i=0; i < values_stay_1.size(); i++){
  //        if(track_stay_st[values_stay_1[i]->box.track_id] == 0)
  //            track_stay_st[values_stay_1[i]->box.track_id]=values_stay_1[i]->box.frameIndex;
  //        else
  //            track_stay_st[values_stay_1[i]->box.track_id]=min(track_stay_st[values_stay_1[i]->box.track_id], values_stay_1[i]->box.frameIndex);
  //        track_stay_ed[values_stay_1[i]->box.track_id]=max(track_stay_ed[values_stay_1[i]->box.track_id], values_stay_1[i]->box.frameIndex);
  //    }
  //
  //    //uav
  //    //uav_A
  //    vector<Point>hull_uav_A;
  //    p = Point(181.0*2,223.0*2);
  //    hull_uav_A.push_back(p);
  //    p = Point(353.0*2,223.0*2);
  //    hull_uav_A.push_back(p);
  //    p = Point(353.0*2,540.0*2);
  //    hull_uav_A.push_back(p);
  //    p = Point(181.0*2,540.0*2);
  //    hull_uav_A.push_back(p);
  //
  //    //uav_B
  //    vector<Point>hull_uav_B;
  //    p = Point(392.0*2,88.0*2);
  //    hull_uav_B.push_back(p);
  //    p = Point(960.0*2,88.0*2);
  //    hull_uav_B.push_back(p);
  //    p = Point(960.0*2,321.0*2);
  //    hull_uav_B.push_back(p);
  //    p = Point(392.0*2,321.0*2);
  //    hull_uav_B.push_back(p);
  //
  //    startTime = clock();
  //
  //    auto values_uav_a = quadtree.query_time(hull_uav_A,fid_st,fid_ed);
  //    auto values_uav_b = quadtree.query_time(hull_uav_B,fid_st,fid_ed);
  //
  //    unordered_map<int,int> track_time_uav;
  //    unordered_set<int> track_uav_a_b;
  //    unordered_set<int> track_uav_b_a;
  //
  //    for(int i=0;i<values_uav_a.size();i++){
  //        track_time_uav[values_uav_a[i]->box.track_id]=values_uav_a[i]->box.frameIndex;
  //    }
  //
  //    for(int i=0; i < values_uav_b.size(); i++){
  //        if(track_time_uav[values_uav_b[i]->box.track_id]){
  //            if(values_uav_b[i]->box.frameIndex > track_time_uav[values_uav_b[i]->box.track_id]){
  //                track_uav_a_b.insert(values_uav_b[i]->box.track_id);
  //            }
  //        }
  //    }
  //
  //    endTime = clock();
  //    cout << " uav A->B: " << track_uav_a_b.size() <<endl;
  //    cout << "The uav time is: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s" << endl;
  //
  //
  //
  //    //warsaw
  //    //warsaw_l
  //    vector<Point>hull_warsaw_l;
  //   // Point p = Point(0.0,0.0);
  //    p = Point(0,610);
  //    hull_warsaw_l.push_back(p);
  //    p = Point(930,610);
  //    hull_warsaw_l.push_back(p);
  //    p = Point(930,1080);
  //    hull_warsaw_l.push_back(p);
  //    p = Point(0,1080);
  //    hull_warsaw_l.push_back(p);
  //
  //    //warsaw_r
  //    vector<Point>hull_warsaw_r;
  //    p = Point(1190,700);
  //    hull_warsaw_r.push_back(p);
  //    p = Point(1920,700);
  //    hull_warsaw_r.push_back(p);
  //    p = Point(1920,1080);
  //    hull_warsaw_r.push_back(p);
  //    p = Point(1190,1080);
  //    hull_warsaw_r.push_back(p);
  //    //warsaw_t
  //    vector<Point>hull_warsaw_t;
  //    p = Point(978,337);
  //    hull_warsaw_t.push_back(p);
  //    p = Point(1150,680);
  //    hull_warsaw_t.push_back(p);
  //    p = Point(1450,680);
  //    hull_warsaw_t.push_back(p);
  //    p = Point(1580,590);
  //    hull_warsaw_t.push_back(p);
  //    p = Point(1107,325);
  //    hull_warsaw_t.push_back(p);
  //    //warsaw_b
  //    vector<Point>hull_warsaw_b;
  //    p = Point(1920,685);
  //    hull_warsaw_b.push_back(p);
  //    p = Point(1645,669);
  //    hull_warsaw_b.push_back(p);
  //    p = Point(1400,780);
  //    hull_warsaw_b.push_back(p);
  //    p = Point(1573,1080);
  //    hull_warsaw_b.push_back(p);
  //    p = Point(1920,1080);
  //    hull_warsaw_b.push_back(p);
  //    //warsaw_h
  //    vector<Point>hull_warsaw_h;
  //    p = Point(1314,403);
  //    hull_warsaw_h.push_back(p);
  //    p = Point(901,253);
  //    hull_warsaw_h.push_back(p);
  //    p = Point(978,202);
  //    hull_warsaw_h.push_back(p);
  //    p = Point(1390,333);
  //    hull_warsaw_h.push_back(p);
  //    //warsaw_w
  //    vector<Point>hull_warsaw_w;
  //    p = Point(1558,393);
  //    hull_warsaw_w.push_back(p);
  //    p = Point(1491,466);
  //    hull_warsaw_w.push_back(p);
  //    p = Point(1920,680);
  //    hull_warsaw_w.push_back(p);
  //    p = Point(1920,550);
  //    hull_warsaw_w.push_back(p);
  //
  //
  //    startTime = clock();
  //
  //    auto values_warsaw_l = quadtree.query_time(hull_warsaw_l,fid_st,fid_ed);
  //    auto values_warsaw_r = quadtree.query_time(hull_warsaw_r,fid_st,fid_ed);
  //    auto values_warsaw_t= quadtree.query_time(hull_warsaw_t,fid_st,fid_ed);
  //    auto values_warsaw_b = quadtree.query_time(hull_warsaw_b,fid_st,fid_ed);
  //    auto values_warsaw_h = quadtree.query_time(hull_warsaw_h,fid_st,fid_ed);
  //    auto values_warsaw_w = quadtree.query_time(hull_warsaw_w,fid_st,fid_ed);
  //
  //
  //    unordered_map<int,int> track_time_warsaw_l;
  //    unordered_map<int,int> track_time_warsaw_t;
  //    unordered_map<int,int> track_time_warsaw_h;
  //
  //    unordered_set<int> track_warsaw_l_r;
  //    unordered_set<int> track_warsaw_t_b;
  //    unordered_set<int> track_warsaw_h_w;
  //
  //
  //    for(int i=0;i<values_warsaw_l.size();i++){
  //        track_time_warsaw_l[values_warsaw_l[i]->box.track_id]=values_warsaw_l[i]->box.frameIndex;
  //    }
  //
  //    for(int i=0; i < values_warsaw_r.size(); i++){
  //        if(track_time_warsaw_l[values_warsaw_r[i]->box.track_id]){
  //            if(values_warsaw_r[i]->box.frameIndex > track_time_warsaw_l[values_warsaw_r[i]->box.track_id]){
  //                track_warsaw_l_r.insert(values_warsaw_r[i]->box.track_id);
  //            }
  //        }
  //    }
  //
  //    for(int i=0;i<values_warsaw_t.size();i++){
  //        track_time_warsaw_t[values_warsaw_t[i]->box.track_id]=values_warsaw_t[i]->box.frameIndex;
  //    }
  //
  //    for(int i=0; i < values_warsaw_b.size(); i++){
  //        if(track_time_warsaw_t[values_warsaw_b[i]->box.track_id]){
  //            if(values_warsaw_b[i]->box.frameIndex > track_time_warsaw_t[values_warsaw_b[i]->box.track_id]){
  //                    track_warsaw_t_b.insert(values_warsaw_b[i]->box.track_id);
  //
  //            }
  //        }
  //    }
  //
  //    for(int i=0;i<values_warsaw_h.size();i++){
  //        track_time_warsaw_h[values_warsaw_h[i]->box.track_id]=values_warsaw_h[i]->box.frameIndex;
  //    }
  //
  //    for(int i=0; i < values_warsaw_w.size(); i++){
  //        if(track_time_warsaw_h[values_warsaw_w[i]->box.track_id]){
  //            if(values_warsaw_w[i]->box.frameIndex > track_time_warsaw_h[values_warsaw_w[i]->box.track_id]){
  //                    track_warsaw_h_w.insert(values_warsaw_w[i]->box.track_id);
  //            }
  //        }
  //    }
  //
  //    endTime = clock();
  //
  //    cout << "The warsaw time is: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s" << endl;
  ////    cout << " lr: " << track_warsaw_l_r.size() << endl;
  ////    cout << " tb: " << track_warsaw_t_b.size() << endl;
  ////    cout << " hw: " << track_warsaw_h_w.size();
  ////    cout << "1) warsawlr ： " << endl;
  ////    printTrackInf(warsaw_frameId_l_r);
  ////    cout << "2) warsawtb ： " << endl;
  ////    printTrackInf(warsaw_frameId_t_b);
  ////    cout << "3) warsawhw ： " << endl;
  ////    printTrackInf(warsaw_frameId_h_w);
  //




  return 0;
}


/*
//scan
int main() {

    // read json from disk, parse it and store into unorderedmap
    int sumTime = 0 , sumCarId = 0;
    JsonHelper jsonHelper;
    //jsonHelper.readSingleJson("/home/shuai/miris/data-splited/shibuya/2-baseline-6000.json");
    //jsonHelper.readSingleJson("/home/shuai/miris/data-splited/shibuya/2-baseline-12000.json");
    //jsonHelper.readSingleJson("/home/shuai/miris/data-splited/shibuya/2-baseline-18000.json");
    //jsonHelper.readSingleJson("/home/shuai/miris/data-splited/shibuya/2-baseline-24000.json");
    //jsonHelper.readSingleJson("/home/shuai/miris/data-splited/shibuya/2-baseline-30000.json");
    jsonHelper.readSingleJson("/home/shuai/miris/data-splited/shibuya/2-baseline-all.json");

    //jsonHelper.readSingleJson("/home/shuai/miris/data-splited/uav/0011-baseline-1000.json");
    //jsonHelper.readSingleJson("/home/shuai/miris/data-splited/uav/0011-baseline-2000.json");
    //jsonHelper.readSingleJson("/home/shuai/miris/data-splited/uav/0011-baseline-3000.json");
    //jsonHelper.readSingleJson("/home/shuai/miris/data-splited/uav/0011-baseline-4000.json");
    //jsonHelper.readSingleJson("/home/shuai/miris/data-splited/uav/0011-baseline-5000.json");
    //jsonHelper.readSingleJson("/home/shuai/miris/data-splited/uav/0011-baseline-6000.json");
    //jsonHelper.readSingleJson("/home/shuai/miris/data-splited/uav/0011-baseline-all.json");

    //jsonHelper.readSingleJson("/home/shuai/miris/data-splited/warsaw/2-baseline-6000.json");
    //jsonHelper.readSingleJson("/home/shuai/miris/data-splited/warsaw/2-baseline-12000.json");
    //jsonHelper.readSingleJson("/home/shuai/miris/data-splited/warsaw/2-baseline-18000.json");
    //jsonHelper.readSingleJson("/home/shuai/miris/data-splited/warsaw/2-baseline-24000.json");
    //jsonHelper.readSingleJson("/home/shuai/miris/data-splited/warsaw/2-baseline-30000.json");
    //jsonHelper.readSingleJson("/home/shuai/miris/data-splited/warsaw/2-baseline-all.json");

    vector<DetectorState> res;
    //将所有数据存到res中
    jsonHelper.parseJsonForMiris(res);

    int fid_st = 0;
    int fid_ed = 36000;



    int maxTime = 0;
    for(int i=0;i<res.size();i++){
        maxTime = max( maxTime , res[i].frameIndex );
    }

    int maxCarId = 0;
    for(int i=0;i<res.size();i++){
        maxCarId = max( maxCarId , res[i].track_id );
    }
    sumCarId =  sumCarId + maxCarId + 1;

    cout<<"帧的个数：   "<< maxTime+1 <<endl;

    cout<<"车的个数：   "<< sumCarId <<endl;
    int vec_len = res.size();
    cout<<" 轨迹点detection 个数   ： " << vec_len << endl;
    auto nodes = std::vector<Nodes>();
    for(int i=0;i<vec_len;i++){
        Nodes node;
        node.box.left = (res[i].left+res[i].right)/2;
        node.box.top =  (res[i].top+res[i].bottom)/2;
        node.box.width = 0;
        node.box.height = 0;
        node.box.track_id = res[i].track_id;
        node.box.frameIndex = res[i].frameIndex;
        nodes.push_back(node);
    }
    // cout<<" 1帧 几辆车 " << maxxx <<endl;
    clock_t startTime,endTime;

    startTime = clock();
    auto box = Box(0.0, 0.0, 2048.0, 2048.0);
    auto getBox = [](Nodes* node)
    {
        return node->box;
    };
    auto quadtree = Quadtree<Nodes*, decltype(getBox)>(box, getBox);
    for (auto& node : nodes){
        quadtree.add(&node);
    }



    endTime = clock();
    cout << "The 建树 time is: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s" << endl;

    quadtree.bianli();
    //A路口
    vector<Point>hull_A;
    Point p = Point(0.0,0.0);
    hull_A.push_back(p);
    p = Point(0.0,525.0);
    hull_A.push_back(p);
    p = Point(550.0,525.0);
    hull_A.push_back(p);
    p = Point(1200.0,420.0);
    hull_A.push_back(p);
    p = Point(1200.0,0.0);
    hull_A.push_back(p);

    //B
    vector<Point>hull_B;
    p = Point(678.0*2,209.0*2);
    hull_B.push_back(p);
    p = Point(795.0*2,148.0*2);
    hull_B.push_back(p);
    p = Point(906.0*2,182.0*2);
    hull_B.push_back(p);
    p = Point(843.0*2,251.0*2);
    hull_B.push_back(p);

    //C路口
    vector<Point>hull_C;
    p = Point(1640.0,630.0);
    hull_C.push_back(p);
    p = Point(1040.0,1080.0);
    hull_C.push_back(p);
    p = Point(1920.0,1080.0);
    hull_C.push_back(p);
    p = Point(1920.0,630.0);
    hull_C.push_back(p);

    //D
    vector<Point>hull_D;
    p = Point(10.0*2,312.0*2);
    hull_D.push_back(p);
    p = Point(193.0*2,300.0*2);
    hull_D.push_back(p);
    p = Point(428.0*2,539.0*2);
    hull_D.push_back(p);
    p = Point(10.0*2,539.0*2);
    hull_D.push_back(p);

    startTime = clock();

    /// 二分查找A C路口的车辆
    //auto values_a= quadtree.query_time(hull_A,fid_st,fid_ed);//(hull_A,31000,92999);
    //auto values_b = quadtree.query_time(hull_B,fid_st,fid_ed);
    //auto values_c= quadtree.query_time(hull_C, fid_st, fid_ed);
    //auto values_d = quadtree.query_time(hull_D,fid_st,fid_ed);

    Scan<Nodes> scan = Scan<Nodes>();
    auto values_a= scan.query_scan(nodes, hull_A,fid_st,fid_ed);//(hull_A,31000,92999);
    auto values_b = scan.query_scan(nodes,hull_B,fid_st,fid_ed);
    auto values_c= scan.query_scan(nodes,hull_C, fid_st, fid_ed);
    auto values_d = scan.query_scan(nodes,hull_D,fid_st,fid_ed);



    unordered_map<int,int> track_time;
    unordered_set<int> track_a_b;
    unordered_set<int> track_b_a;
    unordered_set<int> track_a_c;
    unordered_set<int> track_c_a;
    unordered_set<int> track_a_d;
    unordered_set<int> track_d_a;

    unordered_set<int> track_b_c;
    unordered_set<int> track_c_b;
    unordered_set<int> track_b_d;
    unordered_set<int> track_d_b;

    unordered_set<int> track_c_d;
    unordered_set<int> track_d_c;



    for(int i=0;i<values_a.size();i++){
        track_time[values_a[i].box.track_id]=values_a[i].box.frameIndex;
    }

    for(int i=0; i < values_b.size(); i++){
        if(track_time[values_b[i].box.track_id]){
            if(values_b[i].box.frameIndex > track_time[values_b[i].box.track_id]){
                track_a_b.insert(values_b[i].box.track_id);
            }else{
                track_b_a.insert(values_b[i].box.track_id);
            }
        }
    }

    for(int i=0; i < values_c.size(); i++){
        if(track_time[values_c[i].box.track_id]){
            if(values_c[i].box.frameIndex > track_time[values_c[i].box.track_id]){
                track_a_c.insert(values_c[i].box.track_id);
            }else{
                track_c_a.insert(values_c[i].box.track_id);
            }
        }
    }

    for(int i=0; i < values_d.size(); i++){
        if(track_time[values_d[i].box.track_id]){
            if(values_d[i].box.frameIndex > track_time[values_d[i].box.track_id]){
                track_a_d.insert(values_d[i].box.track_id);
            }else{
                track_d_a.insert(values_d[i].box.track_id);
            }
        }
    }

    for(int i=0;i<values_b.size();i++){
        track_time[values_b[i].box.track_id]=values_b[i].box.frameIndex;
    }

    for(int i=0; i < values_c.size(); i++){
        if(track_time[values_c[i].box.track_id]){
            if(values_c[i].box.frameIndex > track_time[values_c[i].box.track_id]){
                track_b_c.insert(values_c[i].box.track_id);
            }else{
                track_c_b.insert(values_c[i].box.track_id);
            }
        }
    }

    for(int i=0; i < values_d.size(); i++){
        if(track_time[values_d[i].box.track_id]){
            if(values_d[i].box.frameIndex > track_time[values_d[i].box.track_id]){
                track_b_d.insert(values_d[i].box.track_id);
            }else{
                track_d_b.insert(values_d[i].box.track_id);
            }
        }
    }
    for(int i=0;i<values_c.size();i++){
        track_time[values_c[i].box.track_id]=values_c[i].box.frameIndex;
    }

    for(int i=0; i < values_d.size(); i++){
        if(track_time[values_d[i].box.track_id]){
            if(values_d[i].box.frameIndex > track_time[values_d[i].box.track_id]){
                track_c_d.insert(values_d[i].box.track_id);
            }else{
                track_d_c.insert(values_d[i].box.track_id);
            }
        }
    }

    endTime = clock();

    cout << "The shibuya scan 二分查询转弯 time is: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s" << endl;

    cout << " A - C  的个数  ： " << track_a_c.size() << endl;
    int sum = 0;// 某一段时间段内的车辆detection个数
    for(int i=0;i<res.size();i++){
        if(res[i].frameIndex >= fid_st && res[i].frameIndex<=fid_ed){
            sum++;
        }
        if(res[i].frameIndex>fid_ed){
            break;
        }
    }

    cout<< "detection 的个数： "<<sum<<endl;

    // 查询指定区域内停留的车辆
    vector<Point>hull_S_1;
    p = Point(710.0*2,197.0*2);
    hull_S_1.push_back(p);
    p = Point(853.0*2,234.0*2);
    hull_S_1.push_back(p);
    p = Point(832.0*2,250.0*2);
    hull_S_1.push_back(p);
    p = Point(676.0*2,207.0*2);
    hull_S_1.push_back(p);

    vector<Point>hull_S_2;
    p = Point(838.0*2,364.0*2);
    hull_S_2.push_back(p);
    p = Point(960.0*2,420.0*2);
    hull_S_2.push_back(p);
    p = Point(960.0*2,540.0*2);
    hull_S_2.push_back(p);
    p = Point(590.0*2,540.0*2);
    hull_S_2.push_back(p);

    vector<Point>hull_S_3;
    p = Point(191.0*2,304.0*2);
    hull_S_3.push_back(p);
    p = Point(394.0*2,512.0*2);
    hull_S_3.push_back(p);
    p = Point(62.0*2,492.0*2);
    hull_S_3.push_back(p);
    p = Point(53.0*2,332.0*2);
    hull_S_3.push_back(p);

    vector<Point>hull_S_4;
    p = Point(258.0*2,231.0*2);
    hull_S_4.push_back(p);
    p = Point(533.0*2,184.0*2);
    hull_S_4.push_back(p);
    p = Point(598.0*2,209.0*2);
    hull_S_4.push_back(p);
    p = Point(272.0*2,260.0*2);
    hull_S_4.push_back(p);

    vector<Point>hull_S_5;
    p = Point(297.0*2,255.0*2);
    hull_S_5.push_back(p);
    p = Point(799.0*2,279.0*2);
    hull_S_5.push_back(p);
    p = Point(839.0*2,345.0*2);
    hull_S_5.push_back(p);
    p = Point(210.0*2,307.0*2);
    hull_S_5.push_back(p);

    startTime = clock();

    //auto values_a= scan.query_scan(nodes, hull_A,fid_st,fid_ed);
    //Scan<Nodes> scan = Scan<Nodes>();
    auto values_stay_1= scan.query_scan(nodes,hull_S_1, fid_st, fid_ed);
    auto values_stay_2= scan.query_scan(nodes,hull_S_2, fid_st, fid_ed);
    auto values_stay_3= scan.query_scan(nodes,hull_S_3, fid_st, fid_ed);
    auto values_stay_4= scan.query_scan(nodes,hull_S_4, fid_st, fid_ed);
    auto values_stay_5= scan.query_scan(nodes,hull_S_5, fid_st, fid_ed);

    //  quadtree.query_stop_pointer(hull_S_1,0,30999);

    //方法一： 即可转换成在这个区间内出现了多少次
    unordered_map<int,int> track_stay;
    for(int i=0; i < values_stay_1.size(); i++){
        track_stay[values_stay_1[i].box.track_id]++;
    }
    for(int i=0; i < values_stay_2.size(); i++){
        track_stay[values_stay_2[i].box.track_id]++;
    }
    for(int i=0; i < values_stay_3.size(); i++){
        track_stay[values_stay_3[i].box.track_id]++;
    }
    for(int i=0; i < values_stay_4.size(); i++){
        track_stay[values_stay_4[i].box.track_id]++;
    }
    for(int i=0; i < values_stay_5.size(); i++){
        track_stay[values_stay_5[i].box.track_id]++;
    }
    int weiguitingche = 0;
    for(auto it=track_stay.begin();it!=track_stay.end();it++){
        // cout<<it->first<<" 出现了几次 ： "<<track_stay[it->first]<<endl;
        if(track_stay[it->first]>=150){
            weiguitingche++;
        }
    }
    endTime = clock();
    cout << "The shibuya scan 停留 time is: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s" << endl;
    cout<<"违规停车的个数 ： " <<weiguitingche <<endl;

    startTime = clock();
    scan.query_scan(nodes,hull_S_1, fid_st, fid_ed);
    endTime = clock();
    cout << "The scan 时空查询 time is: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s" << endl;

    //方法二： 进入和离开这个区域的时间帧
    unordered_map<int,int> track_stay_st;
    unordered_map<int,int> track_stay_ed;
    for(int i=0; i < values_stay_1.size(); i++){
        if(track_stay_st[values_stay_1[i].box.track_id] == 0)
            track_stay_st[values_stay_1[i].box.track_id]=values_stay_1[i].box.frameIndex;
        else
            track_stay_st[values_stay_1[i].box.track_id]=min(track_stay_st[values_stay_1[i].box.track_id], values_stay_1[i].box.frameIndex);
        track_stay_ed[values_stay_1[i].box.track_id]=max(track_stay_ed[values_stay_1[i].box.track_id], values_stay_1[i].box.frameIndex);
    }

    // 查询闯红灯的车辆
    startTime = clock();
    //通行区
    vector<Point>hull_Red_Y;
    p = Point(0.0,0.0);
    hull_Red_Y.push_back(p);
    p = Point(0.0,525.0);
    hull_Red_Y.push_back(p);
    p = Point(550.0,525.0);
    hull_Red_Y.push_back(p);
    p = Point(1200.0,420.0);
    hull_Red_Y.push_back(p);
    p = Point(1200.0,0.0);
    hull_Red_Y.push_back(p);

    unordered_map<int,int> track_red_y;

    vector<Point>hull_Red_N;
    p = Point(550.0,525.0);
    hull_Red_N.push_back(p);
    p = Point(1040.0,1080.0);
    hull_Red_N.push_back(p);
    p = Point(1640.0,630.0);
    hull_Red_N.push_back(p);
    p = Point(1200.0,420.0);
    hull_Red_N.push_back(p);
    // unordered_map<int,int>track_red_n;

    int st,ed,fid_total;
    st=183;ed=1222;

    unordered_set<int> track_chuang;

    startTime = clock();
    for (int i = 0; ed + 1400*i < fid_ed; ++i) {
        auto values_Red_Y= scan.query_scan(nodes,hull_Red_Y,st+i*1400,ed+i*1400);
        for(int i=0;i<values_Red_Y.size();i++){
            track_red_y[values_Red_Y[i].box.track_id]=max(track_red_y[values_Red_Y[i].box.track_id],values_Red_Y[i].box.frameIndex);
        }

        auto values_Red_N= scan.query_scan(nodes,hull_Red_N,st+i*1400,ed+i*1400);


        for(int i=0;i<values_Red_N.size();i++){
            if(track_red_y[values_Red_N[i].box.track_id]!=0) {
                /// values_Red_N[i]->box.frameIndex<=ed && values_Red_N[i]->box.frameIndex>=st && track_red_y[values_Red_N[i]->box.track_id]<=ed && track_red_y[values_Red_N[i]->box.track_id]>=st &&
                if( values_Red_N[i].box.frameIndex>track_red_y[values_Red_N[i].box.track_id]){
                    track_chuang.insert(values_Red_N[i].box.track_id);
                }
            }
        }


    }






//    for(auto it=track_chuang.begin();it!=track_chuang.end();it++){
//        cout<< *it<<" 闯红灯"<<endl;
//    }

    endTime = clock();
    cout << "The 闯红灯 time is: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s" << endl;


    //uav
    //uav_A
    vector<Point>hull_uav_A;
    p = Point(181.0*2,223.0*2);
    hull_uav_A.push_back(p);
    p = Point(353.0*2,223.0*2);
    hull_uav_A.push_back(p);
    p = Point(353.0*2,540.0*2);
    hull_uav_A.push_back(p);
    p = Point(181.0*2,540.0*2);
    hull_uav_A.push_back(p);

    //uav_B
    vector<Point>hull_uav_B;
    p = Point(392.0*2,88.0*2);
    hull_uav_B.push_back(p);
    p = Point(960.0*2,88.0*2);
    hull_uav_B.push_back(p);
    p = Point(960.0*2,321.0*2);
    hull_uav_B.push_back(p);
    p = Point(392.0*2,321.0*2);
    hull_uav_B.push_back(p);

    startTime = clock();

    auto values_uav_a = scan.query_scan(nodes,hull_uav_A,fid_st,fid_ed);
    auto values_uav_b = scan.query_scan(nodes,hull_uav_B,fid_st,fid_ed);

    unordered_set<int> track_uav_a_b;
    unordered_set<int> track_uav_b_a;

    for(int i=0;i<values_uav_a.size();i++){
        track_time[values_uav_a[i].box.track_id]=values_uav_a[i].box.frameIndex;
    }

    for(int i=0; i < values_uav_b.size(); i++){
        if(track_time[values_uav_b[i].box.track_id]){
            if(values_uav_b[i].box.frameIndex > track_time[values_uav_b[i].box.track_id]){
                track_uav_a_b.insert(values_uav_b[i].box.track_id);
            }else{
                track_uav_b_a.insert(values_uav_b[i].box.track_id);
            }
        }
    }

    endTime = clock();

    cout << "The uav scan 转弯 time is: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s" << endl;

    //warsaw
    //warsaw_l
    vector<Point>hull_warsaw_l;
    p = Point(0,610);
    hull_warsaw_l.push_back(p);
    p = Point(930,610);
    hull_warsaw_l.push_back(p);
    p = Point(930,1080);
    hull_warsaw_l.push_back(p);
    p = Point(0,1080);
    hull_warsaw_l.push_back(p);

    //warsaw_r
    vector<Point>hull_warsaw_r;
    p = Point(1190,700);
    hull_warsaw_r.push_back(p);
    p = Point(1920,700);
    hull_warsaw_r.push_back(p);
    p = Point(1920,1080);
    hull_warsaw_r.push_back(p);
    p = Point(1190,1080);
    hull_warsaw_r.push_back(p);
    //warsaw_t
    vector<Point>hull_warsaw_t;
    p = Point(978,337);
    hull_warsaw_t.push_back(p);
    p = Point(1150,680);
    hull_warsaw_t.push_back(p);
    p = Point(1450,680);
    hull_warsaw_t.push_back(p);
    p = Point(1580,590);
    hull_warsaw_t.push_back(p);
    p = Point(1107,325);
    hull_warsaw_t.push_back(p);
    //warsaw_b
    vector<Point>hull_warsaw_b;
    p = Point(1920,685);
    hull_warsaw_b.push_back(p);
    p = Point(1645,669);
    hull_warsaw_b.push_back(p);
    p = Point(1400,780);
    hull_warsaw_b.push_back(p);
    p = Point(1573,1080);
    hull_warsaw_b.push_back(p);
    p = Point(1920,1080);
    hull_warsaw_b.push_back(p);
    //warsaw_h
    vector<Point>hull_warsaw_h;
    p = Point(1314,403);
    hull_warsaw_h.push_back(p);
    p = Point(901,253);
    hull_warsaw_h.push_back(p);
    p = Point(978,202);
    hull_warsaw_h.push_back(p);
    p = Point(1390,333);
    hull_warsaw_h.push_back(p);
    //warsaw_w
    vector<Point>hull_warsaw_w;
    p = Point(1558,393);
    hull_warsaw_w.push_back(p);
    p = Point(1491,466);
    hull_warsaw_w.push_back(p);
    p = Point(1920,680);
    hull_warsaw_w.push_back(p);
    p = Point(1920,550);
    hull_warsaw_w.push_back(p);


    startTime = clock();

    auto values_warsaw_l = scan.query_scan(nodes,hull_warsaw_l,fid_st,fid_ed);
    auto values_warsaw_r = scan.query_scan(nodes,hull_warsaw_r,fid_st,fid_ed);
    auto values_warsaw_t = scan.query_scan(nodes,hull_warsaw_t,fid_st,fid_ed);
    auto values_warsaw_b = scan.query_scan(nodes,hull_warsaw_b,fid_st,fid_ed);
    auto values_warsaw_h = scan.query_scan(nodes,hull_warsaw_h,fid_st,fid_ed);
    auto values_warsaw_w = scan.query_scan(nodes,hull_warsaw_w,fid_st,fid_ed);

    unordered_set<int> track_warsaw_l_r;
    unordered_set<int> track_warsaw_r_l;

    unordered_set<int> track_warsaw_t_b;
    unordered_set<int> track_warsaw_b_t;

    unordered_set<int> track_warsaw_h_w;
    unordered_set<int> track_warsaw_w_h;

    for(int i=0;i<values_warsaw_l.size();i++){
        track_time[values_warsaw_l[i].box.track_id]=values_warsaw_l[i].box.frameIndex;
    }

    for(int i=0; i < values_warsaw_r.size(); i++){
        if(track_time[values_warsaw_r[i].box.track_id]){
            if(values_warsaw_r[i].box.frameIndex > track_time[values_warsaw_r[i].box.track_id]){
                track_warsaw_l_r.insert(values_warsaw_r[i].box.track_id);
            }else{
                track_warsaw_r_l.insert(values_warsaw_r[i].box.track_id);
            }
        }
    }

    for(int i=0;i<values_warsaw_t.size();i++){
        track_time[values_warsaw_t[i].box.track_id]=values_warsaw_t[i].box.frameIndex;
    }

    for(int i=0; i < values_warsaw_b.size(); i++){
        if(track_time[values_warsaw_b[i].box.track_id]){
            if(values_warsaw_b[i].box.frameIndex > track_time[values_warsaw_b[i].box.track_id]){
                track_warsaw_t_b.insert(values_warsaw_b[i].box.track_id);
            }else{
                track_warsaw_b_t.insert(values_warsaw_b[i].box.track_id);
            }
        }
    }
    for(int i=0;i<values_warsaw_h.size();i++){
        track_time[values_warsaw_h[i].box.track_id]=values_warsaw_h[i].box.frameIndex;
    }

    for(int i=0; i < values_warsaw_w.size(); i++){
        if(track_time[values_warsaw_w[i].box.track_id]){
            if(values_warsaw_w[i].box.frameIndex > track_time[values_warsaw_w[i].box.track_id]){
                track_warsaw_h_w.insert(values_warsaw_w[i].box.track_id);
            }else{
                track_warsaw_w_h.insert(values_warsaw_w[i].box.track_id);
            }
        }
    }

    endTime = clock();

    cout << "The warsaw scan 转弯 time is: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s" << endl;

    return 0;
}
*/
