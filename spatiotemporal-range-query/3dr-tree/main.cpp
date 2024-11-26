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
#include "Scan.h"

using namespace std;
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
    if (track_stay[it->first] > 150) {
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

 vector <DetectorState> res;
  vector <DetectorState> ans;
  int pre_frame = 0, pre_trackid = 0;
  ans.clear();


  int pre = res.size();

  int xx = pre_frame, yy = pre_trackid;
  

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

  clock_t startTime, endTime;
  startTime = clock();
  int maxLeft = 0, maxTop = 0;
  for (int i = 0; i < vec_len; i++) {
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


  endTime = clock();
  cout << "The 建树 time is: " << (double)(endTime - startTime) / CLOCKS_PER_SEC << "  s" << endl;

  startTime = clock();


    double findt = 0, cleart = 0;
#define MULTIQUERY
#ifdef MULTIQUERY
    int n;
    cin >> n;
    int cnt = 0;
    for(int i = 0; i < n; ++i){
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
  return 0;
}