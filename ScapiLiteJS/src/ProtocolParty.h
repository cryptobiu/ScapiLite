#ifndef PROTOCOL_H_
#define PROTOCOL_H_

#include <stdlib.h>
#include "Matrix.h"
#include "ArithmeticCircuit.h"
#include <vector>
#include <iostream>
#include <fstream>
#include <chrono>
#include "TemplateField.h"
#include "MPCCommunicationBF.h"
#include "Common.h"
#include <thread>
#include "Protocol.h"
#include "SecurityLevel.h"

#define flag_print false
#define flag_print_timings true
#define flag_print_output false

using namespace std;
using namespace std::chrono;

typedef enum {
  UNINITIAZLIED,
  RUN_OFFLINE_RANDOM_SHARING,
  RUN_OFFLINE_PREP_PHASE,
  RUN_OFFLINE_INPUT_PREP,
  RUN_ONLINE_INPUT_ADJUSTMENT,
  RUN_ONLINE_COMPUTATION_PHASE,
  RUN_ONLINE_OUTPUT_PHASE,
  DONE
} state_t;

typedef enum {
  PHASE0,
  PHASE1,
  PHASE2,
  PHASE3
} internal_state_t;

bool should_read = false;
int read_from_index = 0;
bool read_to_default_buffer = true;

high_resolution_clock::time_point tstart_program;
high_resolution_clock::time_point tstart_state;
high_resolution_clock::time_point tstart_internal_state;

int iteration_state = 0;

template <class FieldType>
class ProtocolParty : public Protocol, public HonestMajority, public MultiParty {
private:
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
    string inputsFile, outputFile;
    vector<FieldType> beta;
    HIM<FieldType> matrix_for_interpolate;
    HIM<FieldType> matrix_for_t;
    HIM<FieldType> matrix_for_2t;
    TemplateField<FieldType> *field;

    HIM<FieldType> matrix_him;
    VDM<FieldType> matrix_vand;
    HIM<FieldType> m;

    //Communication* comm;
    vector<shared_ptr<ProtocolPartyDataBF>>  parties;

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

    state_t state = UNINITIAZLIED;
    internal_state_t internal_state = PHASE0;

    vector<vector<byte>> sendBufsBytes;
    vector<vector<byte>> recBufsBytes;
    vector<vector<byte>> recBufsBytesTmp;
    vector<FieldType> valBufField;
    vector<FieldType> reconsBufField;
    int indexField = 0;
    ofstream outputFileStream;

public:
    ProtocolParty(int argc, char* argv []);
    void split(const string &s, char delim, vector<string> &elems);
    vector<string> split(const string &s, char delim);


    void roundFunctionASync(vector<vector<byte>> &sendBufs, vector<vector<byte>> &recBufs, int round);
    void exchangeData(vector<vector<byte>> &sendBufs,vector<vector<byte>> &recBufs, int first, int last);
    void roundFunctionSyncBroadcast(vector<byte> &message, vector<vector<byte>> &recBufs);
    void recData(vector<byte> &message, vector<vector<byte>> &recBufs, int first, int last);

    int b = 10;

    /**
     * This method runs the protocol:
     * Preparation Phase
     * Input Phase
     * Computation Phase
     * Output Phase
     */
    bool run() override;

    bool hasOffline() override {
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
    void runOnline() override;

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
    bool broadcast(int party_id, vector<byte> myMessage, vector<vector<byte>> &recBufsdiffBytes, vector<vector<byte>> &recBufsdiffBytesTmp, HIM<FieldType> &mat);

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
    bool inputAdjustment(string &diff/*, HIM<FieldType> &mat*/);

    /**
     * Check whether given points lie on polynomial of degree d.
     * This check is performed by interpolating x on the first d + 1 positions of α and check the remaining positions.
     */
    bool checkConsistency(vector<FieldType>& x, int d);

    /**
     * Process all additions which are ready.
     * Return number of processed gates.
     */
    int processAdditions();


    /**
     * Process all subtractions which are ready.
     * Return number of processed gates.
     */
    int processSubtractions();

    /**
     * Process all multiplications which are ready.
     * Return number of processed gates.
     */
    int processMultiplications(HIM<FieldType> &m);

    /**
     * Process all random gates.
     */
    void processRandoms();

    int processSmul();

    int processNotMult();

    /**
     * Walk through the circuit and evaluate the gates. Always take as many gates at once as possible,
     * i.e., all gates whose inputs are ready.
     * We first process all random gates, then alternately process addition and multiplication gates.
     */
    bool computationPhase(HIM<FieldType> &m);

    /**
     * The cheap way: Create a HIM from the αi’s onto ZERO (this is actually a row vector), and multiply
     * this HIM with the given x-vector (this is actually a scalar product).
     * The first (and only) element of the output vector is the secret.
     */
    FieldType interpolate(vector<FieldType> x);

    FieldType tinterpolate(vector<FieldType> x);

    /**
     * Walk through the circuit and reconstruct output gates.
     */
    bool outputPhase();

    ~ProtocolParty();
};


template <class FieldType>
ProtocolParty<FieldType>::ProtocolParty(int argc, char* argv []) : Protocol ("PerfectSecureMPC", argc, argv)
{

    string circuitFile = argv[5];//this->getParser().getValueByKey(arguments, "circuitFile");
    this->times = stoi(argv[8]);//stoi(this->getParser().getValueByKey(arguments, "internalIterationsNumber"));
    string fieldType = argv[7];//this->getParser().getValueByKey(arguments, "fieldType");
    m_partyId = stoi(argv[1]);//stoi(this->getParser().getValueByKey(arguments, "partyID"));
    int n = stoi(argv[2]);//stoi(this->getParser().getValueByKey(arguments, "partiesNumber"));
    string partiesFileName = argv[6];//this->getParser().getValueByKey(arguments, "partiesFile");

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
    this->inputsFile = argv[3];//this->getParser().getValueByKey(arguments, "inputFile");
    this->outputFile = argv[4];//this->getParser().getValueByKey(arguments, "outputFile");
    if(n%3 > 0)
    {
        T++;
    }

    vector<string> subTaskNames{"Offline", "PreparationForInputPhase", "PreparationPhase", "inputPreparation", "Online",
                                "InputAdjustment", "ComputationPhase", "OutputPhase"};

    s = to_string(m_partyId);
    circuit.readCircuit(circuitFile.c_str());
    circuit.reArrangeCircuit();
    M = circuit.getNrOfGates();
    numOfInputGates = circuit.getNrOfInputGates();
    numOfOutputGates = circuit.getNrOfOutputGates();
    myInputs.resize(numOfInputGates);
    shareIndex = 0;//numOfInputGates;

    parties = MPCCommunicationBF::setCommunication(m_partyId, N, partiesFileName);

    readMyInputs();

    auto t1 = high_resolution_clock::now();
    initializationPhase(/*matrix_him, matrix_vand, m*/);

    auto t2 = high_resolution_clock::now();

    auto duration = duration_cast<milliseconds>(t2-t1).count();
    if(flag_print_timings) {
        cout << "time in milliseconds initializationPhase: " << duration << endl;
    }

    sendBufsBytes = vector<vector<byte>>(N);
	recBufsBytes = vector<vector<byte>>(N);
	recBufsBytesTmp = vector<vector<byte>>(N);
}

template <class FieldType>
void ProtocolParty<FieldType>::split(const string &s, char delim, vector<string> &elems) {
    stringstream ss;
    ss.str(s);
    string item;
    while (getline(ss, item, delim)) {
        elems.push_back(item);
    }
}

template <class FieldType>
vector<string> ProtocolParty<FieldType>::split(const string &s, char delim) {
    vector<string> elems;
    split(s, delim, elems);
    return elems;
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
bool ProtocolParty<FieldType>::broadcast(int party_id, vector<byte> myMessage, vector<vector<byte>> &recBufsdiffBytes, vector<vector<byte>> &recBufsBytesTmp, HIM<FieldType> &mat)
{
    int no_buckets;
    vector<vector<FieldType>> sendBufsElements(N);
    vector<vector<FieldType>> recBufsElements(N);
    int fieldByteSize = field->getElementSizeInBytes();

	switch (internal_state) {
	case PHASE0: {
	    // Ps sends his values to all parties and received there values.
	    //comm->roundfunction2(myMessage, recBufsdiffBytes); // Values are in recBufsdiff
	    roundFunctionSyncBroadcast(myMessage, recBufsdiffBytes);
	    break;
	}
	case PHASE1: {
	    //turn the recbuf into recbuf of elements
	    for (int i=0; i < N; i++) {
	        recBufsElements[i].resize((recBufsdiffBytes[i].size()) / fieldByteSize);
	        for(int j=0; j<recBufsElements[i].size();j++) {
	            recBufsElements[i][j] = field->bytesToElement(recBufsdiffBytes[i].data() + ( j * fieldByteSize));
	        }
	    }

	    if(flag_print) {
	        cout << "recBufsdiff" << endl;
	        for (int i = 0; i < N; i++) {
	            //cout << i << "  " << recBufsdiff[i] << endl;
	        }
	    }

	    vector<FieldType> X1(N);
	    vector<FieldType> Y1(N);

	    // calculate total number of values which received
	    int count = 0;
	    for(int i=0; i< N; i++) {
	        count+=recBufsElements[i].size();
	    }

	    vector<FieldType> valBufs(count);
	    int index = 0;

	    // concatenate everything
	    for (int l=0; l< N; l++) {
	        for (int i = 0; i < recBufsElements[l].size() ; i++) {
	            valBufs[index] = recBufsElements[l][i];
	            index++;
	        }
	    }

	    index = 0;

	    if (flag_print) {
	        cout << "valBufs " <<endl;
	        for (int k = 0; k < count; k++) {
	            cout << "valBufs " << k << " " << valBufs[k] << endl;
	        }
	    }

	    // nr of buckets
	    no_buckets = count / (N - T) + 1; // nr of buckets

	    for (int i = 0; i < N; i++) {
	        sendBufsElements[i].resize(no_buckets);
	    }

	    if (flag_print) {
	        cout << " before the for " << '\n';
	    }

	    for (int k = 0; k < no_buckets; k++) {
	        for (int i = 0; i < N; i++) {
	            if ((i < N-T) && (k*(N-T)+i < count)) {
	                //X1[i]= field->stringToElement(valBufs[index]);
	                X1[i]= valBufs[index];
	                index++;
	            } else {
	                // padding zero
	                X1[i] = *(field->GetZero());
	            }
	        }

	        if (flag_print) {
	            for (int i = 0; i < N; i++) {
	                cout << "X1[i]" << i << " " << field->elementToString(X1[i]) << endl;
	            }
	        }

	        // x1 contains (up to) N-T values from ValBuf
	        mat.MatrixMult(X1, Y1); // no cheating: all parties have same y1

	        if (flag_print) {
	            cout << "X1[i] after mult" << endl;
	        }

	        // ‘‘Reconstruct’’ values towards some party (‘‘reconstruct’’ with degree 0)
	        if (flag_print) {
	            for (int i = 0; i < N; i++) {
	                cout << "X1[i]" << i << " " << field->elementToString(X1[i])<< endl;
	            }
	        }
	        for (int i = 0; i < N; i++) {
	           sendBufsElements[i][k] = Y1[i];
	        }
	        for (int i = 0; i < N; i++) {
	            X1[i] = *(field->GetZero());
	            Y1[i] = *(field->GetZero());
	        }
	    }

	    if (flag_print) {
	        cout << "index  2 time :" << index << '\n';

	        cout  << "before roundfunction3 " << endl;
	        for (int k=0; k< N; k++) {
	           // cout << k << "  " << buffers[k] << endl;
	        }
	    }

	    for (int i=0; i < N; i++) {
	        sendBufsBytes[i].resize(no_buckets*fieldByteSize);
	        recBufsBytesTmp[i].resize(no_buckets*fieldByteSize);
	        for (int j=0; j<no_buckets;j++) {
	            field->elementToBytes(sendBufsBytes[i].data() + (j * fieldByteSize), sendBufsElements[i][j]);
	        }
	    }

	    roundFunctionASync(sendBufsBytes, recBufsBytesTmp, 3);
	    read_to_default_buffer = false;
	    //comm->roundfunctionI(sendBufsBytes, recBufs2Bytes,3);
	    break;
	}
	case PHASE3: {
	    for (int i=0; i < N; i++) {
	        recBufsElements[i].resize((recBufsBytesTmp[i].size()) / fieldByteSize);
	        for (int j=0; j<recBufsElements[i].size();j++) {
	        	recBufsElements[i][j] = field->bytesToElement(recBufsBytesTmp[i].data() + ( j * fieldByteSize));
	        }
	    }


	    if (flag_print) {
	        cout  << "after roundfunction3 " << endl;
	        for (int k=0; k< N; k++) {
	            //cout << k << "  " << recBufs2[k] << endl;
	        }

	        cout << "no_buckets  " << no_buckets << endl;
	    }
	    FieldType temp1;

	    // no cheating: all parties have same y1
	    // ‘‘Reconstruct’’ values towards some party (‘‘reconstruct’’ with degree 0)
	    for (int k=0; k < no_buckets; k++) {
	        if (flag_print) {
	            cout << "fff  " << k<< endl;
	        }
	        if (recBufsElements[0].size() > 0) {
	            temp1 = recBufsElements[0][k];
	            //  arr.size()-1
	            for (int i = 1; i < N; i++) {
	                if (temp1 != recBufsElements[i][k]) {
	                    // cheating detected!!!
	                    if (flag_print) {
	                        cout << "                 cheating" << endl;
	                    }
	                    return false;
	                }
	            }
	        }
	    }
	    break;
	}
	default: {
		break;
	}
	}

    return true;
}

template <class FieldType>
void ProtocolParty<FieldType>::readMyInputs()
{
    ifstream myfile;
    int input;
    int i =0;
    myfile.open(inputsFile);
    do {
        myfile >> input;
        myInputs[i] = input;
        i++;
    } while(!(myfile.eof()));
    myfile.close();
    //cout<<"after read inputs" <<endl;

}

template <class FieldType>
bool ProtocolParty<FieldType>::run() {
	bool ret_val = true;

	if (should_read) {
		vector<vector<byte>> *recBufs = &recBufsBytes;
		if (!read_to_default_buffer) {
			recBufs = &recBufsBytesTmp;
		}
		for (int i = read_from_index; i < parties.size(); i++) {
			std::shared_ptr<CommPartyBF> channel = parties[i]->getChannel();

			// receive shares from the other party and set them in the shares array
			size_t read_bytes = channel->read((*recBufs)[parties[i]->getID()].data(), (*recBufs)[parties[i]->getID()].size());

			if (read_bytes != (*recBufs)[parties[i]->getID()].size()) {
				if (read_bytes != 0) {
					// todo: we have to handle this case: partial data
					cout<<"read " << read_bytes<< " bytes instead of "<<(*recBufs)[parties[i]->getID()].size()<<" - For now it is error. should be handled"<<endl;
					ret_val = false;
				}

				goto exit;
			}

			read_from_index++;
		}
		should_read = false;
		read_to_default_buffer = true;
		internal_state = (internal_state_t)(internal_state + 1);
	}

	//todo: handle more than one iteration [ for (iteration=0; iteration < times; iteration++){ ]

	switch (state) {
		case UNINITIAZLIED: {
		    for (int i = 0; i < parties.size(); i++) {
		    	std::shared_ptr<CommPartyBF> channel = parties[i]->getChannel();

		    	bool isConnected;
		    	if (!channel->checkConnectivity(&isConnected)) {
		    		ret_val = false;
		    		goto exit;
		    	}

		    	if (isConnected) {
		    		continue;
		    	}

		    	goto exit;
		    }

		    tstart_program = high_resolution_clock::now();
		    cout<<"All channels are established"<<endl;

		    state = (state_t)(state + 1);
			break;
		}

		case RUN_OFFLINE_RANDOM_SHARING:
		case RUN_OFFLINE_PREP_PHASE:
		case RUN_OFFLINE_INPUT_PREP: {
			runOffline();
			break;
		}

		case RUN_ONLINE_INPUT_ADJUSTMENT:
		case RUN_ONLINE_COMPUTATION_PHASE:
		case RUN_ONLINE_OUTPUT_PHASE: {
			runOnline();
			break;
		}

		case DONE: {
			auto t2end = high_resolution_clock::now();
			auto duration = duration_cast<milliseconds>(t2end-tstart_program).count();

			cout << "time in milliseconds for protocol: " << duration << endl;
			cout << "end main" << '\n';

			state = (state_t)(state + 1);
			break;
		}

		default: {
			ret_val = false;
			goto exit;
		}
	}

exit:
	return ret_val;
}

/**
 * This method runs the protocol:
 * Preparation Phase
 */
template <class FieldType>
void ProtocolParty<FieldType>::runOffline() {
	switch (state) {
		case RUN_OFFLINE_RANDOM_SHARING: {
			if (internal_state == PHASE0) {
				cout << "==============  RandomSharingForInputs  ==============" << '\n';
				tstart_state = high_resolution_clock::now();
			}

			bool ret = RandomSharingForInputs();

			if (internal_state == PHASE2) {
				// last phase for this state

				if(ret == false) {
					cout << "cheating!!!" << endl;
					internal_state = (internal_state_t)(internal_state + 1);
					return;
				}

				if(flag_print) {
					cout << "no cheating!!!" << '\n' << "finish RandomSharingForInputs Phase" << '\n';
				}

				high_resolution_clock::time_point t2 = high_resolution_clock::now();
				auto duration = duration_cast<milliseconds>( t2 - tstart_state ).count();
				cout << "RandomSharingForInputs took: " <<duration<<" ms"<<endl;

				// cleanings
			    for (int i=0; i<N; i++) {
			        recBufsBytes[i].clear();
			    }
			    for (int i=0; i<N; i++) {
			    	sendBufsBytes[i].clear();
			    }

				state = (state_t)(state + 1);
				internal_state = PHASE0;
			    cout << "===================================" << '\n';
			}
			break;
		}

		case RUN_OFFLINE_PREP_PHASE: {
			if (internal_state == PHASE0) {
				cout << "==============  preparationPhase  ==============" << '\n';
				tstart_state = high_resolution_clock::now();
			}

			bool ret = preparationPhase();

			if (internal_state == PHASE2) {
				// last phase for this state

				high_resolution_clock::time_point t2 = high_resolution_clock::now();
				auto duration = duration_cast<milliseconds>( t2 - tstart_state ).count();
				cout << "preparationPhase took: " <<duration<<" ms"<<endl;

			    if(ret == false) {
			        cout << "cheating!!!" << '\n';
			        internal_state = (internal_state_t)(internal_state + 1);
			        return;
			    }

				if(flag_print) {
					cout << "no cheating!!!" << '\n' << "finish Preparation Phase" << '\n';
				}

				// cleanings
			    for (int i=0; i<N; i++) {
			        recBufsBytes[i].clear();
			    }
			    for (int i=0; i<N; i++) {
			    	sendBufsBytes[i].clear();
			    }

				state = (state_t)(state + 1);
				internal_state = PHASE0;
				cout << "===================================" << '\n';
			}
			break;
		}

		case RUN_OFFLINE_INPUT_PREP: {
			if (internal_state == PHASE0) {
				cout << "==============  inputPreparation  ==============" << '\n';
				tstart_state = high_resolution_clock::now();
			}

			bool ret = inputPreparation();

			if (internal_state == PHASE1) {
				// last phase for this state

				high_resolution_clock::time_point t2 = high_resolution_clock::now();
				auto duration = duration_cast<milliseconds>( t2 - tstart_state ).count();
				cout << "inputPreparation took: " <<duration<<" ms"<<endl;

			    if(ret == false) {
			        cout << "cheating!!!" << '\n';
			        internal_state = (internal_state_t)(internal_state + 1);
			        return;
			    }

				if(flag_print) {
					cout << "no cheating!!!" << '\n' << "finish inputPreparation Phase" << '\n';
				}

				// cleanings
			    for (int i=0; i<N; i++) {
			        recBufsBytes[i].clear();
			    }
			    for (int i=0; i<N; i++) {
			    	sendBufsBytes[i].clear();
			    }

				state = (state_t)(state + 1);
				internal_state = PHASE0;
				cout << "===================================" << '\n';
			}
			break;
		}

		default: {
			cout << "ERROR" << '\n';
			return;
		}
	}
}

/**
 * This method runs the protocol:
 * Input Phase
 * Computation Phase
 * Verification Phase
 * Output Phase
 */
template <class FieldType>
void ProtocolParty<FieldType>::runOnline() {
	switch (state) {
		case RUN_ONLINE_INPUT_ADJUSTMENT: {
			if (internal_state == PHASE0) {
				cout << "==============  inputAdjustment  ==============" << '\n';
				tstart_state = high_resolution_clock::now();
			}

			string sss = "";
			bool ret = inputAdjustment(sss/*, matrix_him*/);

			if(ret == false) {
				cout << "cheating!!!" << endl;
				internal_state = (internal_state_t)(internal_state + 1);
				return;
			}

			if (internal_state == PHASE2) {
				// last phase for this state

				if(flag_print) {
					cout << "no cheating!!!" << '\n' << "finish inputAdjustment Phase" << '\n';
				}

				high_resolution_clock::time_point t2 = high_resolution_clock::now();
				auto duration = duration_cast<milliseconds>( t2 - tstart_state ).count();
				cout << "inputAdjustment took: " <<duration<<" ms"<<endl;

				// cleanings
			    for (int i=0; i<N; i++) {
			        recBufsBytes[i].clear();
			    }
			    for (int i=0; i<N; i++) {
			    	sendBufsBytes[i].clear();
			    }

				state = (state_t)(state + 1);
				internal_state = PHASE0;
			    cout << "===================================" << '\n';
			}
			break;
		}
		case RUN_ONLINE_COMPUTATION_PHASE: {
			if (internal_state == PHASE0 && currentCirciutLayer == 0) {
				cout << "==============  computationPhase  ==============" << '\n';
				tstart_state = high_resolution_clock::now();
			}

			bool ret = computationPhase(m);

			if (ret == true) {
				// last phase for this state

				high_resolution_clock::time_point t2 = high_resolution_clock::now();
				auto duration = duration_cast<milliseconds>( t2 - tstart_state ).count();
				cout << "computationPhase took: " <<duration<<" ms"<<endl;

				// cleanings
				for (int i=0; i<N; i++) {
					recBufsBytes[i].clear();
				}
				for (int i=0; i<N; i++) {
					sendBufsBytes[i].clear();
				}

				state = (state_t)(state + 1);
				internal_state = PHASE0;
				cout << "===================================" << '\n';
			}

			if (internal_state == PHASE2) {
				// cleanings
				for (int i=0; i<N; i++) {
					recBufsBytes[i].clear();
				}
				for (int i=0; i<N; i++) {
					sendBufsBytes[i].clear();
				}

				internal_state = PHASE0;
			}

			break;
		}
		case RUN_ONLINE_OUTPUT_PHASE: {
			if (internal_state == PHASE0) {
				cout << "==============  outputPhase  ==============" << '\n';
				tstart_state = high_resolution_clock::now();
			}

			bool ret = outputPhase();

			if (internal_state == PHASE1) {
				// last phase for this state

				high_resolution_clock::time_point t2 = high_resolution_clock::now();
				auto duration = duration_cast<milliseconds>( t2 - tstart_state ).count();
				cout << "outputPhase took: " <<duration<<" ms"<<endl;

			    if(ret == false) {
			        cout << "cheating!!!" << '\n';
			        internal_state = (internal_state_t)(internal_state + 1);
			        return;
			    }

				if(flag_print) {
					cout << "no cheating!!!" << '\n' << "finish outputPhase Phase" << '\n';
				}

				// cleanings
			    for (int i=0; i<N; i++) {
			        recBufsBytes[i].clear();
			    }
			    for (int i=0; i<N; i++) {
			    	sendBufsBytes[i].clear();
			    }

				state = (state_t)(state + 1);
				internal_state = PHASE0;
				cout << "===================================" << '\n';
			}

			break;
		}
		default: {
			cout << "ERROR" << '\n';
			return;
		}
	}
}

template <class FieldType>
bool ProtocolParty<FieldType>::computationPhase(HIM<FieldType> &m) {

	if (currentCirciutLayer == circuit.getLayers().size() - 1) {
		return true;
	}

	valBufField = vector<FieldType>(circuit.getLayers()[currentCirciutLayer+1]- circuit.getLayers()[currentCirciutLayer]);
	reconsBufField = vector<FieldType>(circuit.getLayers()[currentCirciutLayer+1]- circuit.getLayers()[currentCirciutLayer]);

	switch (internal_state) {
	case PHASE0: {
		indexField = 0;
		int count = processNotMult();
		if (processMultiplications(m) == -1) {
			currentCirciutLayer++;
		}

		break;
	}
	case PHASE1: {
		if (processMultiplications(m) == -1) {
			currentCirciutLayer++;
		}
		break;
	}
	case PHASE2: {
		processMultiplications(m);
		currentCirciutLayer++;
		break;
	}
	default: {
		break;
	}
	}

	return false;
}

/**
 * the function implements the second step of Input Phase:
 * the party broadcasts for each input gate the difference between
 * the random secret and the actual input value.
 * @param diff
 */
template <class FieldType>
bool ProtocolParty<FieldType>::inputAdjustment(string &diff)
{
	vector<byte> sendBufBytes;
	vector<vector<FieldType>> recBufsdiffElements(N);

	int fieldByteSize = field->getElementSizeInBytes();

	switch (internal_state) {
	case PHASE0: {
//	    cout<<"in input adjustment"<<endl;
		int input;
		int index = 0;

		vector<FieldType> diffElements;

		// read the inputs of the party

		vector<int> sizes(N);
		for (int k = 0; k < numOfInputGates; k++)
		{
			if(circuit.getGates()[k].gateType == INPUT) {
				sizes[circuit.getGates()[k].party]++;

				if (circuit.getGates()[k].party == m_partyId) {
					input = myInputs[index];
					index++;
					if (flag_print) {
						cout << "input  " << input << endl;
					}
					// the value is gateValue[k], but should be input.
					FieldType myinput = field->GetElement(input);
					if (flag_print) {
						cout << "gateValueArr " << k << "   " << field->elementToString(gateValueArr[k]) << endl;
					}

					FieldType different = myinput - gateValueArr[k];

					diffElements.push_back(different);


				}
			}
		}

		sendBufBytes.resize(diffElements.size()*fieldByteSize);
		for (int j=0; j<diffElements.size();j++) {
			field->elementToBytes(sendBufBytes.data() + (j * fieldByteSize), diffElements[j]);
		}

		if(flag_print) {
			cout << "try to print diff" << '\n';
			cout << diff << '\n';
		}

		//adjust the size of the difference we need to recieve
		for (int i=0; i<N; i++){
			//cout<< "the size of diff for " << i << " = " <<sizes[i]<<endl;
			recBufsBytes[i].resize(sizes[i]*fieldByteSize);
		}

	    // Broadcast the difference between GateValue[k] to x.
	    return broadcast(m_partyId, sendBufBytes, recBufsBytes, recBufsBytesTmp, matrix_him);

	    break;
	}
	case PHASE1: {
	    // Broadcast the difference between GateValue[k] to x.
	    return broadcast(m_partyId, sendBufBytes, recBufsBytes, recBufsBytesTmp, matrix_him);

	    break;
	}
	case PHASE2: {
	    // Broadcast the difference between GateValue[k] to x.
	    if (broadcast(m_partyId, sendBufBytes, recBufsBytes, recBufsBytesTmp, matrix_him) == false) {
	        return false;
	    }

	    if(flag_print) {
	        cout << "recBufsdiff" << endl;
	        for (int k = 0; k < N; k++) {
	           // cout << "recBufsdiff" << k << "  " << recBufsdiff[k] << endl;
	        }
	    }
	    // handle after broadcast
	    FieldType db;

	    //turn the elements to bytes
	    for(int i=0; i < N; i++)
	    {
	        recBufsdiffElements[i].resize((recBufsBytes[i].size()) / fieldByteSize);
	        for(int j=0; j<recBufsdiffElements[i].size();j++) {
	            recBufsdiffElements[i][j] = field->bytesToElement(recBufsBytes[i].data() + ( j * fieldByteSize));
	        }
	    }

	    vector<int> counters(N);

	    for(int i=0; i<N; i++){
	        counters[i] =0;
	    }

	    for (int k = 0; k < numOfInputGates; k++) {
	        if(circuit.getGates()[k].gateType == INPUT) {
	            db = recBufsdiffElements[circuit.getGates()[k].party][counters[circuit.getGates()[k].party]];
	            counters[circuit.getGates()[k].party] += 1;
	            gateShareArr[circuit.getGates()[k].output] = gateShareArr[circuit.getGates()[k].output] + db; // adjustment
	        }
	    }

	    break;
	}
	default: {
		break;
	}
	}

	return true;
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

    if(flag_print){
        cout<< "matrix_for_t : " <<endl;
        matrix_for_t.Print();

        cout<< "matrix_for_2t : " <<endl;
        matrix_for_2t.Print();
    }
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
	if(flag_print) {
		cout << "public reconstruction" << endl;
		cout << "no buckets" << no_buckets << endl;
	}
	FieldType x;
	vector<FieldType> x1(N);
	vector<FieldType> y1(N);
	vector<FieldType> y2(N);
	vector<vector<FieldType>> sendBufsElements(N);
	int fieldByteSize = field->getElementSizeInBytes();

	switch (internal_state) {
	case PHASE0: {
		for(int i = 0; i < N; i++) {
			sendBufsElements[i].resize(no_buckets);
		}
		if(flag_print) {
			for (int i = 0; i < myShares.size(); i++) {
				cout << "myShares " << i << "   " << myShares[i] << endl;
			}
		}

		// init x to be vector of degree-d (d=2*t) shares of n−t secret
		for(int k=0; k < no_buckets; k++) {
			for(int i = 0; i < N-T; i++) {
				if( k*(N-T)+i < count) {
					// k*(N-T)+i
					x1[i] = myShares[k*(N-T)+i];
				} else {
					x1[i] = *(field->GetZero());
				}
			}

			// compute y = M*x and append it to x
			m.MatrixMult(x1, y1);

			for(int i = 0; i < T; i++) {
				x1[N-T+i] = y1[i];
			}

			// ∀i, j: Pi sends xj to Pj
			for(int i = 0; i < N; i++) {
				sendBufsElements[i][k] = x1[i];
			}
		}

		if(flag_print) {
			cout << "sendBufs[i]" << endl;
			for (int i = 0; i < N; i++) {
				//cout << sendBufs[i] << endl;
			}
		}

		for(int i=0; i < N; i++) {
			sendBufsBytes[i].resize(no_buckets*fieldByteSize);
			recBufsBytes[i].resize(no_buckets*fieldByteSize);
			for(int j=0; j<no_buckets;j++) {
				field->elementToBytes(sendBufsBytes[i].data() + (j * fieldByteSize), sendBufsElements[i][j]);
			}
		}

		//cout<<"before round function 1"<<endl;
		//comm->roundfunctionI(sendBufsBytes, recBufsBytes,1);
		roundFunctionASync(sendBufsBytes, recBufsBytes,1);

		break;
	}
	case PHASE1: {
		for(int i = 0; i < N; i++) {
			sendBufsElements[i].resize(no_buckets);
		}

	    //cout<<"after round function 1"<<endl;
	    if(flag_print) {
	        cout << "recBufs[i]" << endl;
	        for(int i = 0; i < N; i++) {
	            //cout << recBufs[i] << endl;
	        }
	    }
	    //   cout << "after roundfunction1" << '\n';
	    for(int k=0; k < no_buckets; k++) {
	        for (int i = 0; i < N; i++) {
	            x1[i] = field->bytesToElement(recBufsBytes[i].data() + (k*fieldByteSize));
	        }
	        if(flag_print) {
	            cout << "x1[i]" << endl;
	            for(int i = 0; i < N; i++) {
	                cout << field->elementToString(x1[i]) << endl;
	            }
	        }

	        // checking that {xj}i are d-consistent and interpolate them to x j .
	        if (!checkConsistency(x1, d)) {
	            // halt
	            // cheating detected
	            if(flag_print) {
	                cout << "cheating" << '\n';}
	        }

	        // interpolate {xj}i to x
	        x = interpolate(x1);

	        // send x to all parties
	        for (int i = 0; i < N; i++) {
	            //sendBufs2[i] += field->elementToString(x) + "*";
	            sendBufsElements[i][k] = x;
	        }
	    }

	    for(int i=0; i < N; i++) {
	        sendBufsBytes[i].resize(no_buckets*fieldByteSize );
	        recBufsBytes[i].resize(no_buckets*fieldByteSize );
	        for(int j=0; j<no_buckets;j++) {
	            field->elementToBytes(sendBufsBytes[i].data() + (j * fieldByteSize), sendBufsElements[i][j]);
	        }
	    }

	    if(flag_print) {
	        cout << "sendBufs2[i]" << endl;
	        for(int i = 0; i < N; i++) {
	            //cout << sendBufs2[i] << endl;
	        }
	    }
	    //comm->roundfunctionI(sendBufs2Bytes, recBufs2Bytes,8);
	    roundFunctionASync(sendBufsBytes, recBufsBytes,8);
		break;
	}
	case PHASE2: {
	    if (flag_print) {
	        cout << "recBufs2[i]" << endl;
	        for(int i = 0; i < N; i++) {
	            //cout << recBufs2[i] << endl;
	        }
	    }
	    int index = 0;
	    for (int k=0; k < no_buckets; k++) {
	        for (int i = 0; i < N; i++) {
	            x1[i] = field->bytesToElement(recBufsBytes[i].data() + (k*fieldByteSize));
	        }

	        // checking that (Xn−t,...,Xn) = M*(X1,...,Xn−t)
	        m.MatrixMult(x1, y1);

	        for (int i = 0; i < T; i++) {
	            if(x1[N-T+i] != y1[i]) {
	                if(flag_print) {
	                    // halt !
	                    cout << "                  cheating" << '\n'; }
	            }
	        }

	        for (int i = 0; i < N-T; i++) {
	            if(k*(N-T)+i < count) {
	                valBuf[index] = x1[i];
	                index++;
	            }
	        }
	    }
		break;
	}
	default: {
		break;
	}
	}
}

template <class FieldType>
bool ProtocolParty<FieldType>::preparationPhase(/*VDM<FieldType> &matrix_vand, HIM<FieldType> &matrix_him*/)
{
	vector<vector<FieldType>> sendBufsElements(N);

	int robin = 0;

    // the number of random double sharings we need altogether
    int no_random = circuit.getNrOfMultiplicationGates();
    vector<FieldType> x1(N),x2(N),y1(N),y2(N);

	// the number of buckets (each bucket requires one double-sharing
	// from each party and gives N-2T random double-sharings)
	int no_buckets = (no_random / (N-2*T))+1;

	int fieldByteSize = field->getElementSizeInBytes();
	int fieldBytesSize = field->getElementSizeInBytes();

	switch (internal_state) {
	case PHASE0: {
		tstart_internal_state = high_resolution_clock::now();

	    sharingBufTElements.resize(no_buckets*(N-2*T)); // my shares of the double-sharings
	    sharingBuf2TElements.resize(no_buckets*(N-2*T)); // my shares of the double-sharings

	    for (int i=0; i < N; i++) {
	        sendBufsElements[i].resize(no_buckets*2);
	        sendBufsBytes[i].resize(no_buckets*2*field->getElementSizeInBytes());
	        recBufsBytes[i].resize(no_buckets*2*field->getElementSizeInBytes());
	    }

	    /**
	     *  generate double sharings.
	     *  first degree t.
	     *  subsequent: degree 2t with same secret.
	     */
//	    high_resolution_clock::time_point t1 = high_resolution_clock::now();
	    for(int k=0; k < no_buckets; k++) {
	        // generate random degree-T polynomial
	        for(int i = 0; i < T+1; i++) {
	            // A random field element, uniform distribution
	            x1[i] = field->Random();
	        }

	        x2[0] = x1[0];

	        for(int i = 1; i < 2*T+1; i++) {
	            // otherwise random
	            x2[i] = field->Random();
	        }

	        matrix_vand.MatrixMult(x1, y1, T+1); // eval poly at alpha-positions
	        matrix_vand.MatrixMult(x2, y2, 2*T+1); // eval poly at alpha-positions

	        // prepare shares to be sent
	        for(int i=0; i < N; i++) {
	            //cout << "y1[ " <<i<< "]" <<y1[i] << endl;
	            sendBufsElements[i][2*k] = y1[i];
	            sendBufsElements[i][2*k+1] = y2[i];
	        }
	    }//end print one

	    if (flag_print) {
	        for (int i = 0; i < N; i++) {
	            for (int k = 0; k < sendBufsElements[0].size(); k++) {

	               // cout << "before roundfunction4 send to " <<i <<" element: "<< k << " " << sendBufsElements[i][k] << endl;
	            }
	        }
	    }

	    if(flag_print) {
	        cout << "sendBufs" << endl;
	        cout << "N" << N << endl;
	        cout << "T" << T << endl;
	    }

	    for(int i=0; i < N; i++) {
	        for(int j=0; j<sendBufsElements[i].size();j++) {
	            field->elementToBytes(sendBufsBytes[i].data() + (j * fieldByteSize), sendBufsElements[i][j]);
	        }
	    }

	    roundFunctionASync(sendBufsBytes, recBufsBytes, 4);

		break;
	}

	case PHASE1: {
	    high_resolution_clock::time_point t2 = high_resolution_clock::now();
	    auto duration = duration_cast<milliseconds>( t2 - tstart_internal_state ).count();
	    cout << "preparationPhase::PHASE0 took : " <<duration<<" ms"<<endl;

	    tstart_internal_state = high_resolution_clock::now();

	    for (int i=0; i<N; i++) {
	        sendBufsElements[i].clear();
	    }

	    // x1 : used for the N degree-t sharings
	    // x2 : used for the N degree-2t sharings
	    for (int k=0; k < no_buckets; k++) {
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

	    for(int i=0; i < N; i++) {
	        sendBufsBytes[i].resize(sendBufsElements[i].size()*fieldByteSize);
	        //cout<< "size of sendBufs1Elements["<<i<<" ].size() is " << sendBufs1Elements[i].size() <<"myID =" <<  m_partyId<<endl;
	        recBufsBytes[i].resize(sendBufsElements[m_partyId].size()*fieldByteSize);
	        for(int j=0; j<sendBufsElements[i].size();j++) {
	            field->elementToBytes(sendBufsBytes[i].data() + (j * fieldByteSize), sendBufsElements[i][j]);
	        }
	    }

	    roundFunctionASync(sendBufsBytes, recBufsBytes, 5);

		break;
	}

	case PHASE2: {
	    high_resolution_clock::time_point t2 = high_resolution_clock::now();
	    auto duration = duration_cast<milliseconds>( t2 - tstart_internal_state ).count();
	    cout << "preparationPhase::PHASE1 took : " <<duration<<" ms"<<endl;

		tstart_internal_state = high_resolution_clock::now();

	    if(flag_print) {
	        for (int i = 0; i < N; i++) {
	            for (int k = 0; k < sendBufsBytes[i].size(); k++) {
	                cout << "roundfunction4 send to " <<i <<" element: "<< k << " " << (int)sendBufsBytes[i][k] << endl;
	            }
	        }
	    }

	    if(flag_print) {
	        for (int i = 0; i < N; i++) {
	            for (int k = 0; k < recBufsBytes[i].size(); k++) {
	                cout << "roundfunction4 receive from " <<i <<" element: "<< k << " " << (int) recBufsBytes[i][k] << endl;
	            }
	        }
	    }

	    /**
	     * Apply hyper-invertible matrix on each bucket.
	     * From the resulting sharings, 2T are being reconstructed towards some party,
	     * the remaining N-2T are kept as prepared sharings.
	     * For balancing, we do round-robin the party how shall reconstruct and check!
	     */
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
	        for(int i=0; i<T; i++) {
	            x_until_d[i] = x1[i];
	        }
	        for(int i=T; i<N; i++) {
	            x_until_d[i] = *(field->GetZero());
	        }
	        if(flag_print) {
	            cout << "k " << k << "interpolate(x1).toString()  " << field->elementToString(interpolate(x1)) << endl;
	            cout << "k " << k << "interpolate(x2).toString()  " << field->elementToString(interpolate(x2)) << endl;
	        }
	        // Check that x1 is t-consistent and x2 is 2t-consistent and secret is the same
	        if(!checkConsistency(x1,T) || !checkConsistency(x2,2*T) ||
	                (interpolate(x1)) != (interpolate(x2)))  {
	            // cheating detected, abort
	            if(flag_print) {
	                cout << "k" << k<< endl;
	            }
	            return false;
	        }
	    }

	    t2 = high_resolution_clock::now();
	    duration = duration_cast<milliseconds>( t2 - tstart_internal_state ).count();
	    cout << "preparationPhase::PHASE2 took : " <<duration<<" ms"<<endl;

		break;
	}
	default: {
		break;
	}
	}

    return true;
}

template <class FieldType>
bool ProtocolParty<FieldType>::RandomSharingForInputs()
{
	int robin = 0;

	// the number of random double sharings we need altogether
	int no_random = circuit.getNrOfInputGates();
	vector<FieldType> x1(N),y1(N);

	vector<vector<FieldType>> sendBufsElements(N);

	// the number of buckets (each bucket requires one double-sharing
	// from each party and gives N-2T random double-sharings)
	int no_buckets = (no_random / (N-2*T))+1;

	int fieldByteSize = field->getElementSizeInBytes();
	int fieldBytesSize = field->getElementSizeInBytes();

	switch (internal_state) {
	case PHASE0: {
		tstart_internal_state = high_resolution_clock::now();

		sharingBufInputsTElements.resize(no_buckets*(N-2*T));

		for (int i=0; i < N; i++) {
			sendBufsElements[i].resize(no_buckets);
			sendBufsBytes[i].resize(no_buckets*field->getElementSizeInBytes());
			recBufsBytes[i].resize(no_buckets*field->getElementSizeInBytes());
		}

		/**
		 *  generate double sharings.
		 *  first degree t.
		 *  subsequent: degree 2t with same secret.
		 */
		for (int k=0; k < no_buckets; k++) {
			// generate random degree-T polynomial
			for (int i = 0; i < T+1; i++) {
				// A random field element, uniform distribution
				x1[i] = field->Random();
			}

			matrix_vand.MatrixMult(x1, y1, T+1); // eval poly at alpha-positions

			// prepare shares to be sent
			for (int i=0; i < N; i++) {
				//cout << "y1[ " <<i<< "]" <<y1[i] << endl;
				sendBufsElements[i][k] = y1[i];

			}
		}//end print one

		if (flag_print) {
			for (int i = 0; i < N; i++) {
				for (int k = 0; k < sendBufsElements[0].size(); k++) {
					// cout << "before roundfunction4 send to " <<i <<" element: "<< k << " " << sendBufsElements[i][k] << endl;
				}
			}
		}

		//cout << "generate random degree-T polynomial took : " <<duration<<" ms"<<endl;

		if(flag_print) {
			cout << "sendBufs" << endl;
			cout << "N" << N << endl;
			cout << "T" << T << endl;
		}

		for (int i=0; i < N; i++) {
			for (int j=0; j<sendBufsElements[i].size();j++) {
				field->elementToBytes(sendBufsBytes[i].data() + (j * fieldByteSize), sendBufsElements[i][j]);
			}
		}

		roundFunctionASync(sendBufsBytes, recBufsBytes, 4);

		break;
	}

	case PHASE1: {
	    high_resolution_clock::time_point t2 = high_resolution_clock::now();
	    auto duration = duration_cast<milliseconds>( t2 - tstart_internal_state ).count();
	    cout << "RandomSharingForInputs::PHASE0 took : " <<duration<<" ms"<<endl;

	    tstart_internal_state = high_resolution_clock::now();

	    if (flag_print) {
			for (int i = 0; i < N; i++) {
				for (int k = 0; k < sendBufsBytes[0].size(); k++) {
					cout << "roundfunction4 send to " <<i <<" element: "<< k << " " << (int)sendBufsBytes[i][k] << endl;
				}
			}
		}

		if (flag_print) {
			for (int i = 0; i < N; i++) {
				for (int k = 0; k < recBufsBytes[0].size(); k++) {
					cout << "roundfunction4 receive from " <<i <<" element: "<< k << " " << (int) recBufsBytes[i][k] << endl;
				}
			}
		}

		/**
		 * Apply hyper-invertible matrix on each bucket.
		 * From the resulting sharings, 2T are being reconstructed towards some party,
		 * the remaining N-2T are kept as prepared sharings.
		 * For balancing, we do round-robin the party how shall reconstruct and check!
		 */

		for (int i=0; i<N; i++){
			sendBufsElements[i].clear();
		}

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
			for (int i = 2 * T; i < N; i++) {
				sharingBufInputsTElements[k*(N-2*T) + i - 2*T] = y1[i];
				//sharingBufTElements[k*(N-2*T) + i - 2*T] = y1[i];
				//sharingBuf2TElements[k*(N-2*T) + i - 2*T] =  y2[i];
			}
		}

		for(int i=0; i < N; i++) {
			sendBufsBytes[i].resize(sendBufsElements[i].size()*fieldByteSize);
			recBufsBytes[i].resize(sendBufsElements[m_partyId].size()*fieldByteSize);
			for(int j=0; j<sendBufsElements[i].size();j++) {
				field->elementToBytes(sendBufsBytes[i].data() + (j * fieldByteSize), sendBufsElements[i][j]);
			}
		}

		roundFunctionASync(sendBufsBytes, recBufsBytes, 5);

		break;
	}

	case PHASE2: {
	    high_resolution_clock::time_point t2 = high_resolution_clock::now();
	    auto duration = duration_cast<milliseconds>( t2 - tstart_internal_state ).count();
	    cout << "RandomSharingForInputs::PHASE1 took : " <<duration<<" ms"<<endl;

	    tstart_internal_state = high_resolution_clock::now();

	    int count = no_buckets * (2*T) / N; // nr of sharings *I* have to check
	    // got one in the last round
	    if (no_buckets * (2*T)%N > m_partyId) { // maybe -1
	        count++;
	    }

	    for (int k=0; k < count; k++) {
	        for (int i = 0; i < N; i++) {
	            x1[i] = field->bytesToElement(recBufsBytes[i].data() + (k*fieldBytesSize));
	        }

	        vector<FieldType> x_until_d(N);
	        for(int i=0; i<T; i++) {
	            x_until_d[i] = x1[i];
	        }
	        for(int i=T; i<N; i++) {
	            x_until_d[i] = *(field->GetZero());
	        }
	        if(flag_print) {
	            cout << "k " << k << "interpolate(x1).toString()  " << field->elementToString(interpolate(x1)) << endl;
	        }
	        // Check that x1 is t-consistent and x2 is 2t-consistent and secret is the same
	        if(!checkConsistency(x1,T) )  {
	            // cheating detected, abort
	            if(flag_print) {
	                cout << "k" << k<< endl;
	            }
	            return false;
	        }
	    }

	    t2 = high_resolution_clock::now();
	    duration = duration_cast<milliseconds>( t2 - tstart_internal_state ).count();
		cout << "RandomSharingForInputs::PHASE2 took : " <<duration<<" ms"<<endl;

		break;
	}
	default: {
		break;
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
	vector<vector<FieldType>> recBufsElements(N);
	vector<FieldType> x1(N); // vector for the shares of my inputs
	int fieldByteSize = field->getElementSizeInBytes();
	FieldType secret;

	switch (internal_state) {
	case PHASE0: {
		tstart_internal_state = high_resolution_clock::now();
		FieldType elem;
		int i;


		for(int k = 0; k < numOfInputGates; k++) { //these are only input gates
			gateShareArr[circuit.getGates()[k].output] = sharingBufInputsTElements[k];
			i = (circuit.getGates())[k].party; // the number of party which has the input
			// reconstruct sharing towards input party
		   sendBufsElements[i].push_back(gateShareArr[circuit.getGates()[k].output]);

		}
		if (flag_print) {
			cout << "sendBufs[i] in input preperation" << endl;
		}

		for (int i=0; i < N; i++) {
			sendBufsBytes[i].resize(sendBufsElements[i].size()*fieldByteSize);
			for (int j=0; j<sendBufsElements[i].size();j++) {
				field->elementToBytes(sendBufsBytes[i].data() + (j * fieldByteSize), sendBufsElements[i][j]);
			}
		}

		for (int i=0; i<N; i++){
			recBufsBytes[i].resize(sendBufsBytes[m_partyId].size());
		}

		roundFunctionASync(sendBufsBytes, recBufsBytes, 6);

		break;
	}

	case PHASE1: {
	    high_resolution_clock::time_point t2 = high_resolution_clock::now();
	    auto duration = duration_cast<milliseconds>( t2 - tstart_internal_state ).count();
	    cout << "inputPreparation::PHASE0 took : " <<duration<<" ms"<<endl;

	    tstart_internal_state = high_resolution_clock::now();

	    //turn the recbuf into recbuf of elements
	    for (int i=0; i < N; i++) {
	        recBufsElements[i].resize((recBufsBytes[i].size()) / fieldByteSize);
	        for (int j=0; j<recBufsElements[i].size();j++) {
	            recBufsElements[i][j] = field->bytesToElement(recBufsBytes[i].data() + ( j * fieldByteSize));
	        }
	    }

	    if(flag_print) {
	        for(int k = 0; k < recBufsElements[0].size(); k++) {
	            //cout << "roundfunction6 recBufs" << k << " " << recBufsElements[0][k] << endl;
	        }
	    }

	    if(flag_print) {
	        for(int k = 0; k < sendBufsElements[0].size(); k++) {
	            //cout << "roundfunction6 recBufs" << k << " " << sendBufsElements[0][k] << endl;
	        }
	    }

	    int counter = 0;
	    // reconstruct my random input values
	    for (int k = 0; k < numOfInputGates; k++) {
	        if (circuit.getGates()[k].party == m_partyId) {
	            for (int i = 0; i < N; i++) {
	                x1[i] = recBufsElements[i][counter];
	            }
	            counter++;
	            if (!checkConsistency(x1, T)) {
	                // someone cheated!
	                return false;
	            }
	            // the (random) secret
	            secret = interpolate(x1);

	            gateValueArr[k] = secret;
	            if (flag_print) {
	                cout << "           the secret is " << field->elementToString(secret) << endl;
	            }
	        }
	    }

	    t2 = high_resolution_clock::now();
	    duration = duration_cast<milliseconds>( t2 - tstart_internal_state ).count();
	    cout << "inputPreparation::PHASE1 took : " <<duration<<" ms"<<endl;

		break;
	}
	default: {
		break;
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
    if(d == T) {
        vector<FieldType> y(N - 1 - d); // the result of multiplication
        vector<FieldType> x_until_t(T + 1);

        for (int i = 0; i < T + 1; i++) {
            x_until_t[i] = x[i];
        }

        matrix_for_t.MatrixMult(x_until_t, y);

        // compare that the result is equal to the according positions in x
        for (int i = 0; i < N - d - 1; i++) { // n-d-2 or n-d-1 ??
            if ((y[i]) != (x[d + 1 + i])) {
                return false;
            }
        }
        return true;
    } else if (d == 2*T) {
        vector<FieldType> y(N - 1 - d); // the result of multiplication

        vector<FieldType> x_until_2t(2*T + 1);

        for (int i = 0; i < 2*T + 1; i++) {
            x_until_2t[i] = x[i];
        }

        matrix_for_2t.MatrixMult(x_until_2t, y);

        // compare that the result is equal to the according positions in x
        for (int i = 0; i < N - d - 1; i++) { // n-d-2 or n-d-1 ??
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
        for (int i = 0; i < N - d - 1; i++) { // n-d-2 or n-d-1 ??
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
    for(int k=circuit.getLayers()[currentCirciutLayer]; k < circuit.getLayers()[currentCirciutLayer+1]; k++) {
        // add gate
        if(circuit.getGates()[k].gateType == MULT) {
            ;//do nothing
        }
        // add gate
        else if(circuit.getGates()[k].gateType == ADD) {
            gateShareArr[circuit.getGates()[k].output] = gateShareArr[circuit.getGates()[k].input1] + gateShareArr[circuit.getGates()[k].input2];
            count++;
        } else if(circuit.getGates()[k].gateType == SUB) { //sub gate
            gateShareArr[circuit.getGates()[k].output] = gateShareArr[circuit.getGates()[k].input1] - gateShareArr[circuit.getGates()[k].input2];
            count++;
        } else if(circuit.getGates()[k].gateType == SCALAR) {
            long scalar(circuit.getGates()[k].input2);
            FieldType e = field->GetElement(scalar);
            gateShareArr[circuit.getGates()[k].output] = gateShareArr[circuit.getGates()[k].input1] * e;

            count++;
        } else if(circuit.getGates()[k].gateType == SCALAR_ADD) {
            long scalar(circuit.getGates()[k].input2);
            FieldType e = field->GetElement(scalar);
            gateShareArr[circuit.getGates()[k].output] = gateShareArr[circuit.getGates()[k].input1] + e;

            count++;
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
    FieldType p2, d2;
    FieldType r1, r2;
    FieldType d;

	switch (internal_state) {
	case PHASE0: {
	    for(int k = circuit.getLayers()[currentCirciutLayer]; k < circuit.getLayers()[currentCirciutLayer+1] ; k++) { //go over only the logit gates
	        // its a multiplication which not yet processed and ready
	        if(circuit.getGates()[k].gateType == MULT ) {
	            r1 = sharingBufTElements[shareIndex]; // t-share of random r
	            r2 = sharingBuf2TElements[shareIndex]; // t2-share of same r

	            shareIndex++;

	            p2 = gateShareArr[circuit.getGates()[k].input1] * gateShareArr[circuit.getGates()[k].input2]; // product share (degree-2t)
	            d2 = p2 - r2; // t2-share of difference
	            reconsBufField[indexField] = d2; // reconstruct difference (later)
	            indexField++;
	            // for now gateShareArr[k] is random sharing, needs to be adjusted (later)
	            gateShareArr[circuit.getGates()[k].output] = r1;
	        }
	    }

	    if(indexField == 0) {
	        return -1;
	    }
	    if(flag_print) {
	        cout <<"index for publicReconstruction " << indexField << '\n';
	    }

	    // reconstruct the differences into valBuf
	    publicReconstruction(reconsBufField, indexField, 2*T, valBufField, m);
		break;
	}
	case PHASE1: {
		// reconstruct the differences into valBuf
		publicReconstruction(reconsBufField, indexField, 2*T, valBufField, m);
		break;
	}
	case PHASE2: {
		// reconstruct the differences into valBuf
		publicReconstruction(reconsBufField, indexField, 2*T, valBufField, m);

	    int indexForValBuf = indexField-1;

	    for(int k=circuit.getLayers()[currentCirciutLayer+1]-1 ; k >= circuit.getLayers()[currentCirciutLayer]; k--) {
	        // its a multiplication which not yet processed and ready
	        if(circuit.getGates()[k].gateType == MULT) {
	            if(flag_print) {
	                cout << "indexForValBuf " << indexForValBuf << endl;
	            }
	            d = valBufField[indexForValBuf];  // the difference
	            indexForValBuf--;
	            gateShareArr[circuit.getGates()[k].output] = gateShareArr[circuit.getGates()[k].output] + d; // the adjustment
	            count++;
	        }
	    }

		break;
	}
	default: {
		break;
	}
	}

    return count;
}


/**
 * the Function process all multiplications which are ready.
 * @return the number of processed gates.
 */
template <class FieldType>
void ProtocolParty<FieldType>::processRandoms()
{
    FieldType r1;

    for(int k = (numOfInputGates - 1); k < (M - numOfOutputGates); k++) {
        if(circuit.getGates()[k].gateType == RANDOM) {
            r1 = sharingBufTElements[shareIndex];
            shareIndex++;

            gateShareArr[circuit.getGates()[k].output] = r1;
        }
    }
}

/**
 * the function Walk through the circuit and reconstruct output gates.
 * @param circuit
 * @param gateShareArr
 * @param alpha
 */
template <class FieldType>
bool ProtocolParty<FieldType>::outputPhase()
{
	int count=0;
	vector<FieldType> x1(N); // vector for the shares of my outputs
	vector<vector<FieldType>> sendBufsElements(N);
	int fieldByteSize = field->getElementSizeInBytes();

	switch (internal_state) {
	case PHASE0: {
		FieldType num;
		outputFileStream.open(outputFile);

		for(int k=M-numOfOutputGates; k < M; k++) {
			if(circuit.getGates()[k].gateType == OUTPUT) {
				// send to party (which need this gate) your share for this gate
				sendBufsElements[circuit.getGates()[k].party].push_back(gateShareArr[circuit.getGates()[k].input1]);
			}
		}

		for(int i=0; i < N; i++) {
			sendBufsBytes[i].resize(sendBufsElements[i].size()*fieldByteSize);
			recBufsBytes[i].resize(sendBufsElements[m_partyId].size()*fieldByteSize);
			for(int j=0; j<sendBufsElements[i].size();j++) {
				field->elementToBytes(sendBufsBytes[i].data() + (j * fieldByteSize), sendBufsElements[i][j]);
			}
		}

		roundFunctionASync(sendBufsBytes, recBufsBytes, 7);

		break;
	}
	case PHASE1: {
		int counter = 0;
		if(flag_print) {
			cout << "endnend" << endl;
		}
		for(int k=M-numOfOutputGates ; k < M; k++) {
			if(circuit.getGates()[k].gateType == OUTPUT && circuit.getGates()[k].party == m_partyId) {
				for(int i=0; i < N; i++) {
					x1[i] = field->bytesToElement(recBufsBytes[i].data() + (counter*fieldByteSize));
				}

				// my output: reconstruct received shares
				if (!checkConsistency(x1, T)) {
					return false;
				}
				if(flag_print_output)
					cout << "the result for "<< circuit.getGates()[k].input1 << " is : " << field->elementToString(interpolate(x1)) << '\n';

				counter++;
			}
		}

		// close output file
		outputFileStream.close();

		break;
	}
	default: {
		break;
	}
	}

	return true;
}

template <class FieldType>
void ProtocolParty<FieldType>::roundFunctionASync(vector<vector<byte>> &sendBufs, vector<vector<byte>> &recBufs, int round)
{
	recBufs[m_partyId] = sendBufs[m_partyId];
	ProtocolParty::exchangeData(sendBufs, recBufs, 0, parties.size());
	should_read = true;
	read_from_index = 0;
}

template <class FieldType>
void ProtocolParty<FieldType>::exchangeData(vector<vector<byte>> &sendBufs, vector<vector<byte>> &recBufs, int first, int last)
{
    for (int i = first; i < last; i++) {
        //send shares to my input bits
        parties[i]->getChannel()->write(sendBufs[parties[i]->getID()].data(), sendBufs[parties[i]->getID()].size());
    }
}

template <class FieldType>
void ProtocolParty<FieldType>::roundFunctionSyncBroadcast(vector<byte> &message, vector<vector<byte>> &recBufs)
{
	recBufs[m_partyId] = message;
	ProtocolParty::recData(message, recBufs, 0, parties.size());
	should_read = true;
	read_from_index = 0;
	read_to_default_buffer = true;
}

template <class FieldType>
void ProtocolParty<FieldType>::recData(vector<byte> &message, vector<vector<byte>> &recBufs, int first, int last)
{
    for (int i = first; i < last; i++) {
        //send shares to my input bits
        parties[i]->getChannel()->write(message.data(), message.size());
    }
}


template <class FieldType>
ProtocolParty<FieldType>::~ProtocolParty()
{
    delete field;
}


#endif /* PROTOCOL_H_ */
