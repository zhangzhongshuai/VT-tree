#include <iostream>
#include <cstring>
#include <unordered_set>
#include "DetectorState.h"
#include "JsonHelper.h"
#include "Box.h"
#include "Quadtree.h"
#include <time.h>
#include "Scan.h"

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
    tracks[res[i].track_id].push_back(DetectorState(res[i].frameIndex, res[i].left, res[i].top, res[i].right, res[i].bottom, res[i].track_id));
    #ifdef DOUBLE
    tracks[res[i].track_id + 46271].push_back(DetectorState(res[i].frameIndex+360078, res[i].left, res[i].top, res[i].right, res[i].bottom, res[i].track_id));
    #endif
  }
  for (auto item : tracks)
  {
    vector<DetectorState>& tmp = item.second;
    sort(tmp.begin(), tmp.end(), [](DetectorState& lft, DetectorState& rft) {return lft.frameIndex < rft.frameIndex;});
  }

    cin >> Alpha >> MaxDepth >> M;
  clock_t startTime, endTime;
  startTime = clock();

  auto box = Box(0.0, 0.0, 2048.0, 2048.0);
  auto getBox = [](Nodes* node) {
    return node->box;
    };
  auto quadtree = Quadtree<Nodes*, decltype(getBox)>(box, getBox);
  // 建立一个满四叉树，默认层高为6 ，根节点是0层
  quadtree.makeFullTree();
  // 将全部轨迹段插入四叉树
   quadtree.buildTreeCA(tracks);


  endTime = clock();
  cout << "The 建树 time is: " << (double)(endTime - startTime) / CLOCKS_PER_SEC << "  s" << endl;



  //    遍历一下四叉树
  quadtree.bianli2();


  startTime = clock();


#define MULTIQUERY
#ifdef MULTIQUERY
    int n;
    cin >> n;
    int xxxx = 0;
    for(int i = 0; i < n; ++i){
        std::vector<Tracks> ans;
        double pxx = (double)(rand() % 1000), pyy = (double)(rand() % 1000);
        double px = 100, py = 100;
	    Box queryBox(pxx, pyy, px, py);
        int t = 300000;
        quadtree.queryT(queryBox, 1, t, ans);
        int cnt = 0;
        cout << "ans:" << ans.size() << endl;
        for(auto& x : ans){
            
            for(int j = x.framestart; j <= x.frameend; ++j){
                ++cnt;
                ++xxxx;
            }
        }
        cout << cnt << endl;
    }
#else

    std::vector<Tracks> ans;
        quadtree.queryT(1, 1000, 1, 1000, 1, 10000, ans);
    cout << endl <<"查询结果“未去重”个数："<<ans.size() << endl;
#endif
    endTime = clock();
    cout << "The query time is: " << (double)(endTime - startTime) / CLOCKS_PER_SEC << "  s" << endl;

  return 0;
}
