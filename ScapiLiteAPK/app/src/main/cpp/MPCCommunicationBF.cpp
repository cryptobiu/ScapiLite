//
// Created by liork on 21/03/18.
//

#include "MPCCommunicationBF.h"


vector<shared_ptr<ProtocolPartyDataBF>>MPCCommunicationBF::setCommunication
        (int id, int numParties, string configFile, JNIEnv *env, AAssetManager *assetManager)
{
    __android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "Num of parties: %d", numParties);
    __android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "My id: %d", id);

    vector< shared_ptr<ProtocolPartyDataBF> > parties(numParties - 1);

    //open file
    ConfigFile cf(configFile, env, assetManager);

    string portString, ipString;
    vector<int> ports(numParties);
    vector< string > ips(numParties);

    int counter = 0;
    for (int i = 0; i < numParties; i++) {
        portString = "party_" + to_string(i) + "_port";
        ipString = "party_" + to_string(i) + "_ip";

        //get partys IPs and ports data
        ports[i] = stoi(cf.Value("", portString));
        ips[i] = cf.Value("", ipString);
    }

    for (int i=0; i<numParties; i++)
    {
        u_int16_t self_port = ports[id]+i, peer_port = ports[i]+id;
        if(i<id)
            peer_port -= 1;
        else if (i>id)
            self_port -= 1;
        else
            continue;
        __android_log_print(ANDROID_LOG_VERBOSE, APPNAME,
        "%d: self %s:%d <-> peer %s:%d", i, ips[id].c_str(), self_port, ips[i].c_str(), peer_port);
        shared_ptr<CommPartyBF> channel =
                make_shared<CommPartyTCPSyncedBoostFree>(ips[id].c_str(), self_port,
                                                         ips[i].c_str(), peer_port, false);
        channel->join();
        __android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "Channel established");
        parties[counter++] = make_shared<ProtocolPartyDataBF>(i, channel);
    }

    return parties;
}

