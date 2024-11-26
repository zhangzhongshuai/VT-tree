#include <iostream>
#include <cstring>
#include <unordered_set>
#include "DetectorState.h"
#include "JsonHelper.h"
#include "Box.h"
#include "Quadtree.h"
#include <time.h>
#include <chrono>
#include <thread>
#include "Scan.h"

#define TIMES 1

using namespace std;

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

  // key:track_id,valuelist：车辆轨迹
  map<int, vector<DetectorState>> tracks;
  //以检测框插入轨迹
  for (int i = 0; i < vec_len; i++) {
    Nodes node;
    node.box.left = res[i].left;
    node.box.top = res[i].top;
    node.box.width = res[i].right - res[i].left;
    node.box.height = res[i].bottom - res[i].top;
    node.box.track_id = res[i].track_id;
    node.box.frameIndex = res[i].frameIndex;
    nodes.push_back(node);
    for(int j = 1; j <= TIMES; ++j){
        tracks[res[i].track_id + (j - 1) * vec_len].push_back(DetectorState(res[i].frameIndex, res[i].left, res[i].top, res[i].right, res[i].bottom, res[i].track_id));
    }
    #ifdef DOUBLE
    tracks[res[i].track_id + vec_len].push_back(DetectorState(res[i].frameIndex, res[i].left, res[i].top, res[i].right, res[i].bottom, res[i].track_id));
    #endif
  }
  
  for (auto item : tracks)
  {
    vector<DetectorState>& tmp = item.second;
    sort(tmp.begin(), tmp.end(), [](DetectorState& lft, DetectorState& rft) {return lft.frameIndex < rft.frameIndex;});
  }


  clock_t startTime, endTime;
  startTime = clock();

  auto box = Box(0.0, 0.0, 2048.0, 2048.0);
  auto getBox = [](Nodes* node) {
    return node->box;
    };
  auto quadtree = Quadtree<Nodes*, decltype(getBox)>(box, getBox);
  // 建立一个满四叉树，默认层高为6 ，根节点是0层
  quadtree.makeFullTree();
  quadtree.buildTree(tracks); //扫描线，有重复计算



  endTime = clock();
  cout << "The 建树 time is: " << (double)(endTime - startTime) / CLOCKS_PER_SEC << "  s" << endl;

  //    遍历一下四叉树
  quadtree.bianli2();
  cerr<<cse::MODIFY_TIMES;


  startTime = clock();


    double findt = 0, cleart = 0;
    int cnt = 0;
#define MULTIQUERY
#ifdef MULTIQUERY
    int n;
    cin >> n;
    for(int i = 0; i < n; ++i){
        std::vector<Tracks> ans;
	Box queryBox((double)0, (double)0, (double)(rand() % 1000), (double)(rand() % 1000));
        auto currentTime = chrono::steady_clock::now();
        auto a = chrono::time_point_cast<chrono::microseconds>(currentTime).time_since_epoch().count();
        quadtree.queryT(queryBox, 1, rand() % 10000, ans);
        currentTime = chrono::steady_clock::now();
        auto b = chrono::time_point_cast<chrono::microseconds>(currentTime).time_since_epoch().count();
        for(Tracks x : ans){
            for(auto y : x.value){
                if(y.right <= queryBox.width){
                    ++cnt;
                    break;
                }
            }
        }
        currentTime = chrono::steady_clock::now();
        auto c = chrono::time_point_cast<chrono::microseconds>(currentTime).time_since_epoch().count();
        findt += (b - a);
        cleart += (c - b);
        // 查找的车id
        int trid = rand() % 10000;
        bool f = false;
        for(auto& x : ans){
            if(x.trackid == trid) {
                f = true;
                break;
            }
        }
    }
#else

    std::vector<Tracks> ans;
    quadtree.queryT(1, 10000, ans);
    cout << endl <<"查询结果“未去重”个数："<<ans.size() << endl;
#endif
    endTime = clock();
    cout << cnt << endl;
    cout << "findt = " << (double)findt / 1000000 << "s" << endl;
    cout << "cleart = " << (double)cleart / 1000000 << "s" << endl;
    cout << "The query time is: " << (double)(endTime - startTime) / CLOCKS_PER_SEC << "  s" << endl;
    
  return 0;
}