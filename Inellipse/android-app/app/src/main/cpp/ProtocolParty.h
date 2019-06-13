//
// Created by liork on 12/03/18.
//

#ifndef SCAPILITEAPK_PROTOCOLPARTY_H
#define SCAPILITEAPK_PROTOCOLPARTY_H


#include <stdlib.h>
#include "Matrix.h"
#include "ArithmeticCircuit.h"
#include <vector>
#include <iostream>
#include <fstream>
#include <chrono>
#include "TemplateField.h"
#include "MPCCommunication.h"
#include "Common.h"
#include "Measurement.h"
#include <thread>
#include "Protocol.h"
#include "SecurityLevel.h"

#define flag_print_output false

using namespace std;
using namespace std::chrono;


template <class FieldType>
class ProtocolParty : public Protocol, public HonestMajority, public MultiParty {
protected:
    /**
     * N - number of parties
     * M - number of gates
     * T - number of malicious
     */
    int currentCirciutLayer = 0;
    int times; //number of times to run the run function
    int iteration; //number of the current iteration

    int N, M, T, m_partyId;
    int numOfInputGates, numOfOutputGates;
    string inputsFile;
    vector<FieldType> beta;
    HIM<FieldType> matrix_for_interpolate;
    HIM<FieldType> matrix_for_t;
    HIM<FieldType> matrix_for_2t;
    TemplateField<FieldType> *field;

    HIM<FieldType> matrix_him;
    VDM<FieldType> matrix_vand;
    HIM<FieldType> m;

    boost::asio::io_service io_service;
    vector<shared_ptr<ProtocolPartyData>>  parties;

    ArithmeticCircuit circuit;
    vector<FieldType> gateValueArr; // the value of the gate (for my input and output gates)
    vector<FieldType> gateShareArr; // my share of the gate (for all gates)
    vector<FieldType> alpha; // N distinct non-zero field elements

    vector<FieldType> sharingBufTElements; // prepared T-sharings (my shares)
    vector<FieldType> sharingBuf2TElements; // prepared 2T-sharings (my shares)
    vector<FieldType> sharingBufInputsTElements; // prepared T-sharings (my shares)
    int shareIndex;

    vector<int> myInputs;
    string s;

    virtual void do_send_and_recv(const vector< vector< byte > > & _2send, vector< vector< byte > > & _2recv);

public:
    ProtocolParty(int argc, char* argv [], JNIEnv *env, AAssetManager *assetManager, bool commOn=true);


    void roundFunctionSync(const vector<vector<byte>> &sendBufs, vector<vector<byte>> &recBufs, int round);
    void exchangeData(const vector<vector<byte>> &sendBufs,vector<vector<byte>> &recBufs, int first, int last);
    void roundFunctionSyncBroadcast(vector<byte> &message, vector<vector<byte>> &recBufs);
    void recData(vector<byte> &message, vector<vector<byte>> &recBufs, int first, int last);



    /**
     * This method runs the protocol:
     * Preparation Phase
     * Input Phase
     * Computation Phase
     * Output Phase
     */
    string run() override;

    bool hasOffline() {
        return true;
    }


    bool hasOnline() override {
        return true;
    }

    /**
     * This method runs the protocol:
     * Preparation Phase
     */
    void runOffline() override;

    /**
     * This method runs the protocol:
     * Input Phase
     * Computation Phase
     * Verification Phase
     * Output Phase
     */
    string runOnline() override;

    /**
     * This method reads text file and inits a vector of Inputs according to the file.
     */
    void readMyInputs();

    /**
     * We describe the protocol initialization.
     * In particular, some global variables are declared and initialized.
     */
    void initializationPhase(/*HIM<FieldType> &matrix_him, VDM<FieldType> &matrix_vand, HIM<FieldType> &m*/);

    /**
     * A random double-sharing is a pair of two sharings of the same random value, where the one sharing is
     * of degree t, and the other sharing is of degree 2t. Such random double-sharing are of big help in the
     * multiplication protocol.
     * We use hyper-invertible matrices to generate random double-sharings. The basic idea is as follows:
     * Every party generates one random double-sharing. These n double-sharings are processes through a
     * hyper-invertible matrix. From the resulting n double-sharings, t are checked to be valid (correct degree,
     * same secret), and t are then kept as “good” double-sharings. This is secure due to the diversion property
     * of hyper-invertible matrix: We know that n − t of the input double-sharings are good. So, if there are t
     * valid output double-sharings, then all double-sharings must be valid. Furthermore, the adversary knows
     * his own up to t input double-sharings, and learns t output double sharings. So, n − 2t output double
     * sharings are random and unknown to the adversary.
     * For the sake of efficiency, we do not publicly reconstruct t of the output double-sharings. Rather, we
     * reconstruct 2t output double sharings, each to one dedicated party only. At least t of these parties are
     * honest and correctly validate the reconstructed double-sharing.
     *
     * The goal of this phase is to generate “enough” double-sharings to evaluate the circuit. The double-
     * sharings are stored in a buffer SharingBuf , where alternating a degree-t and a degree-2t sharing (of the same secret)
     * is stored (more precisely, a share of each such corresponding sharings is stored).
     * The creation of double-sharings is:
     *
     * Protocol Generate-Double-Sharings:
     * 1. ∀i: Pi selects random value x-(i) and computes degree-t shares x1-(i) and degree-2t shares x2-(i).
     * 2. ∀i,j: Pi sends the shares x1,j and X2,j to party Pj.
     * 3. ∀j: Pj applies a hyper-invertible matrix M on the received shares, i.e:
     *      (y1,j,..., y1,j) = M(x1,j,...,x1,j)
     *      (y2,j,...,y2,j) = M (x2,j,...,x2,)
     * 4. ∀j, ∀k ≤ 2t: Pj sends y1,j and y2,j to Pk.
     * 5. ∀k ≤ 2t: Pk checks:
     *      • that the received shares (y1,1,...,y1,n) are t-consistent,
     *      • that the received shares (y2,1,...,y2,n) are 2t-consistent, and
     *      • that both sharings interpolate to the same secret.
     *
     * We use this algorithm, but extend it to capture an arbitrary number of double-sharings.
     * This is, as usual, achieved by processing multiple buckets in parallel.
     */
    bool preparationPhase();
    bool RandomSharingForInputs();

    /**
     * We do not need robust broadcast (which we require an involved and expensive sub-protocol).
     * As we allow abort, broadcast can be achieved quite easily.
     * Note that the trivial approach (distribute value, one round of echoing) requires quadratic communication,
     * which is too much.
     * Goal: One party Ps wants to broadcast n−t values x = (x1,...,xn−t).
     *
     * Protocol Broadcast:
     * 0. Ps holds input vector x = (x1,...,xn−t).
     * 1. ∀j: Ps sends x to Pj. Denote the received vector as x(j) (P j -th view on the sent vector).
     * 2. ∀j: Pj applies hyper-invertible matrix M : (y1(j),...,yn(j))= M*(x1(j),..,xn−t(j),0,...,0).
     * 3. ∀j,k: Pj sends yk to Pk.
     * 4. ∀k: Pk checks whether all received values {yk(j)}j are equal. If so, be happy, otherwise cry.
     */
    bool broadcast(int party_id, vector<byte> myMessage, vector<vector<byte>> &recBufsdiffBytes, HIM<FieldType> &mat);

    /**
     * For multiplication and for output gates, we need public reconstruction of sharings (degree t and degree 2t).
     * The straight-forward protocol requires n^2 communication, which is too slow.
     * We present a protocol which efficiently reconstructs n − t sharings. The basic idea is to compute t shared
     * authentication checks and to reconstruct the n sharings, one towards each party, who then computes the
     * secret and sends it to everybody. Each party receives n − t secrets and t authentication checks.
     *
     * Protocol Public-Reconstruction:
     * 0. Every party Pi holds a vector x(i) of degree-d shares of n−t secret values x. Let M be a t-by-(n−t)
     *  hyper-invertible matrix.
     * 1. ∀i: Compute y(i) = M*x(i) and append it to x(i). Note that this is now a vector of length n.
     * 2. ∀i,j: Pi sends xj to Pj.
     * 3. ∀j: Pj checks that {xj}i are d-consistent (otherwise cry), and interpolate them to xj.
     * 4. ∀j,i: Pj sends xj to Pi.
     * 5. ∀i: Pi checks that (xn−t,...,xn) = M*(x1,...,xn−t), otherwise cry.
     *
     * However, in our protocol, arbitrary many sharings can be reconstructed.
     * This is achieved by dividing the sharings into buckets of size n − t.
     */
    void publicReconstruction(vector<FieldType> &myShares, int &count, int d, vector<FieldType> &valBuf, HIM<FieldType> &m);

    /**
     * The input phase proceeds in two steps: input preparation and input adjustment
     * First, input preparation -
     *      for each input gate, a prepared t-sharings is reconstructed towards the party giving input.
     * Then, input adjustment -
     *      the party broadcasts for each input gate the difference between the random secret and the actual input value.
     *
     * Note that the first step can still be performed in to offline phase.
     */
    bool inputPreparation();

    /**
      * The input phase proceeds in two steps: input preparation and input adjustment
      * First, input preparation -
      *      for each input gate, a prepared t-sharings is reconstructed towards the party giving input.
      * Then, input adjustment -
      *      the party broadcasts for each input gate the difference between the random secret and the actual input value.
      *
      * Note that the first step can still be performed in to offline phase.
      */
    void inputAdjustment(string &diff/*, HIM<FieldType> &mat*/);

    /**
     * Check whether given points lie on polynomial of degree d.
     * This check is performed by interpolating x on the first d + 1 positions of α and check the remaining positions.
     */
    bool checkConsistency(vector<FieldType>& x, int d);

    /**
     * Process all multiplications which are ready.
     * Return number of processed gates.
     */
    int processMultiplications(HIM<FieldType> &m);

    int processNotMult();

    /**
     * Walk through the circuit and evaluate the gates. Always take as many gates at once as possible,
     * i.e., all gates whose inputs are ready.
     * We first process all random gates, then alternately process addition and multiplication gates.
     */
    void computationPhase(HIM<FieldType> &m);

    /**
     * The cheap way: Create a HIM from the αi’s onto ZERO (this is actually a row vector), and multiply
     * this HIM with the given x-vector (this is actually a scalar product).
     * The first (and only) element of the output vector is the secret.
     */
    FieldType interpolate(vector<FieldType> x);

    /**
     * Walk through the circuit and reconstruct output gates.
     */
    string outputPhase();

    ~ProtocolParty();
};


template <class FieldType>
ProtocolParty<FieldType>::ProtocolParty(int argc, char* argv [],
                                        JNIEnv *env, AAssetManager *assetManager, bool commOn)
        : Protocol ("PerfectSecureMPC", argc, argv)
{

    string circuitFile = this->getParser().getValueByKey(arguments, "circuitFile");
    this->times = stoi(this->getParser().getValueByKey(arguments, "internalIterationsNumber"));
    string fieldType = this->getParser().getValueByKey(arguments, "fieldType");
    m_partyId = stoi(this->getParser().getValueByKey(arguments, "partyID"));
    int n = stoi(this->getParser().getValueByKey(arguments, "partiesNumber"));
    string partiesFileName = this->getParser().getValueByKey(arguments, "partiesFile");

    if(fieldType.compare("ZpMersenne") == 0) {
        field = new TemplateField<FieldType>(2147483647);
    } else if(fieldType.compare("ZpMersenne61") == 0) {
        field = new TemplateField<FieldType>(0);
    } else if(fieldType.compare("GF2_8LookupTable") == 0) {
        field = new TemplateField<FieldType>(0);
    } else if(fieldType.compare("GF2E") == 0) {
        field = new TemplateField<FieldType>(8);
    } else if(fieldType.compare("Zp") == 0) {
        field = new TemplateField<FieldType>(2147483647);
    }

    N = n;
    T = n/3 - 1;
    this->inputsFile = this->getParser().getValueByKey(arguments, "userInput");
    if(n%3 > 0)
    {
        T++;
    }

    s = to_string(m_partyId);
    circuit.readCircuit(circuitFile.c_str(), env, assetManager);
    circuit.reArrangeCircuit();
    M = circuit.getNrOfGates();
    numOfInputGates = circuit.getNrOfInputGates();
    numOfOutputGates = circuit.getNrOfOutputGates();
    myInputs.resize(numOfInputGates);
    shareIndex = 0;//numOfInputGates;

    readMyInputs();

    auto t1 = high_resolution_clock::now();
    initializationPhase(/*matrix_him, matrix_vand, m*/);

    auto t2 = high_resolution_clock::now();

    auto duration = duration_cast<milliseconds>(t2-t1).count();
}

/**
 * Protocol Broadcast:
 *  0. Ps holds input vector x = (X1,...,Xn−t).
 *  1. ∀j: Ps sends x to Pj . Denote the received vector as x (j) (P j -th view on the sent vector).
 *  2. ∀j: Pj applies hyper-invertible matrix M and calculate (y1,...,yn) = M(X1,...,Xn−t,0,...,0), padding t zeroes.
 *  3. ∀j,k: Pj sends yk to Pk .
 *  4. ∀k: Pk checks whether all received values are equal. If so, be happy, otherwise cry.
 *
 *  This protocol uses when Ps wants to broadcast exactly n−t values.
 *  if Ps wants more than n-t values we divide the values to buckes.
 *  Every bucket contains n-t values.
 *
 *  @param myMessage = vector of all the values which Ps wants to broadcast.
 *  @param recBufsdiff = the values which received from the protocol.
 */
template <class FieldType>
bool ProtocolParty<FieldType>::broadcast(int party_id, vector<byte> myMessage, vector<vector<byte>> &recBufsdiffBytes, HIM<FieldType> &mat)
{
    int no_buckets;
    vector<vector<byte>> sendBufsBytes(N);
    vector<vector<FieldType>> sendBufsElements(N);
    vector<vector<FieldType>> recBufsElements(N);

    vector<vector<byte>> recBufs2Bytes(N);
    vector<vector<FieldType>> recBufs2Elements(N);

    // Ps sends his values to all parties and received there values.
    //comm->roundfunction2(myMessage, recBufsdiffBytes); // Values are in recBufsdiff
    roundFunctionSyncBroadcast(myMessage, recBufsdiffBytes);

    //turn the recbuf into recbuf of elements
    int fieldByteSize = field->getElementSizeInBytes();
    for(int i=0; i < N; i++)
    {
        recBufsElements[i].resize((recBufsdiffBytes[i].size()) / fieldByteSize);
        for(int j=0; j<recBufsElements[i].size();j++) {
            recBufsElements[i][j] = field->bytesToElement(recBufsdiffBytes[i].data() + ( j * fieldByteSize));
        }
    }

    vector<FieldType> X1(N);
    vector<FieldType> Y1(N);

    // calculate total number of values which received
    int count = 0;
    for(int i=0; i< N; i++)
    {
        count+=recBufsElements[i].size();
    }

    vector<FieldType> valBufs(count);
    int index = 0;

    // concatenate everything
    for(int l=0; l< N; l++)
    {
        for (int i = 0; i < recBufsElements[l].size() ; i++) {
            valBufs[index] = recBufsElements[l][i];
            index++;
        }
    }

    index = 0;


    // nr of buckets
    no_buckets = count / (N - T) + 1; // nr of buckets


    for(int i = 0; i < N; i++)
        sendBufsElements[i].resize(no_buckets);

    for(int k = 0; k < no_buckets; k++)
    {
        for(int i = 0; i < N; i++)
        {
            if((i < N-T) && (k*(N-T)+i < count))
            {
                //X1[i]= field->stringToElement(valBufs[index]);
                X1[i]= valBufs[index];
                index++;
            }
            else
            {
                // padding zero
                X1[i] = *(field->GetZero());
            }
        }

        // x1 contains (up to) N-T values from ValBuf
        mat.MatrixMult(X1, Y1); // no cheating: all parties have same y1

        // ‘‘Reconstruct’’ values towards some party (‘‘reconstruct’’ with degree 0)

        for(int i = 0; i < N; i++)

            sendBufsElements[i][k] = Y1[i];

        for(int i = 0; i < N; i++)
        {
            X1[i] = *(field->GetZero());
            Y1[i] = *(field->GetZero());
        }
    }

    for(int i=0; i < N; i++)
    {
        sendBufsBytes[i].resize(no_buckets*fieldByteSize);
        recBufs2Bytes[i].resize(no_buckets*fieldByteSize);
        for(int j=0; j<no_buckets;j++) {
            field->elementToBytes(sendBufsBytes[i].data() + (j * fieldByteSize), sendBufsElements[i][j]);
        }
    }

    roundFunctionSync(sendBufsBytes, recBufs2Bytes,3);
    //comm->roundfunctionI(sendBufsBytes, recBufs2Bytes,3);

    for(int i=0; i < N; i++)
    {
        recBufs2Elements[i].resize((recBufs2Bytes[i].size()) / fieldByteSize);
        for(int j=0; j<recBufs2Elements[i].size();j++) {
            recBufs2Elements[i][j] = field->bytesToElement(recBufs2Bytes[i].data() + ( j * fieldByteSize));
        }
    }

    FieldType temp1;
    // no cheating: all parties have same y1
    // ‘‘Reconstruct’’ values towards some party (‘‘reconstruct’’ with degree 0)
    for(int k=0; k < no_buckets; k++)
    {
        if(recBufs2Elements[0].size() > 0)
        {
            temp1 = recBufs2Elements[0][k];
            //  arr.size()-1
            for (int i = 1; i < N; i++)
            {
                if(temp1 != recBufs2Elements[i][k])
                {
                    // cheating detected!!!
                    return false;
                }
            }
        }
    }


    return true;
}

template <class FieldType>
void ProtocolParty<FieldType>::readMyInputs()
{
    myInputs[0] = stoi(inputsFile);
}

template <class FieldType>
string ProtocolParty<FieldType>::run()
{
    runOffline();
    return runOnline();
}

/**
 * This method runs the protocol:
 * Preparation Phase
 */
template <class FieldType>
void ProtocolParty<FieldType>::runOffline() {
    shareIndex = 0;//numOfInputGates;

    auto t1 = high_resolution_clock::now();
    if(RandomSharingForInputs() == false) {
        return;
    }
    else {
    }

    auto t2 = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(t2-t1).count();

    t1 = high_resolution_clock::now();
    if(preparationPhase() == false) {
        return;
    }
    else {
    }
    t2 = high_resolution_clock::now();
    duration = duration_cast<milliseconds>(t2-t1).count();


    t1 = high_resolution_clock::now();
    if(inputPreparation() == false) {
        return;
    }

    t2 = high_resolution_clock::now();
    duration = duration_cast<milliseconds>(t2-t1).count();
}

/**
 * This method runs the protocol:
 * Input Phase
 * Computation Phase
 * Verification Phase
 * Output Phase
 */
template <class FieldType>
string ProtocolParty<FieldType>::runOnline() {
    string sss = "";

    inputAdjustment(sss/*, matrix_him*/);
    computationPhase(m);
    return outputPhase();

}

template <class FieldType>
void ProtocolParty<FieldType>::computationPhase(HIM<FieldType> &m) {
    int count = 0;
    //processRandoms();

    int numOfLayers = circuit.getLayers().size();
    for(int i=0; i<numOfLayers-1;i++)
    {
        currentCirciutLayer = i;
        count = processNotMult();
        count += processMultiplications(m);

    }

}

/**
 * the function implements the second step of Input Phase:
 * the party broadcasts for each input gate the difference between
 * the random secret and the actual input value.
 * @param diff
 */
template <class FieldType>
void ProtocolParty<FieldType>::inputAdjustment(string &diff)
{
    int input;
    int index = 0;

    vector<FieldType> diffElements;
    vector<byte> sendBufBytes;

    // read the inputs of the party

    vector<int> sizes(N);
    for (int k = 0; k < numOfInputGates; k++)
    {
        if(circuit.getGates()[k].gateType == INPUT) {
            sizes[circuit.getGates()[k].party]++;

            if (circuit.getGates()[k].party == m_partyId) {
                input = myInputs[index];
                index++;

                // the value is gateValue[k], but should be input.
                FieldType myinput = field->GetElement(input);
                FieldType different = myinput - gateValueArr[k];
                diffElements.push_back(different);
            }
        }
    }

    // p17

    int fieldByteSize = field->getElementSizeInBytes();

    sendBufBytes.resize(diffElements.size()*fieldByteSize);
    for(int j=0; j<diffElements.size();j++) {
        field->elementToBytes(sendBufBytes.data() + (j * fieldByteSize), diffElements[j]);
    }


    vector<vector<byte>> recBufsdiffBytes(N);
    vector<vector<FieldType>> recBufsdiffElements(N);

    //adjust the size of the difference we need to recieve
    for(int i=0; i<N; i++)
        recBufsdiffBytes[i].resize(sizes[i]*fieldByteSize);

    // Broadcast the difference between GateValue[k] to x.
    if(broadcast(m_partyId, sendBufBytes, recBufsdiffBytes, matrix_him) == false)
        return;

    // handle after broadcast
    FieldType db;

    //turn the elements to bytes
    for(int i=0; i < N; i++)
    {
        recBufsdiffElements[i].resize((recBufsdiffBytes[i].size()) / fieldByteSize);
        for(int j=0; j<recBufsdiffElements[i].size();j++) {
            recBufsdiffElements[i][j] = field->bytesToElement(recBufsdiffBytes[i].data() + ( j * fieldByteSize));
        }
    }

    vector<int> counters(N);

    for(int i=0; i<N; i++){
        counters[i] =0;
    }

    for (int k = 0; k < numOfInputGates; k++)
    {
        if(circuit.getGates()[k].gateType == INPUT)
        {
            db = recBufsdiffElements[circuit.getGates()[k].party][counters[circuit.getGates()[k].party]];
            counters[circuit.getGates()[k].party] += 1;
            gateShareArr[circuit.getGates()[k].output] = gateShareArr[circuit.getGates()[k].output] + db; // adjustment
        }
    }

}

/**
 * some global variables are initialized
 * @param GateValueArr
 * @param GateShareArr
 * @param matrix_him
 * @param matrix_vand
 * @param alpha
 */
template <class FieldType>
void ProtocolParty<FieldType>::initializationPhase()
{
    beta.resize(1);
    gateValueArr.resize(M);  // the value of the gate (for my input and output gates)
    gateShareArr.resize(M - circuit.getNrOfOutputGates()); // my share of the gate (for all gates)
    alpha.resize(N); // N distinct non-zero field elements
    vector<FieldType> alpha1(N-T);
    vector<FieldType> alpha2(T);

    beta[0] = field->GetElement(0); // zero of the field
    matrix_for_interpolate.allocate(1,N, field);


    matrix_him.allocate(N,N,field);
    matrix_vand.allocate(N,N,field);
    m.allocate(T, N-T,field);

    // Compute Vandermonde matrix VDM[i,k] = alpha[i]^k
    matrix_vand.InitVDM();

    // Prepare an N-by-N hyper-invertible matrix
    matrix_him.InitHIM();

    // N distinct non-zero field elements
    for(int i=0; i<N; i++)
    {
        alpha[i]=field->GetElement(i+1);
    }

    for(int i = 0; i < N-T; i++)
    {
        alpha1[i] = alpha[i];
    }
    for(int i = N-T; i < N; i++)
    {
        alpha2[i - (N-T)] = alpha[i];
    }

    m.InitHIMByVectors(alpha1, alpha2);

    matrix_for_interpolate.InitHIMByVectors(alpha, beta);

    vector<FieldType> alpha_until_t(T + 1);
    vector<FieldType> alpha_from_t(N - 1 - T);

    // Interpolate first d+1 positions of (alpha,x)
    matrix_for_t.allocate(N - 1 - T, T + 1, field); // slices, only positions from 0..d
    //matrix_for_t.InitHIMByVectors(alpha_until_t, alpha_from_t);
    matrix_for_t.InitHIMVectorAndsizes(alpha, T+1, N-T-1);

    vector<FieldType> alpha_until_2t(2*T + 1);
    vector<FieldType> alpha_from_2t(N - 1 - 2*T);

    // Interpolate first d+1 positions of (alpha,x)
    matrix_for_2t.allocate(N - 1 - 2*T, 2*T + 1, field); // slices, only positions from 0..d
    //matrix_for_2t.InitHIMByVectors(alpha_until_2t, alpha_from_2t);
    matrix_for_2t.InitHIMVectorAndsizes(alpha, 2*T + 1, N-(2*T +1));
}

/**
 * The function compute t shared authentication checks and to reconstruct the n sharings,
 * one towards each party, who then computes the secret and sends it to everybody.
 * Each party receives n − t secrets and t authentication checks.
 * Reconstruct a bunch of degree-d sharings to all parties (into ValBuf)
 * @param myShares
 * @param alpha
 * @param valBuf
 */
template <class FieldType>
void ProtocolParty<FieldType>::publicReconstruction(vector<FieldType> &myShares, int &count, int d, vector<FieldType> &valBuf, HIM<FieldType> &m)
{
    int no_buckets = count / (N-T) + 1;
    FieldType x;

    vector<FieldType> x1(N);
    vector<FieldType> y1(N);
    vector<FieldType> y2(N);

    vector<vector<FieldType>> sendBufsElements(N);
    vector<vector<byte>> sendBufsBytes(N);

    vector<vector<byte>> sendBufs2Bytes(N);
    vector<vector<FieldType>> sendBufsElements2(N);

    vector<vector<byte>> recBufsBytes(N);
    vector<vector<byte>> recBufs2Bytes(N);

    for(int i = 0; i < N; i++)
    {
        sendBufsElements[i].resize(no_buckets);
        sendBufsElements2[i].resize(no_buckets);
    }

    // init x to be vector of degree-d (d=2*t) shares of n−t secret
    for(int k=0; k < no_buckets; k++)
    {
        for(int i = 0; i < N-T; i++)
        {
            if( k*(N-T)+i < count)
            {
                // k*(N-T)+i
                x1[i] = myShares[k*(N-T)+i];
            }
            else
            {
                x1[i] = *(field->GetZero());
            }
        }

        // compute y = M*x and append it to x
        m.MatrixMult(x1, y1);

        for(int i = 0; i < T; i++)
        {
            x1[N-T+i] = y1[i];
        }

        // ∀i, j: Pi sends xj to Pj
        for(int i = 0; i < N; i++)
        {
            sendBufsElements[i][k] = x1[i];
        }
    }

    int fieldByteSize = field->getElementSizeInBytes();

    for(int i=0; i < N; i++)
    {
        sendBufsBytes[i].resize(no_buckets*fieldByteSize);
        recBufsBytes[i].resize(no_buckets*fieldByteSize);
        for(int j=0; j<no_buckets;j++) {
            field->elementToBytes(sendBufsBytes[i].data() + (j * fieldByteSize), sendBufsElements[i][j]);
        }
    }

    roundFunctionSync(sendBufsBytes, recBufsBytes,1);

    for(int k=0; k < no_buckets; k++) {

        for (int i = 0; i < N; i++)

            x1[i] = field->bytesToElement(recBufsBytes[i].data() + (k*fieldByteSize));

        // interpolate {xj}i to x
        x = interpolate(x1);

        // send x to all parties
        for (int i = 0; i < N; i++) {
            //sendBufs2[i] += field->elementToString(x) + "*";
            sendBufsElements2[i][k] = x;
        }
    }

    for(int i=0; i < N; i++)
    {
        sendBufs2Bytes[i].resize(no_buckets*fieldByteSize );
        recBufs2Bytes[i].resize(no_buckets*fieldByteSize );
        for(int j=0; j<no_buckets;j++) {
            field->elementToBytes(sendBufs2Bytes[i].data() + (j * fieldByteSize), sendBufsElements2[i][j]);
        }
    }

    roundFunctionSync(sendBufs2Bytes, recBufs2Bytes,8);
    int index = 0;
    for(int k=0; k < no_buckets; k++) {
        for (int i = 0; i < N; i++) {

            x1[i] = field->bytesToElement(recBufs2Bytes[i].data() + (k*fieldByteSize));
        }

        // checking that (Xn−t,...,Xn) = M*(X1,...,Xn−t)
        m.MatrixMult(x1, y1);

        for (int i = 0; i < N-T; i++) {
            if(k*(N-T)+i < count)
            {
                valBuf[index] = x1[i];
                index++;
            }
        }
    }

}

template <class FieldType>
bool ProtocolParty<FieldType>::preparationPhase(/*VDM<FieldType> &matrix_vand, HIM<FieldType> &matrix_him*/)
{
    vector<vector<byte>> recBufsBytes(N);
    //vector<vector<byte>> recBufs1Bytes(N);
    int robin = 0;

    // the number of random double sharings we need altogether
    int no_random = circuit.getNrOfMultiplicationGates();
    vector<FieldType> x1(N),x2(N),y1(N),y2(N);

    vector<vector<FieldType>> sendBufsElements(N);
    vector<vector<byte>> sendBufsBytes(N);

    // the number of buckets (each bucket requires one double-sharing
    // from each party and gives N-2T random double-sharings)
    int no_buckets = (no_random / (N-2*T))+1;


    sharingBufTElements.resize(no_buckets*(N-2*T)); // my shares of the double-sharings
    sharingBuf2TElements.resize(no_buckets*(N-2*T)); // my shares of the double-sharings

    for(int i=0; i < N; i++)
    {
        //sendBufs[i] = "";

        sendBufsElements[i].resize(no_buckets*2);
        sendBufsBytes[i].resize(no_buckets*2*field->getElementSizeInBytes());
        recBufsBytes[i].resize(no_buckets*2*field->getElementSizeInBytes());
    }

    /**
     *  generate double sharings.
     *  first degree t.
     *  subsequent: degree 2t with same secret.
     */
    high_resolution_clock::time_point t1 = high_resolution_clock::now();
    for(int k=0; k < no_buckets; k++)
    {
        // generate random degree-T polynomial
        for(int i = 0; i < T+1; i++)
        {
            // A random field element, uniform distribution
            x1[i] = field->Random();

        }

        x2[0] = x1[0];


        for(int i = 1; i < 2*T+1; i++)
        {
            // otherwise random
            x2[i] = field->Random();
        }

        matrix_vand.MatrixMult(x1, y1, T+1); // eval poly at alpha-positions
        matrix_vand.MatrixMult(x2, y2, 2*T+1); // eval poly at alpha-positions

        // prepare shares to be sent
        for(int i=0; i < N; i++)
        {

            sendBufsElements[i][2*k] = y1[i];
            sendBufsElements[i][2*k+1] = y2[i];
        }

    }//end print one


    high_resolution_clock::time_point t2 = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>( t2 - t1 ).count();

//
    int fieldByteSize = field->getElementSizeInBytes();
    for(int i=0; i < N; i++)
    {
        for(int j=0; j<sendBufsElements[i].size();j++) {
            field->elementToBytes(sendBufsBytes[i].data() + (j * fieldByteSize), sendBufsElements[i][j]);
        }
    }


    high_resolution_clock::time_point t3 = high_resolution_clock::now();
    //comm->roundfunctionI(sendBufsBytes, recBufsBytes,4);
    roundFunctionSync(sendBufsBytes, recBufsBytes,4);
    high_resolution_clock::time_point t4 = high_resolution_clock::now();
    auto duration2 = duration_cast<milliseconds>( t4 - t3 ).count();


    /**
     * Apply hyper-invertible matrix on each bucket.
     * From the resulting sharings, 2T are being reconstructed towards some party,
     * the remaining N-2T are kept as prepared sharings.
     * For balancing, we do round-robin the party how shall reconstruct and check!
     */


    //vector<vector<FieldType>> sendBufs1Elements(N);
    //vector<vector<byte>> sendBufs1Bytes(N);

    for(int i=0; i<N; i++){
        sendBufsElements[i].clear();

    }

    int fieldBytesSize = field->getElementSizeInBytes();


    // x1 : used for the N degree-t sharings
    // x2 : used for the N degree-2t sharings
    for(int k=0; k < no_buckets; k++) {
        // generate random degree-T polynomial
        for (int i = 0; i < N; i++) {
            x1[i] = field->bytesToElement(recBufsBytes[i].data() + (2*k*fieldBytesSize));
            x2[i] = field->bytesToElement(recBufsBytes[i].data() + ((2*k + 1)*fieldBytesSize));

        }
        matrix_him.MatrixMult(x1, y1);
        matrix_him.MatrixMult(x2, y2);
        // these shall be checked
        for (int i = 0; i < 2 * T; i++) {
            sendBufsElements[robin].push_back(y1[i]);
            sendBufsElements[robin].push_back(y2[i]);
            robin = (robin+1) % N; // next robin

        }
        // Y1 : the degree-t shares of my poly
        // Y2 : the degree 2t shares of my poly
        for (int i = 2 * T; i < N; i++) {

            sharingBufTElements[k*(N-2*T) + i - 2*T] = y1[i];
            sharingBuf2TElements[k*(N-2*T) + i - 2*T] =  y2[i];
        }



        x2[0] = *(field->GetZero());
        x1[0] = *(field->GetZero());

    }

    for(int i=0; i < N; i++)
    {
        sendBufsBytes[i].resize(sendBufsElements[i].size()*fieldByteSize);
        recBufsBytes[i].resize(sendBufsElements[m_partyId].size()*fieldByteSize);
        for(int j=0; j<sendBufsElements[i].size();j++) {
            field->elementToBytes(sendBufsBytes[i].data() + (j * fieldByteSize), sendBufsElements[i][j]);
        }
    }

    roundFunctionSync(sendBufsBytes, recBufsBytes,5);

    int count = no_buckets * (2*T) / N; // nr of sharings *I* have to check
    // got one in the last round
    if(no_buckets * (2*T)%N > m_partyId) { // maybe -1
        count++;
    }

    for(int k=0; k < count; k++) {
        for (int i = 0; i < N; i++) {

            x1[i] = field->bytesToElement(recBufsBytes[i].data() + (2*k*fieldBytesSize));
            x2[i] = field->bytesToElement(recBufsBytes[i].data() + ((2*k +1)*fieldBytesSize));
        }


        vector<FieldType> x_until_d(N);
        for(int i=0; i<T; i++)
        {
            x_until_d[i] = x1[i];
        }
        for(int i=T; i<N; i++)
        {
            x_until_d[i] = *(field->GetZero());
        }
        // Check that x1 is t-consistent and x2 is 2t-consistent and secret is the same
        if(!checkConsistency(x1,T) || !checkConsistency(x2,2*T) ||
           (interpolate(x1)) != (interpolate(x2)))  {
            // cheating detected, abort
            return false;
        }
    }
    return true;
}

template <class FieldType>
bool ProtocolParty<FieldType>::RandomSharingForInputs()
{
    vector<vector<byte>> recBufsBytes(N);
    //vector<vector<byte>> recBufs1Bytes(N);
    int robin = 0;

    // the number of random double sharings we need altogether
    int no_random = circuit.getNrOfInputGates();
    vector<FieldType> x1(N),y1(N);

    vector<vector<FieldType>> sendBufsElements(N);
    vector<vector<byte>> sendBufsBytes(N);

    // the number of buckets (each bucket requires one double-sharing
    // from each party and gives N-2T random double-sharings)
    int no_buckets = (no_random / (N-2*T))+1;

    //sharingBufTElements.resize(no_buckets*(N-2*T)); // my shares of the double-sharings
    //sharingBuf2TElements.resize(no_buckets*(N-2*T)); // my shares of the double-sharings
    sharingBufInputsTElements.resize(no_buckets*(N-2*T));

    for(int i=0; i < N; i++)
    {
        //sendBufs[i] = "";

        sendBufsElements[i].resize(no_buckets);
        sendBufsBytes[i].resize(no_buckets*field->getElementSizeInBytes());
        recBufsBytes[i].resize(no_buckets*field->getElementSizeInBytes());
    }

    /**
     *  generate double sharings.
     *  first degree t.
     *  subsequent: degree 2t with same secret.
     */
    high_resolution_clock::time_point t1 = high_resolution_clock::now();
    for(int k=0; k < no_buckets; k++)
    {
        // generate random degree-T polynomial
        for(int i = 0; i < T+1; i++)
        {
            // A random field element, uniform distribution
            x1[i] = field->Random();

        }

        matrix_vand.MatrixMult(x1, y1, T+1); // eval poly at alpha-positions

        // prepare shares to be sent
        for(int i=0; i < N; i++)
            sendBufsElements[i][k] = y1[i];


    }//end print one


    int fieldByteSize = field->getElementSizeInBytes();
    for(int i=0; i < N; i++)
    {
        for(int j=0; j<sendBufsElements[i].size();j++) {
            field->elementToBytes(sendBufsBytes[i].data() + (j * fieldByteSize), sendBufsElements[i][j]);
        }
    }

    high_resolution_clock::time_point t3 = high_resolution_clock::now();
    roundFunctionSync(sendBufsBytes, recBufsBytes,4);
    high_resolution_clock::time_point t4 = high_resolution_clock::now();
    auto duration2 = duration_cast<milliseconds>( t4 - t3 ).count();


    /**
     * Apply hyper-invertible matrix on each bucket.
     * From the resulting sharings, 2T are being reconstructed towards some party,
     * the remaining N-2T are kept as prepared sharings.
     * For balancing, we do round-robin the party how shall reconstruct and check!
     */

    for(int i=0; i<N; i++)
        sendBufsElements[i].clear();

    int fieldBytesSize = field->getElementSizeInBytes();

    // x1 : used for the N degree-t sharings
    // x2 : used for the N degree-2t sharings
    for(int k=0; k < no_buckets; k++) {
        // generate random degree-T polynomial
        for (int i = 0; i < N; i++) {
            x1[i] = field->bytesToElement(recBufsBytes[i].data() + (k*fieldBytesSize));

        }
        matrix_him.MatrixMult(x1, y1);
        // these shall be checked
        for (int i = 0; i < 2 * T; i++) {
            sendBufsElements[robin].push_back(y1[i]);
            robin = (robin+1) % N; // next robin

        }
        // Y1 : the degree-t shares of my poly
        // Y2 : the degree 2t shares of my poly
        for (int i = 2 * T; i < N; i++)
            sharingBufInputsTElements[k*(N-2*T) + i - 2*T] = y1[i];
    }

    for(int i=0; i < N; i++)
    {
        sendBufsBytes[i].resize(sendBufsElements[i].size()*fieldByteSize);
        recBufsBytes[i].resize(sendBufsElements[m_partyId].size()*fieldByteSize);
        for(int j=0; j<sendBufsElements[i].size();j++) {
            field->elementToBytes(sendBufsBytes[i].data() + (j * fieldByteSize), sendBufsElements[i][j]);
        }
    }

    int count = no_buckets * (2*T) / N; // nr of sharings *I* have to check
    // got one in the last round
    if(no_buckets * (2*T)%N > m_partyId) { // maybe -1
        count++;
    }

    {
        vector<vector<FieldType>> BufsElement(N);
        for (int i = 0; i < N; i++)
        {
            for(int k=0; k < count; k++)
            {
                BufsElement[i].push_back(field->bytesToElement(recBufsBytes[i].data() + (k * fieldBytesSize)));
            }
        }
    }


    for(int k=0; k < count; k++) {
        for (int i = 0; i < N; i++) {

            x1[i] = field->bytesToElement(recBufsBytes[i].data() + (k*fieldBytesSize));
        }


        vector<FieldType> x_until_d(N);
        for(int i=0; i<T; i++)
        {
            x_until_d[i] = x1[i];
        }
        for(int i=T; i<N; i++)
        {
            x_until_d[i] = *(field->GetZero());
        }

        // Check that x1 is t-consistent and x2 is 2t-consistent and secret is the same
        if(!checkConsistency(x1,T) )  {
            // cheating detected, abort
            return false;
        }
    }
    return true;
}
/**
 * the function implements the first step of Input Phase:
 * for each input gate, a prepared t-sharings is reconstructed
 * towards the party giving input
 */
template <class FieldType>
bool ProtocolParty<FieldType>::inputPreparation()
{
    vector<vector<FieldType>> sendBufsElements(N); // upper bound
    vector<vector<byte>> sendBufsBytes(N);

    vector<vector<FieldType>> recBufsElements(N);
    vector<vector<byte>> recBufsBytes(N);
    vector<FieldType> x1(N); // vector for the shares of my inputs
    FieldType elem;
    FieldType secret;
    int i;


    for(int k = 0; k < numOfInputGates; k++)//these are only input gates
    {
        gateShareArr[circuit.getGates()[k].output] = sharingBufInputsTElements[k];
        i = (circuit.getGates())[k].party; // the number of party which has the input
        // reconstruct sharing towards input party
        sendBufsElements[i].push_back(gateShareArr[circuit.getGates()[k].output]);

    }

    int fieldByteSize = field->getElementSizeInBytes();

    for(int i=0; i < N; i++)
    {
        sendBufsBytes[i].resize(sendBufsElements[i].size()*fieldByteSize);
        for(int j=0; j<sendBufsElements[i].size();j++) {
            field->elementToBytes(sendBufsBytes[i].data() + (j * fieldByteSize), sendBufsElements[i][j]);
        }
    }

    for(int i=0; i<N; i++){
        recBufsBytes[i].resize(sendBufsBytes[m_partyId].size());
    }


    roundFunctionSync(sendBufsBytes, recBufsBytes,6);

    //turn the recbuf into recbuf of elements
    for(int i=0; i < N; i++)
    {
        recBufsElements[i].resize((recBufsBytes[i].size()) / fieldByteSize);
        for(int j=0; j<recBufsElements[i].size();j++) {
            recBufsElements[i][j] = field->bytesToElement(recBufsBytes[i].data() + ( j * fieldByteSize));
        }
    }


    int counter = 0;
    // reconstruct my random input values
    for(int k = 0; k < numOfInputGates; k++) {
        if (circuit.getGates()[k].party == m_partyId) {

            for (int i = 0; i < N; i++) {
                x1[i] = recBufsElements[i][counter];
            }
            counter++;
            if(!checkConsistency(x1, T))
            {
                // someone cheated!
                return false;
            }
            // the (random) secret
            secret = interpolate(x1);

            gateValueArr[k] = secret;
        }
    }

    return true;

}

/**
 * Check whether given points lie on polynomial of degree d. This check is performed by interpolating x on
 * the first d + 1 positions of α and check the remaining positions.
 */
template <class FieldType>
bool ProtocolParty<FieldType>::checkConsistency(vector<FieldType>& x, int d)
{
    if(d == T)
    {
        vector<FieldType> y(N - 1 - d); // the result of multiplication
        vector<FieldType> x_until_t(T + 1);

        for (int i = 0; i < T + 1; i++) {
            x_until_t[i] = x[i];
        }


        matrix_for_t.MatrixMult(x_until_t, y);

        // compare that the result is equal to the according positions in x
        for (int i = 0; i < N - d - 1; i++)   // n-d-2 or n-d-1 ??
        {
            if ((y[i]) != (x[d + 1 + i])) {
                return false;
            }
        }
        return true;
    } else if (d == 2*T)
    {
        vector<FieldType> y(N - 1 - d); // the result of multiplication

        vector<FieldType> x_until_2t(2*T + 1);

        for (int i = 0; i < 2*T + 1; i++) {
            x_until_2t[i] = x[i];
        }

        matrix_for_2t.MatrixMult(x_until_2t, y);

        // compare that the result is equal to the according positions in x
        for (int i = 0; i < N - d - 1; i++)   // n-d-2 or n-d-1 ??
        {
            if ((y[i]) != (x[d + 1 + i])) {
                return false;
            }
        }
        return true;

    } else {
        vector<FieldType> alpha_until_d(d + 1);
        vector<FieldType> alpha_from_d(N - 1 - d);
        vector<FieldType> x_until_d(d + 1);
        vector<FieldType> y(N - 1 - d); // the result of multiplication

        for (int i = 0; i < d + 1; i++) {
            alpha_until_d[i] = alpha[i];
            x_until_d[i] = x[i];
        }
        for (int i = d + 1; i < N; i++) {
            alpha_from_d[i - (d + 1)] = alpha[i];
        }
        // Interpolate first d+1 positions of (alpha,x)
        HIM<FieldType> matrix(N - 1 - d, d + 1, field); // slices, only positions from 0..d
        matrix.InitHIMByVectors(alpha_until_d, alpha_from_d);
        matrix.MatrixMult(x_until_d, y);

        // compare that the result is equal to the according positions in x
        for (int i = 0; i < N - d - 1; i++)   // n-d-2 or n-d-1 ??
        {
            //if (field->elementToString(y[i]) != field->elementToString(x[d + 1 + i])) {
            if (y[i] != x[d + 1 + i]) {
                return false;
            }
        }
        return true;
    }
    return true;
}

// Interpolate polynomial at position Zero
template <class FieldType>
FieldType ProtocolParty<FieldType>::interpolate(vector<FieldType> x)
{
    vector<FieldType> y(N); // result of interpolate
    matrix_for_interpolate.MatrixMult(x, y);
    return y[0];
}

template <class FieldType>
int ProtocolParty<FieldType>::processNotMult(){
    int count=0;
    for(int k=circuit.getLayers()[currentCirciutLayer]; k < circuit.getLayers()[currentCirciutLayer+1]; k++)
    {
        switch(circuit.getGates()[k].gateType)
        {
            case MULT:
                break;
            case ADD:
                gateShareArr[circuit.getGates()[k].output] = gateShareArr[circuit.getGates()[k].input1] + gateShareArr[circuit.getGates()[k].input2];
                count++;
                break;
            case SUB:
                gateShareArr[circuit.getGates()[k].output] = gateShareArr[circuit.getGates()[k].input1] - gateShareArr[circuit.getGates()[k].input2];
                count++;
                break;
            case SCALAR:
            {
                long scalar(circuit.getGates()[k].input2);
                FieldType e = field->GetElement(scalar);
                gateShareArr[circuit.getGates()[k].output] = gateShareArr[circuit.getGates()[k].input1] * e;
                count++;
            }
                break;
            case SCALAR_ADD:
            {
                long scalar(circuit.getGates()[k].input2);
                FieldType e = field->GetElement(scalar);
                gateShareArr[circuit.getGates()[k].output] = gateShareArr[circuit.getGates()[k].input1] + e;
                count++;
            }
                break;
            default:
                cerr << "ProtocolParty<FieldType>::processNotMult(): unsupported gate type " << circuit.getGates()[k].gateType << endl;
        }

    }
    return count;

}

/**
 * the Function process all multiplications which are ready.
 * @return the number of processed gates.
 */
template <class FieldType>
int ProtocolParty<FieldType>::processMultiplications(HIM<FieldType> &m)
{
    int count =0;
    int index = 0;
    FieldType p2, d2;
    FieldType r1, r2;
    vector<FieldType> valBuf(circuit.getLayers()[currentCirciutLayer+1]- circuit.getLayers()[currentCirciutLayer]); // Buffers for differences
    FieldType d;
    int indexForValBuf = 0;
    vector<FieldType> ReconsBuf(circuit.getLayers()[currentCirciutLayer+1]- circuit.getLayers()[currentCirciutLayer]);

    for(int k = circuit.getLayers()[currentCirciutLayer]; k < circuit.getLayers()[currentCirciutLayer+1] ; k++)//go over only the logit gates
    {
        // its a multiplication which not yet processed and ready
        if(circuit.getGates()[k].gateType == MULT )
        {

            r1 = sharingBufTElements[shareIndex]; // t-share of random r
            r2 = sharingBuf2TElements[shareIndex]; // t2-share of same r

            shareIndex++;


            p2 = gateShareArr[circuit.getGates()[k].input1] * gateShareArr[circuit.getGates()[k].input2]; // product share (degree-2t)
            d2 = p2 - r2; // t2-share of difference
            ReconsBuf[index] = d2; // reconstruct difference (later)
            index++;
            // for now gateShareArr[k] is random sharing, needs to be adjusted (later)
            gateShareArr[circuit.getGates()[k].output] = r1;
        }

    }

    if(index == 0)
    {
        return count;
    }

    // reconstruct the differences into valBuf
    publicReconstruction(ReconsBuf, index, 2*T, valBuf, m);
    indexForValBuf = index-1;

    for(int k=circuit.getLayers()[currentCirciutLayer+1]-1 ; k >= circuit.getLayers()[currentCirciutLayer]; k--)
    {
        // its a multiplication which not yet processed and ready
        if(circuit.getGates()[k].gateType == MULT)
        {
            d = valBuf[indexForValBuf];  // the difference
            indexForValBuf--;
            gateShareArr[circuit.getGates()[k].output] = gateShareArr[circuit.getGates()[k].output] + d; // the adjustment
            count++;
        }
    }
    return count;
}


/**
 * the function Walk through the circuit and reconstruct output gates.
 * @param circuit
 * @param gateShareArr
 * @param alpha
 */
template <class FieldType>
string ProtocolParty<FieldType>::outputPhase()
{
    int count=0;
    string output = "";
    vector<FieldType> x1(N); // vector for the shares of my outputs
    vector<vector<FieldType>> sendBufsElements(N);
    vector<vector<byte>> sendBufsBytes(N);
    vector<vector<byte>> recBufBytes(N);

    FieldType num;

    for(int k=M-numOfOutputGates; k < M; k++)
    {
        if(circuit.getGates()[k].gateType == OUTPUT)
        {
            // send to party (which need this gate) your share for this gate
            sendBufsElements[circuit.getGates()[k].party].push_back(gateShareArr[circuit.getGates()[k].input1]);
        }
    }


    int fieldByteSize = field->getElementSizeInBytes();
    for(int i=0; i < N; i++)
    {
        sendBufsBytes[i].resize(sendBufsElements[i].size()*fieldByteSize);
        recBufBytes[i].resize(sendBufsElements[m_partyId].size()*fieldByteSize);
        for(int j=0; j<sendBufsElements[i].size();j++) {
            field->elementToBytes(sendBufsBytes[i].data() + (j * fieldByteSize), sendBufsElements[i][j]);
        }
    }
    roundFunctionSync(sendBufsBytes, recBufBytes,7);

    int counter = 0;

    for(int k=M-numOfOutputGates ; k < M; k++) {
        if(circuit.getGates()[k].gateType == OUTPUT && circuit.getGates()[k].party == m_partyId)
        {
            for(int i=0; i < N; i++) {

                x1[i] = field->bytesToElement(recBufBytes[i].data() + (counter*fieldByteSize));
            }


            // my output: reconstruct received shares
            if (!checkConsistency(x1, T))
            {
                // someone cheated!
                return "Error";
            }
            if(flag_print_output){
                output += field->elementToString(interpolate(x1));

            }

            counter++;
        }
    }

    return output;
}


template <class FieldType>
void ProtocolParty<FieldType>::roundFunctionSync(const vector<vector<byte>> &sendBufs, vector<vector<byte>> &recBufs, int round) {

    int numThreads = parties.size();
    int numPartiesForEachThread;

    if (parties.size() <= numThreads){
        numThreads = parties.size();
        numPartiesForEachThread = 1;
    } else{
        numPartiesForEachThread = (parties.size() + numThreads - 1)/ numThreads;
    }


    recBufs[m_partyId] = sendBufs[m_partyId];//move(sendBufs[m_partyId-1]);
    //recieve the data using threads
    vector<thread> threads(numThreads);
    for (int t=0; t<numThreads; t++) {
        if ((t + 1) * numPartiesForEachThread <= parties.size()) {
            threads[t] = thread(&ProtocolParty::exchangeData, this, ref(sendBufs), ref(recBufs),
                                t * numPartiesForEachThread, (t + 1) * numPartiesForEachThread);
        } else {
            threads[t] = thread(&ProtocolParty::exchangeData, this, ref(sendBufs), ref(recBufs), t * numPartiesForEachThread, parties.size());
        }
    }
    for (int t=0; t<numThreads; t++){
        threads[t].join();
    }

}


template <class FieldType>
void ProtocolParty<FieldType>::exchangeData(const vector<vector<byte>> &sendBufs, vector<vector<byte>> &recBufs, int first, int last){

    for (int i=first; i < last; i++)
    {
        if ((m_partyId) < parties[i]->getID())
        {
            parties[i]->getChannel()->write(sendBufs[parties[i]->getID()].data(), sendBufs[parties[i]->getID()].size());
            parties[i]->getChannel()->read(recBufs[parties[i]->getID()].data(), recBufs[parties[i]->getID()].size());
        }
        else
        {
            parties[i]->getChannel()->read(recBufs[parties[i]->getID()].data(), recBufs[parties[i]->getID()].size());
            parties[i]->getChannel()->write(sendBufs[parties[i]->getID()].data(), sendBufs[parties[i]->getID()].size());
        }
    }
}

template <class FieldType>
void ProtocolParty<FieldType>::do_send_and_recv(const vector< vector< byte > > & _2send, vector< vector< byte > > & _2recv)
{

    roundFunctionSync(_2send, _2recv, 5);
}

template <class FieldType>
void ProtocolParty<FieldType>::roundFunctionSyncBroadcast(vector<byte> &message, vector<vector<byte>> &recBufs) {

    int numThreads = parties.size();
    int numPartiesForEachThread;

    if (parties.size() <= numThreads){
        numThreads = parties.size();
        numPartiesForEachThread = 1;
    } else{
        numPartiesForEachThread = (parties.size() + numThreads - 1)/ numThreads;
    }

    recBufs[m_partyId] = message;

    //recieve the data using threads
    vector<thread> threads(numThreads);
    for (int t=0; t<numThreads; t++) {
        if ((t + 1) * numPartiesForEachThread <= parties.size()) {
            threads[t] = thread(&ProtocolParty::recData, this, ref(message), ref(recBufs),
                                t * numPartiesForEachThread, (t + 1) * numPartiesForEachThread);
        } else {
            threads[t] = thread(&ProtocolParty::recData, this, ref(message),  ref(recBufs), t * numPartiesForEachThread, parties.size());
        }
    }
    for (int t=0; t<numThreads; t++){
        threads[t].join();
    }

}


template <class FieldType>
void ProtocolParty<FieldType>::recData(vector<byte> &message, vector<vector<byte>> &recBufs, int first, int last){

    for (int i=first; i < last; i++)
    {
        if ((m_partyId) < parties[i]->getID())
        {
            parties[i]->getChannel()->write(message.data(), message.size());
            parties[i]->getChannel()->read(recBufs[parties[i]->getID()].data(), recBufs[parties[i]->getID()].size());;

        }
        else
        {
            parties[i]->getChannel()->read(recBufs[parties[i]->getID()].data(), recBufs[parties[i]->getID()].size());
            parties[i]->getChannel()->write(message.data(), message.size());

        }
    }
}


template <class FieldType>
ProtocolParty<FieldType>::~ProtocolParty()
{
    delete field;
    io_service.stop();
}


#endif //SCAPILITEAPK_PROTOCOLPARTY_H
