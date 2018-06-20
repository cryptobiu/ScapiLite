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

using namespace std;
using namespace boost;

void workerFunc()
{
    posix_time::seconds workTime(3);
    cout << "Worker: running" << std::endl;

    // Pretend to do something useful...
    boost::this_thread::sleep(workTime);
    cout << "Worker: finished" << std::endl;
}

extern "C"
JNIEXPORT jstring JNICALL
Java_crypto_cs_biu_scapilite_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */)
{
    string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}



extern "C"
JNIEXPORT jstring JNICALL
Java_crypto_cs_biu_scapilite_MainActivity_testLibs(
        JNIEnv *env,
        jobject /* this */)
{
    PrgFromAES prg;
    prg.generateKey(128);
    vector<byte> v(16);
    prg.getPRGBytes(v,0,16);
    string s="hello";
    mpz_t a;
    mpz_init_set_ui(a, 12345);
    char str[20];
    mpz_get_str(str,10, a);

    cout << "main: startup" << std::endl;
    boost::thread workerThread(workerFunc);

    cout << "main: waiting for thread" << std::endl;
    workerThread.join();
    cout << "main: done" << std::endl;

    NTL::ZZ b, c, d;
    b = 12;
    c = 12;
    d = (b+1)*(c+1);

    return env->NewStringUTF(str);
}

extern "C"
JNIEXPORT void JNICALL
Java_crypto_cs_biu_scapilite_ProtocolActivity_protocolMain(
        JNIEnv *env,
        jobject obj /* this */,
        jobject assetManager,
        jstring partyId,
        jstring filesPath)
{
    AAssetManager *assMgr = AAssetManager_fromJava(env, assetManager);


    char* argv[18];
    argv[0] = (char*)"PerfectSecureMPC";
    argv[1] = (char*)"circuitFile";
    argv[2] = (char*)"ArythmeticVarianceFor3InputsAnd3Parties.txt";
    argv[3] = (char*)"fieldType";
    argv[4] = (char*)"GF2_8LookupTable";
    argv[5] = (char*)"internalIterationsNumber";
    argv[6] = (char*)"1";
    argv[7] = (char*)"partyID";
    argv[8] = (char*)env->GetStringUTFChars(partyId, 0);
    argv[9] = (char*)"partiesNumber";
    argv[10] = (char*)"3";
    argv[11] = (char*)"partiesFile";
    argv[12] = (char*)"parties.conf";
    argv[13] = (char*)"inputFile";
    argv[14] = (char*)"inputsSalary0.txt";
    argv[15] = (char*)"outputFile";
    argv[16] = (char*)"output.txt";
    argv[17] = NULL;

    jboolean isCopy = (jboolean) false;
    const char * path = env->GetStringUTFChars(filesPath, &isCopy);

    AAsset* file = AAssetManager_open(assMgr, argv[12], AASSET_MODE_BUFFER);
    off_t fileLength = AAsset_getLength(file);
    char* fileContent = new char[fileLength+1];

    // Read your file
    AAsset_read(file, fileContent, (size_t)fileLength);
    fileContent[fileLength] = '\0';
    stringstream partiesData(fileContent);

    string proxySocket;
    int counter = 0;
    while(partiesData >> proxySocket)
    {
        if(counter == stoi(argv[8]))
            break;
        counter++;
    }

    size_t delimIdx = proxySocket.find(":");

    comm_client::cc_args_t cc_args;
    cc_args.logcat = "psmpc";
    cc_args.proxy_addr = proxySocket.substr(0, delimIdx);
    cc_args.proxy_port = (u_int16_t)
            stoi(proxySocket.substr(delimIdx + 1, proxySocket.size() - delimIdx));

    stringstream strValue1;
    strValue1 << partyId;

    int partyIdVal;

    stringstream strValue2;
    strValue2 << partyId;

    int partyNumsId;
    strValue2 >> partyNumsId;

    psmpc_ac_gf28lt ps(17, argv, &cc_args, env, assMgr, (char*)path);
    ps.run_ac_protocol((size_t)stoi(argv[8]), (size_t)stoi(argv[10]), argv[12], 180);
}

