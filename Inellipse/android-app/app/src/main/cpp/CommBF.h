//
// Created by liork on 21/03/18.
//

#ifndef SCAPILITEAPK_COMMBF_H
#define SCAPILITEAPK_COMMBF_H

#include <string>
#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>


/**
* A simple interface that encapsulate all network operations of one peer in a two peers (or more)
* setup.
*/
class CommPartyBF {
public:
    /**
    * This method setups a double edge connection with another party.
    * It connects to the other party, and also accepts connections from it.
    * The method blocks until boths side are connected to each other.
    */
    virtual void join() = 0;
    /**
    * Write data from @param data to the other party.
    * Will write exactly @param size bytes
    */
    virtual void write(const unsigned char * data, int size) = 0;
    /**
    * Read exactly @param sizeToRead bytes int @param buffer
    * Will block until all bytes are read.
    */
    virtual size_t read(unsigned char* buffer, int sizeToRead) = 0;
    virtual void write(std::string s) { write((const unsigned char *)s.c_str(), s.size()); };
    virtual void writeWithSize(const unsigned char* data, int size);
    virtual int readSize();
    virtual size_t readWithSizeIntoVector(std::vector<unsigned char> & targetVector);
    virtual void writeWithSize(std::string s) { writeWithSize((const unsigned char*)s.c_str(), s.size()); };
    virtual bool checkConnectivity(bool *isConnected)=0;
    virtual uint16_t getSelfPort()=0;
    virtual ~CommPartyBF() {};
};

class CommPartyTCPSyncedBoostFree : public CommPartyBF {
public:
    CommPartyTCPSyncedBoostFree(const char * self_addr, const uint16_t self_port,
                                const char * peer_addr, const uint16_t peer_port,
                                bool is_connect_channel);
    virtual ~CommPartyTCPSyncedBoostFree();

    void join();

    void write(const unsigned char* data, int size);
    size_t read(unsigned char* data, int sizeToRead);

    int getFD();
    bool checkConnectivity(bool *isConnected);

    uint16_t getSelfPort();

private:
    std::string m_self_addr, m_peer_addr;
    uint16_t m_self_port, m_peer_port;
    int fd1, fd2;
    bool m_is_connect_channel;

    static int prep_addr(const char * addr, const uint16_t port, struct sockaddr_in * sockaddr);
};



#endif //SCAPILITEAPK_COMMBF_H
