//
// Created by liork on 11/03/18.
//

#ifndef SCAPILITEAPK_MPCCOMMUNICATION_H
#define SCAPILITEAPK_MPCCOMMUNICATION_H


#include "Comm.h"

using namespace boost::asio;

class ProtocolPartyData {
private:
    int id;
    shared_ptr<CommParty> channel;  // Channel between this party to every other party in the protocol.

public:
    ProtocolPartyData() {}
    ProtocolPartyData(int id, shared_ptr<CommParty> channel)
            : id (id), channel(channel){
    }

    int getID() { return id; }
    shared_ptr<CommParty> getChannel() { return channel; }
};

class MPCCommunication {

public:
    static vector<shared_ptr<ProtocolPartyData>>
    setCommunication(io_service & io_service, int id, int numParties, string configFile,
                     JNIEnv *env, AAssetManager *assetManager);
};


#endif //SCAPILITEAPK_MPCCOMMUNICATION_H
