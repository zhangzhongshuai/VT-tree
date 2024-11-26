//
// Created by qingyun on 1/22/21.
//

#ifndef TRANSVIDEO_DETECTORSTATE_H
#define TRANSVIDEO_DETECTORSTATE_H

#include <string>
#include <iostream>
using namespace std;
class DetectorState {
public:
  int frameIndex;
  int left;
  int top;
  int right;
  int bottom;
  int track_id;
  DetectorState(int frame, int l, int t, int r, int b, int track) :frameIndex(frame), left(l), top(t), right(r), bottom(b), track_id(track) {}
  DetectorState() = default;
  string toString();
  void print()
  {
    cout << "frameid:" << frameIndex << " trackid:" << track_id << " ";
  }
};


#endif //TRANSVIDEO_DETECTORSTATE_H
