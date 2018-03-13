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


    char* argv[17];
    argv[0] = "PerfectSecureMPC";
    argv[1] = "circuitFile";
    argv[2] = "1000000G_1000000MG_333In_50Out_20D_OutputOne3P.txt";
    argv[3] = "fieldType";
    argv[4] = "ZpMersenne";
    argv[5] = "internalIterationsNumber";
    argv[6] = "5";
    argv[7] = "partyID";
    argv[8] = "0";
    argv[9] = "partiesNumber";
    argv[10] = "3";
    argv[11] = "partiesFile";
    argv[12] = "parties.conf";
    argv[13] = "inputFile";
    argv[14] = "inputs333.txt";
    argv[15] = "outputFile";
    argv[16] = "output.txt";

    ProtocolParty<GF2_8LookupTable> protocol(17, argv, env, assMgr);

    return env->NewStringUTF("");
}

