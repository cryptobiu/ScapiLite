//
// Created by liork on 11/03/18.
//

#include "Comm.h"


/*****************************************/
/* SocketPartyData						 */
/*****************************************/
int SocketPartyData::compare(const SocketPartyData &other) const {
    string thisString = ipAddress.to_string() + ":" + to_string(port);
    string otherString = other.ipAddress.to_string() + ":" + to_string(other.port);
    return thisString.compare(otherString);
}

/*****************************************/
/* CommParty			                */
/*****************************************/

void CommParty::writeWithSize(const byte* data, int size) {
    write((const byte *)&size, sizeof(int));
    write(data, size);
}

int CommParty::readSize() {
    byte buf[sizeof(int)];
    read(buf, sizeof(int));
    int * res = (int *)buf;
    return *res;
}

size_t CommParty::readWithSizeIntoVector(vector<byte> & targetVector) {
    int msgSize = readSize();
    targetVector.resize(msgSize);
    auto res = read((byte*)&targetVector[0], msgSize);
    return res;
}

/*****************************************/
/* CommPartyTCPSynced                    */
/*****************************************/

void CommPartyTCPSynced::join(int sleepBetweenAttempts, int timeout) {
    int     totalSleep = 0;
    bool    isAccepted  = false;
    bool    isConnected = false;
    // establish connections
    while (!isConnected || !isAccepted) {
        try {
            if (!isConnected) {
                tcp::resolver resolver(ioServiceClient);
                tcp::resolver::query query(other.getIpAddress().to_string(), to_string(other.getPort()));
                tcp::resolver::iterator endpointIterator = resolver.resolve(query);
                boost::asio::connect(clientSocket, endpointIterator);
                isConnected = true;
            }
        }
        catch (const boost::system::system_error& ex)
        {
            if (totalSleep > timeout)
            {
                cerr << "Failed to connect after timeout, aborting!";
                throw ex;
            }
            cout << "Failed to connect. sleeping for " << sleepBetweenAttempts <<
                 " milliseconds, " << ex.what() << endl;
            this_thread::sleep_for(chrono::milliseconds(sleepBetweenAttempts));
            totalSleep += sleepBetweenAttempts;
        }
        if (!isAccepted) {
            boost::system::error_code ec;
            acceptor_.accept(serverSocket, ec);
            isAccepted = true;
        }
    }
    setSocketOptions();
}

void CommPartyTCPSynced::setSocketOptions() {
    boost::asio::ip::tcp::no_delay option(true);
    serverSocket.set_option(option);
    clientSocket.set_option(option);
}

void CommPartyTCPSynced::write(const byte* data, int size) {
    boost::system::error_code ec;
    boost::asio::write(clientSocket,
                       boost::asio::buffer(data, size),
                       boost::asio::transfer_all(), ec);
    if (ec)
        throw PartyCommunicationException("Error while writing. " + ec.message());
}

CommPartyTCPSynced::~CommPartyTCPSynced() {
    acceptor_.close();
    serverSocket.close();
    clientSocket.close();
}
