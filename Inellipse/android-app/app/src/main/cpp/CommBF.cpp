//
// Created by liork on 21/03/18.
//

#include "CommBF.h"

#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <string.h>
#include <errno.h>
#include <sys/time.h>
#include <fcntl.h>

/*****************************************/
/* CommPartyBF			                */
/*****************************************/

void CommPartyBF::writeWithSize(const unsigned char* data, int size) {
    write((const unsigned char *)&size, sizeof(int));
    write(data, size);
}

int CommPartyBF::readSize() {
    unsigned char buf[sizeof(int)];
    read(buf, sizeof(int));
    int * res = (int *)buf;
    return *res;
}

size_t CommPartyBF::readWithSizeIntoVector(std::vector<unsigned char> & targetVector) {
    int msgSize = readSize();
    targetVector.resize(msgSize);
    return read((unsigned char*)&targetVector[0], msgSize);
}

/*****************************************/
/* CommPartyTCPSyncedBoostFree           */
/*****************************************/
CommPartyTCPSyncedBoostFree::CommPartyTCPSyncedBoostFree(const char * self_addr, const uint16_t self_port,
                                                         const char * peer_addr, const uint16_t peer_port,
                                                         bool is_connect_channel)
        : m_self_addr(self_addr), m_peer_addr(peer_addr), m_self_port(self_port), m_peer_port(peer_port)
        , fd1(-1), fd2(-1), m_is_connect_channel(is_connect_channel)
{
    if (m_is_connect_channel) {
        std::cout<<"Connecting channel"<<std::endl;
    } else {
        std::cout<<"Listening channel"<<std::endl;
    }
}

CommPartyTCPSyncedBoostFree::~CommPartyTCPSyncedBoostFree()
{
    if(-1 != fd1) { close(fd1); fd1 = -1; }
    if(-1 != fd2) { close(fd2); fd2 = -1; }
}

int CommPartyTCPSyncedBoostFree::prep_addr(const char * addr, const uint16_t port, struct sockaddr_in * sockaddr)
{
    memset(sockaddr, 0, sizeof(*sockaddr));
    if(inet_pton(AF_INET, addr, &sockaddr->sin_addr) == 0)
        return -1;
    sockaddr->sin_port = htons(port);
    sockaddr->sin_family = AF_INET;
    return 0;
}

void CommPartyTCPSyncedBoostFree::join()
{
    struct sockaddr_in addr;

    // socket
    fd1 = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (fd1 < 0) {
        int errcode = errno;
        char errmsg[256];
        std::cerr << "join: listener socket() failed with error [" << errcode << " : " << strerror_r(errcode, errmsg, 256) << "]" <<std::endl;
        throw errcode;
    }
    fcntl(fd1, F_SETFL, O_NONBLOCK);

    if (m_is_connect_channel) {
        // socket as a client

        //prep address structure
        if (0 != prep_addr(m_peer_addr.c_str(), m_peer_port, &addr)) {
            std::cerr << "join: prep_addr() failed converting address [" << m_peer_addr << "]" << std::endl;
            throw -1;
        }

        std::cout << "connect to " << m_peer_addr.c_str() << ":" << m_peer_port << std::endl;

        int res = connect(fd1, (struct sockaddr *)&addr, sizeof(addr));
        if (res == -1 && errno != EINPROGRESS) {
            int errcode = errno;
            char errmsg[256];
            std::cerr << "join: connect() failed with error [" << errcode << " : " << strerror_r(errcode, errmsg, 256) << "]" <<std::endl;
            throw errcode;
        }
    } else {
        // socket as a server

        //prep address structure
        if (0 != prep_addr(m_self_addr.c_str(), m_self_port, &addr)) {
            std::cerr << "join: prep_addr() failed converting address [" << m_self_addr << "]" << std::endl;
            throw -1;
        }

        if (bind(fd1, (const struct sockaddr *)&addr, (socklen_t)sizeof(struct sockaddr_in)) != 0) {
            int errcode = errno;
            char errmsg[256];
            std::cerr << "join: listener bind() failed with error [" << errcode << " : " << strerror_r(errcode, errmsg, 256) << "]" <<std::endl;
            throw errcode;
        }

        if (listen(fd1, 1) != 0) {
            int errcode = errno;
            char errmsg[256];
            std::cerr << "join: listener listen() failed with error [" << errcode << " : " << strerror_r(errcode, errmsg, 256) << "]" <<std::endl;
            throw errcode;
        }
    }
}

void CommPartyTCPSyncedBoostFree::write(const unsigned char* data, int size)
{
    int written_size = 0, written_now;
    while(size > written_size)
    {
        fd_set wfds;
        FD_ZERO(&wfds);
        FD_SET(getFD(), &wfds);

        if (0 < select(getFD() + 1, NULL, &wfds, NULL, NULL) && FD_ISSET(getFD(), &wfds)) {
            if (0 < (written_now = ::write(getFD(), data + written_size, size - written_size))) {
                written_size += written_now;
            }
        }
    }
}

size_t CommPartyTCPSyncedBoostFree::read(unsigned char* data, int sizeToRead)
{
    int read_size = 0, read_now;
    fd_set rfds;
    FD_ZERO(&rfds);
    FD_SET(getFD(), &rfds);

    if (0 < select(getFD() + 1, &rfds, NULL, NULL, NULL) && FD_ISSET(getFD(), &rfds)) {
        if (0 < (read_now = ::read(getFD(), data + read_size, sizeToRead - read_size))) {
            read_size += read_now;
        }
    }
    return (size_t)read_size;
}

int CommPartyTCPSyncedBoostFree::getFD()
{
    if (m_is_connect_channel) {
        return fd1;
    } else if (fd2 != -1) {
        // we already accept a connection
        return fd2;
    }
    return fd1;
}

bool CommPartyTCPSyncedBoostFree::checkConnectivity(bool *isConnected)
{
    bool ret_val = true;
    fd_set fdr;
    fd_set fdw;
    int res;

    FD_ZERO(&fdr);
    FD_ZERO(&fdw);
    FD_SET(getFD(), &fdr);
    FD_SET(getFD(), &fdw);

    *isConnected = false;

    res = select(getFD() + 1, &fdr, &fdw, NULL, NULL);
    if (res == -1) {
        ret_val = false;
        goto exit;
    }

    if (!m_is_connect_channel && (getFD() == fd1)) {
        // listen channel and we didn't accept a connection yet

        if (!FD_ISSET(getFD(), &fdr)) {
            goto exit;
        }

        fd2 = accept(getFD(), NULL, NULL);
        if (fd2 == -1) {
            ret_val = false;
            goto exit;
        }

        // NOTE: from now on, calls to getFD() will return fd2

        FD_ZERO(&fdr);
        FD_ZERO(&fdw);
        FD_SET(getFD(), &fdr);
        FD_SET(getFD(), &fdw);

        res = select(getFD() + 1, &fdr, &fdw, NULL, NULL);
        if (res == -1) {
            perror("select failed");
            ret_val = false;
            goto exit;
        }
    }

//	if ((m_is_connect_channel && !FD_ISSET(getFD(), &fdw)) || (!m_is_connect_channel && !FD_ISSET(getFD(), &fdr))) {
//		goto exit;
//	}

    if (!FD_ISSET(getFD(), &fdw)) {
        goto exit;
    }

    *isConnected = true;

    exit:
    return ret_val;
}

uint16_t CommPartyTCPSyncedBoostFree::getSelfPort()
{
    return m_self_port;
}