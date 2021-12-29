#pragma once
#ifndef UTILITIES_H
#define UTILITIES_H

#include "Server/server.h"
#include "Client/client.h"

#include <stdio.h>
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

// #define PORT "3490"  // the port users will be connecting to

#define BACKLOG 5	 // how many pending connections queue will hold
#define MAXFILESIZE 1000000 // Maximum file to read
#define ACKPCKTSIZE 8
#define HEADERSSIZE 8
#define MAXDATASIZE 504
#define MAXPCKTSIZE MAXDATASIZE + HEADERSSIZE
#define MAXFILEPATHSIZE 50 // max number of bytes we can get at once 
#define MAXPORTNUMBSIZE 20 // max number of bytes we can get at once 
#define MAXHOSTNAMESIZE 50 // max number of bytes we can get at once 

#define GET_REQUEST "GET"
#define POST_REQUEST "POST"
#define FILE_REQUEST "FILE"

#define OK_RESPONSE "HTTP/1.1 200 OK\r\n"
#define NOTFOUND_RESPONSE "HTTP/1.1 404 Not Found\r\n"
#define CONTENT_LENGTH "Content-Length: "
#define END_OF_TEXT "\0"
#define ENDREQUEST "\r\n"

#define MICROSECONDS 1000000
#define SLEEP100 usleep(100 * MICROSECONDS)
#define SLEEP10 usleep(10 * MICROSECONDS)
#define SLEEP5 usleep(5 * MICROSECONDS)
#define SLEEP1 usleep(1 * MICROSECONDS)
#define TIMEOUT 5000000

#define SLOW_START 0
#define FAST_RECOV 1
#define CONG_AVOID 2

#define DUMMY_VALUE -1
#define FIN_SEQNUM -2
#define FIN_BIT -2

/* Data-only packets */
struct packet {
    /* Header */
    uint16_t cksum = 0; /* optional bonus part */ /* 2 bytes size */
    uint16_t len = DUMMY_VALUE; /* 2 bytes size */
    uint32_t seqno = DUMMY_VALUE; /* 4 bytes size */
    /* Data */
    char data[MAXDATASIZE]; /* Not always MAXDATASIZE bytes, can be less */
};

/* Ack-only packets are only 8 bytes */
struct ack_packet {
    uint16_t cksum; /* optional bonus part */ /* 2 bytes size */
    uint16_t len; /* 2 bytes size */
    uint32_t seqno; /* 4 bytes size */
};

struct messege_content {

    char request[8]; // GET or Post
    char file_path[MAXFILEPATHSIZE];
    char host_name[MAXHOSTNAMESIZE];
    char port_number[MAXPORTNUMBSIZE];
    char request_msg[MAXDATASIZE]; // Whole request messege
    struct packet pckt;

};

class utilities
{
    public:
    void save_data_to_path(char *buffer, std::string path);
    std::string read_data_from_path(std::string);
    struct messege_content request_postprocessing(char[]);
    struct messege_content request_preprocessing(char[]);
    std::vector<struct packet> create_file_packets(std::string);
};

#endif // UTILITIES_H