//
// Created by liork on 21/03/18.
//

#ifndef SCAPILITEAPK_MPCCOMMUNICATIONBF_H
#define SCAPILITEAPK_MPCCOMMUNICATIONBF_H

#include <memory>
#include <iostream>
#include "CommBF.h"
#include "Common.h"
#include "ConfigFile.h"

using namespace std;

class ProtocolPartyDataBF {
private:
    int id;
    std::shared_ptr<CommPartyBF> channel;  // Channel between this party to every other party in the protocol.

public:
    ProtocolPartyDataBF() {}
    ProtocolPartyDataBF(int id, std::shared_ptr<CommPartyBF> channel)
            : id (id), channel(channel){
    }

    int getID() { return id; }
    std::shared_ptr<CommPartyBF> getChannel() { return channel; }
};

class MPCCommunicationBF {

public:
    static vector<shared_ptr<ProtocolPartyDataBF>>
    setCommunication(int id, int numParties, string configFile,
                     JNIEnv *env, AAssetManager *assetManager);
};




#endif //SCAPILITEAPK_MPCCOMMUNICATIONBF_H
