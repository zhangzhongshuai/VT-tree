#include <iostream>
#include <cstring>
#include <unordered_set>
#include "DetectorState.h"
#include "JsonHelper.h"
#include "Box.h"
#include "Quadtree.h"
#include "rtree.h"
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
struct Track{
    vector<DetectorState> track;
    int id;
    Rectangle MBR;
    operator Rectangle() const {
        return MBR;
    }
    bool isOverlap(const Rectangle & other){
        bool flag = false;
        for(DetectorState x : track){
            flag |= !(x.right < other.minX || x.left > other.maxX || x.bottom < other.minY || x.top > other.maxY);
        }
        return flag;
    }
};
Rtree<Track> *root[5000000];
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
    
    memset(root, 0, sizeof(root));
    Rtree<Track> *tree = new Rtree<Track>;
    sort(res.begin(), res.end(), [&](DetectorState a, DetectorState b){
        return a.track_id < b.track_id;
    }); //按照id排序
  for (int i = 0; i < vec_len;){
        int frameIndex = res[i].frameIndex;
      Track rec;
        rec.MBR.maxX = res[i].left;
        rec.MBR.minY = res[i].top;
        rec.MBR.maxX = res[i].right;
        rec.MBR.maxY = res[i].bottom;
    int id = res[i].track_id;
    rec.id = id;
    while(i < vec_len && res[i].track_id == id){
        rec.MBR.minX = min(rec.MBR.minX, res[i].left);
        rec.MBR.minY = min(rec.MBR.minY, res[i].top);
        rec.MBR.maxX = max(rec.MBR.maxX, res[i].right);
        rec.MBR.maxY = max(rec.MBR.maxY, res[i].bottom);
        rec.track.push_back(res[i]);
        ++i;
    }
    #ifdef DOUBLE
    for(int j = 0; j < 2; ++j){
      tree = tree->insert(rec);
      root[res[i].frameIndex] = tree;
    }
    #else
    for(int j = 0; j < TIMES; ++j){
      tree = tree->insert(rec);
    //   cerr << res[i].frameIndex << " ";
      root[frameIndex] = tree;
    }
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
	//cout  << "now:"<< i << endl;
        auto currentTime = chrono::steady_clock::now();
        auto a = chrono::time_point_cast<chrono::microseconds>(currentTime).time_since_epoch().count();
        std::vector<Track> ans;
        Rectangle rec(0, 0, rand() % 2000, rand() % 2000);
	    int s = rand() % 10000, t = s + 10000;
	    ++TEST_TIME;
        for(int i = 0; i <= 1000; ++i){
            if(root[i]){
                tree->query(rec, ans);
            }
        }
        for(int i = 2000; i <= 3000; ++i){
            if(root[i]){
                tree->query(rec, ans);
            }
        }
        for(int i = 4000; i <= 5000; ++i){
            if(root[i]){
                tree->query(rec, ans);
            }
        }
        for(int i = 6000; i <= 7000; ++i){
            if(root[i]){
                tree->query(rec, ans);
            }
        }
        for(int i = 8000; i <= 9000; ++i){
            if(root[i]){
                tree->query(rec, ans);
            }
        }

        currentTime = chrono::steady_clock::now();
        auto b = chrono::time_point_cast<chrono::microseconds>(currentTime).time_since_epoch().count();
        for(Track x : ans){
            cnt += x.isOverlap(rec);
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
    cout << "The query time is: " << (double)(endTime - startTime) / CLOCKS_PER_SEC << "  s" << endl;
  return 0;
}