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
#include "comm_client.h"
#include "comm_client_factory.h"

using namespace std;
using namespace boost;

void workerFunc() {
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
        jobject /* this */) {
    string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}

extern "C"
JNIEXPORT jstring JNICALL
Java_crypto_cs_biu_scapilite_ProtocolActivity_protocolMain(
        JNIEnv *env,
        jobject obj /* this */,
        jobject assetManager,
        jstring partyId,
        jstring partiesNumber,
        jstring inputVal,
        jstring outputFile,
        jstring circuitFile,
        jstring fieldType,
        jstring internalIterationsNumber,
        jstring NG) {
    AAssetManager *assMgr = AAssetManager_fromJava(env, assetManager);

    char *argv[20];

    argv[0] = (char *) "PerfectSecureMPC";
    argv[1] = (char *) "circuitFile";
    argv[2] = (char *) env->GetStringUTFChars(circuitFile, 0);
    argv[3] = (char *) "fieldType";
    argv[4] = (char *) env->GetStringUTFChars(fieldType, 0);
    argv[5] = (char *) "internalIterationsNumber";
    argv[6] = (char *) env->GetStringUTFChars(internalIterationsNumber, 0);
    argv[7] = (char *) "partyID";
    argv[8] = (char *) env->GetStringUTFChars(partyId, 0);
    argv[9] = (char *) "partiesNumber";
    argv[10] = (char *) env->GetStringUTFChars(partiesNumber, 0);
    argv[11] = (char *) "partiesFile";
    argv[12] = (char *) "parties.conf";
    argv[13] = (char *) "userInput";
    argv[14] = (char *) env->GetStringUTFChars(inputVal, 0);
    argv[15] = (char *) "outputFile";
    argv[16] = (char *) env->GetStringUTFChars(outputFile, 0);
    argv[17] = (char *) "NG";
    argv[18] = (char *) env->GetStringUTFChars(NG, 0);
    argv[19] = NULL;

    ProtocolParty<ZpMersenneIntElement> protocol(20, argv, env, assMgr);
    string output = protocol.run();
    return env->NewStringUTF(output.c_str());
}
