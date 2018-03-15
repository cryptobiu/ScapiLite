//
// Created by liork on 11/03/18.
//

#ifndef SCAPILITEAPK_MEASUREMENT_H
#define SCAPILITEAPK_MEASUREMENT_H

#include <string>
#include <chrono>
#include <fstream>
#include <iostream>
#include <exception>
#include <memory>
#include <unistd.h>
#include <stdio.h>
#include <tuple>
#include <fstream>
#include <algorithm>
#include <iterator>
#include <sys/resource.h>
#include <sys/time.h>
#include "ConfigFile.h"
#include "json.h"
#include "Protocol.h"
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>


using namespace std;
using namespace std::chrono;
using json = nlohmann::json;

class Measurement {
public:
    Measurement(Protocol &protocol, JNIEnv *env, AAssetManager *assetManager);
    Measurement(Protocol &protocol, vector<string> names, JNIEnv *env, AAssetManager *assetManager);
    void setTaskNames(vector<string> & names);
    ~Measurement();
    void startSubTask(string taskName, int currentIterationNum);
    void endSubTask(string taskName, int currentIterationNum);


private:
    string getcwdStr()
    {
        char* buff;//automatically cleaned when it exits scope
        return string(getcwd(buff,255));
    }

    void init(Protocol &protocol, JNIEnv *env, AAssetManager *assetManager);
    void init(vector <string> names);
    int getTaskIdx(string name); // return the index of given task name
    void setCommInterface(string partiesFile, JNIEnv *env, AAssetManager *assetManager);

    tuple<unsigned long int, unsigned long int> commData(const char * nic_);
    void analyze(string type);
    void analyzeCpuData(); // create JSON file with cpu times
    void analyzeCommSentData(); // create JSON file with comm sent times
    void analyzeCommReceivedData(); // create JSON file with comm received times
    void analyzeMemory(); // create JSON file with memory usage
    void createJsonFile(json j, string fileName);

    vector<vector<long>> *m_cpuStartTimes;
    vector<vector<unsigned long int>> *m_commSentStartTimes;
    vector<vector<unsigned long int>> *m_commReceivedStartTimes;
    vector<vector<long>> *m_memoryUsage;
    vector<vector<long>> *m_cpuEndTimes;
    vector<vector<unsigned long int>> *m_commSentEndTimes;
    vector<vector<unsigned long int>> *m_commReceivedEndTimes;
    vector<string> m_names;
    vector<pair<string, string>> m_arguments;

    string m_protocolName;
    int m_partyId = 0;
    int m_numOfParties;
    int m_numberOfIterations;
    string m_interface; // states the network interface to listen too
};


#endif //SCAPILITEAPK_MEASUREMENT_H
