//
// Created by qingyun on 1/21/21.
//
#include <iostream>
#include <fstream>
#include "JsonHelper.h"
JsonHelper::JsonHelper() {

}


void JsonHelper::readSingleJson(string filename){
    ifstream ifs(filename);
    reader.parse(ifs, root);
}


void JsonHelper::parseJsonForMiris(vector<DetectorState> &vector){
    int frameSize = root.size();
    for(int i=0;i<frameSize;i++){
        int objectSize = root[i].size();
        for(int j=0;j<objectSize;j++) {
            Json::Value currObj = root[i][j];
            DetectorState currState;
            int track_id = currObj["track_id"].asInt();
            currState.frameIndex = currObj["frame_idx"].asInt();
            currState.left = currObj["left"].asInt();
            currState.top = currObj["top"].asInt();
            currState.right = currObj["right"].asInt();
            currState.bottom = currObj["bottom"].asInt();
            currState.track_id = track_id;
            vector.push_back(currState);
        }
    }
}

void JsonHelper::parseJsonForMiris2(vector<DetectorState> &vector ,int time ,int carId){
    int frameSize = root.size();
    for(int i=0;i<frameSize;i++){
        int objectSize = root[i].size();
        for(int j=0;j<objectSize;j++) {
            Json::Value currObj = root[i][j];
            DetectorState currState;
            int track_id = currObj["track_id"].asInt()+carId;
            currState.frameIndex = currObj["frame_idx"].asInt()+ time;
            currState.left = currObj["left"].asInt();
            currState.top = currObj["top"].asInt();
            currState.right = currObj["right"].asInt();
            currState.bottom = currObj["bottom"].asInt();
            currState.track_id = track_id;
            vector.push_back(currState);
        }
    }
}

//
void JsonHelper::writeFileJson(vector<DetectorState> tracks, string Path)
{
    //根节点
    Json::Value root;
    //根节点属性
    //输出到文件
    ofstream os;
    os.open(Path, std::ios::out | std::ios::app);
    if (!os.is_open())
        cout << "error：can not find or create the file which named \" final.json\"." << endl;
    os << "[[";
    for(int i=0;i<tracks.size();i++) {
        root["track_id"] = tracks[i].track_id;
        root["bottom"] = tracks[i].bottom;
        root["left"] = tracks[i].left;
        root["right"] = tracks[i].right;
        root["top"] = tracks[i].top;
        root["frame_id"] = tracks[i].frameIndex;


        //直接输出
        //cout << "FastWriter:" << endl;
        Json::FastWriter fw;
        //cout << fw.write(root) << endl << endl;

//    //缩进输出
//    cout << "StyledWriter:" << endl;
//    Json::StyledWiter sw;
//    cout << sw.write(root) << endl << endl;


        os << fw.write(root);
        if (i != tracks.size()-1)
            os << ",";
    }
    os << "]]";
    os.close();
}