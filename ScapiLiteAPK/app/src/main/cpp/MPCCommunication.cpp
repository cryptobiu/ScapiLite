//
// Created by liork on 11/03/18.
//

#include "MPCCommunication.h"

vector<shared_ptr<ProtocolPartyData>> MPCCommunication::setCommunication
        (io_service & io_service, int id, int numParties, string configFile,
         JNIEnv *env, AAssetManager *assetManager){

    __android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "Num of parties: %d", numParties);
    __android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "My id: %d", id);

    vector<shared_ptr<ProtocolPartyData>> parties(numParties - 1);

    //open file
    ConfigFile cf(configFile, env, assetManager);

    string portString, ipString;
    vector<int> ports(numParties);
    vector<string> ips(numParties);

    int counter = 0;
    for (int i = 0; i < numParties; i++) {
        portString = "party_" + to_string(i) + "_port";
        ipString = "party_" + to_string(i) + "_ip";

        //get partys IPs and ports data
        ports[i] = stoi(cf.Value("", portString));
        ips[i] = cf.Value("", ipString);
    }

    SocketPartyData me, other;

    for (int i=0; i<numParties; i++){
        if (i < id) {// This party will be the receiver in the protocol

            me = SocketPartyData(boost_ip::address::from_string(ips[id]), ports[id] + i);
            __android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "my port %d", ports[id] + i);
            other = SocketPartyData(boost_ip::address::from_string(ips[i]), ports[i] + id - 1);
            __android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "other port %d", ports[i] + id - 1);

            shared_ptr<CommParty> channel = make_shared<CommPartyTCPSynced>(io_service, me, other);
            // connect to party one
            channel->join(500, 5000);
            __android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "channel established");

            parties[counter++] = make_shared<ProtocolPartyData>(i, channel);
        }
        else if (i>id) {// This party will be the sender in the protocol
            me = SocketPartyData(boost_ip::address::from_string(ips[id]), ports[id] + i - 1);
            __android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "my port %d", ports[id] + i - 1);
            other = SocketPartyData(boost_ip::address::from_string(ips[i]), ports[i] + id);
            __android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "other port %d", ports[i] + id);

            shared_ptr<CommParty> channel = make_shared<CommPartyTCPSynced>(io_service, me, other);
            // connect to party one
            channel->join(500, 5000);
            __android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "channel established");

            parties[counter++] = make_shared<ProtocolPartyData>(i, channel);
        }
    }

    return parties;

}

