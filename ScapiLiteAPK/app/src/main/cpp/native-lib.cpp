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
JNIEXPORT jstring JNICALL
Java_crypto_cs_biu_scapilite_MainActivity_protocolMain(
        JNIEnv *env,
        jobject obj /* this */,
        jobject assetManager)
{
    AAssetManager *assMgr = AAssetManager_fromJava(env, assetManager);
    ConfigFile cf("parties.conf", env, assMgr);

    string portString, ipString;
    vector<int> ports(100);
    vector<string> ips(100);

    int counter = 0;
    for (int i = 0; i < 100; i++) {
        portString = "party_" + to_string(i) + "_port";
        ipString = "party_" + to_string(i) + "_ip";

        //get partys IPs and ports data
        ports[i] = stoi(cf.Value("", portString));
        ips[i] = cf.Value("", ipString);
    }

    char* argv[17];
    argv[0] = "programName";
    argv[1] = "Circuit";
    argv[2] = "c1";
    argv[3] = "field";
    argv[4] = "f1";
    argv[5] = "partyId";
    argv[6] = "0";
    argv[7] = "partiesNum";
    argv[8] = "3";
    argv[9] = "partiesFile";
    argv[10] = "pf.txt";
    argv[11] = "iteration";
    argv[12] = "4";
    argv[13] = "inputFile";
    argv[14] = "if.txt";
    argv[15] = "outputFile";
    argv[16] = "of.txt";

    ProtocolParty<GF2_8LookupTable> protocol(17, argv);

    return env->NewStringUTF(ips[0].c_str());
}

