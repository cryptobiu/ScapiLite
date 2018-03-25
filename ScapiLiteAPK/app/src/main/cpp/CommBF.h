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
    virtual void join(int sleep_between_attempts, int timeout) = 0;
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
    virtual ~CommPartyBF() {};
};

class CommPartyTCPSyncedBoostFree : public CommPartyBF {
public:
    CommPartyTCPSyncedBoostFree(const char * self_addr, const u_int16_t self_port,
                                const char * peer_addr, const u_int16_t peer_port);
    virtual ~CommPartyTCPSyncedBoostFree();

    void join(int sleepBetweenAttempts = 500, int timeout = 5000);

    void write(const unsigned char* data, int size);
    size_t read(unsigned char* data, int sizeToRead);

private:
    std::string m_self_addr, m_peer_addr;
    u_int16_t m_self_port, m_peer_port;
    int lstn_fd, srvc_fd, clnt_fd;

    static int prep_addr(const char * addr, const u_int16_t port, struct sockaddr_in * sockaddr);
};



#endif //SCAPILITEAPK_COMMBF_H
