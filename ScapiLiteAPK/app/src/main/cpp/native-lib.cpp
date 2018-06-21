#include <jni.h>
#include <string>
#include <sstream>
#include <iomanip>
#include "Prg.h"
#include "gmp.h"
#include <boost/thread.hpp>
#include <boost/date_time.hpp>
#include <NTL/ZZ.h>
#include "ConfigFile.h"
#include "GF2_8LookupTable.h"
#include "ProtocolParty.h"
#include "psmpc_ac_gf28lt.h"
#include "comm_client.h"

extern "C"
JNIEXPORT void JNICALL
Java_crypto_cs_biu_scapilite_ProtocolActivity_protocolMain(
        JNIEnv *env,
        jobject obj /* this */,
        jobject assetManager,
        jstring partyId, jstring partiesNumber,
        jstring inputFile, jstring outputFile, jstring circuitFile,
        jstring proxyAddress, jstring fieldType,
        jstring internalIterationsNumber, jstring NG, jstring filesPath)
{
    AAssetManager *assMgr = AAssetManager_fromJava(env, assetManager);

    char* argv[20];

    argv[0] = (char*)"PerfectSecureMPC";
    argv[1] = (char*)"circuitFile";
    argv[2] = (char*)env->GetStringUTFChars(circuitFile, 0);
    argv[3] = (char*)"fieldType";
    argv[4] = (char*)env->GetStringUTFChars(fieldType, 0);
    argv[5] = (char*)"internalIterationsNumber";
    argv[6] = (char*)env->GetStringUTFChars(internalIterationsNumber, 0);
    argv[7] = (char*)"partyID";
    argv[8] = (char*)env->GetStringUTFChars(partyId, 0);
    argv[9] = (char*)"partiesNumber";
    argv[10] = (char*)env->GetStringUTFChars(partiesNumber, 0);
    argv[11] = (char*)"partiesFile";
    argv[12] = (char*)"parties.conf";
    argv[13] = (char*)"inputFile";
    argv[14] = (char*)env->GetStringUTFChars(inputFile, 0);
    argv[15] = (char*)"outputFile";
    argv[16] = (char*)env->GetStringUTFChars(outputFile, 0);
    argv[15] = (char*)"NG";
    argv[16] = (char*)env->GetStringUTFChars(NG, 0);
    argv[17] = NULL;

    jboolean isCopy = (jboolean) false;
    const char * path = env->GetStringUTFChars(filesPath, &isCopy);

    AAsset* file = AAssetManager_open(assMgr, argv[12], AASSET_MODE_BUFFER);
    off_t fileLength = AAsset_getLength(file);
    char* fileContent = new char[fileLength+1];



    comm_client::cc_args_t cc_args;
    cc_args.logcat = "psmpc";
    cc_args.proxy_addr = "34.239.19.87";
    cc_args.proxy_port = (u_int16_t) 9000 + stoi(argv[8]);

    psmpc_ac_gf28lt ps(17, argv, &cc_args, env, assMgr, (char*)path);
    ps.run_ac_protocol((size_t)stoi(argv[8]), (size_t)stoi(argv[10]), argv[12], 180);
}

