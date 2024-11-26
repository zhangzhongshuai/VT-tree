//
// Created by qingyun on 1/21/21.
//

#ifndef TRANSVIDEO_JSONHELPER_H
#define TRANSVIDEO_JSONHELPER_H
#include <string>
#include <vector>
#include "json/json.h"
#include <unordered_map>
#include "DetectorState.h"

using namespace  std;

class JsonHelper {
public:
    JsonHelper();
    void readSingleJson(string filename);
    Json::Reader reader;
    Json::Value root;
    void parseJsonForMiris(vector<DetectorState> &vector);
    void parseJsonForMiris2(vector<DetectorState> &vector,int time ,int carId);

    //reate by liujiaxiao
    void writeFileJson(vector<DetectorState> tracks, string Path);
};


#endif //TRANSVIDEO_JSONHELPER_H
