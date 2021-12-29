#pragma once
#ifndef NET_UTILITIES_H
#define NET_UTILITIES_H


#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <math.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>

#include <pthread.h>
#include <algorithm>
#include <string>
#include <iostream>
#include <fstream>
#include <chrono>
#include <ctime>    
#include <cmath> 

#include "utilities.h"

class net_utilities 
{
    public:
    net_utilities();
    net_utilities(float random_generator_seed_value, float probability_of_datagram_loss);
    bool sendAck(int sockfd, uint32_t seqNum, 
        struct sockaddr_storage toaddress, socklen_t addrlen); // For Server
    bool sendAck(int sockfd, uint32_t seqNum, 
        struct addrinfo *toaddress); // For Client

    bool recieveAck(int sockfd, struct addrinfo *fromaddress, struct ack_packet *ACK); // For Client
    bool recieveAck(int sockfd, struct sockaddr_storage fromaddress, socklen_t addrlen,
        struct ack_packet *ACK); // For Server

    bool recievePacket(int sockfd, struct sockaddr_storage *fromaddress, 
        socklen_t addrlen, struct packet *pkt); // For Server
    bool recievePacket(int sockfd, struct addrinfo *fromaddress, struct packet *pkt); // For Client

    std::string printIP(struct addrinfo *address); // convert IP address to string
    std::string printIP(struct sockaddr_storage address);

    bool sendPacket(int sockfd, struct packet *pkt, struct addrinfo *toaddress); // For Client
    bool sendPacket(int sockfd, struct packet *pkt, 
        struct sockaddr_storage *toaddress, socklen_t addrlen); // For Server

    int createSocket(); // Create socket with timeout
    int createSocket(int timeoutRecieve, int timeoutSend);
    int setTimeout(int timeoutRecieve, int timeoutSend, int sockfd, int usec);

    private:
    bool FIRST_PACKET_SENDER = true;
    float random_generator_seed = 0;
    float prob_of_datagram_loss = 0;
    // get sockaddr, IPv4 or IPv6:
    void *get_in_addr(struct sockaddr *sa);
    bool loose_packet();
};

#endif // NET_UTILITIES_H