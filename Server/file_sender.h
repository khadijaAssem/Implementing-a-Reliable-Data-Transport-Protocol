#pragma once
#ifndef FILE_SENDER_H
#define FILE_SENDER_H

#include "../utilities.h"
#include "../net_utilities.h"

#define SLOW_START 0
#define FAST_RECOV 1
#define CONG_AVOID 2


class file_sender 
{
    public:
    bool go_back_n(net_utilities net_utils, std::vector<struct packet> &pkts, 
        struct sockaddr_storage clientaddress, socklen_t addrlen);
    bool stop_and_wait(net_utilities net_utils, std::vector<struct packet> &pkts, 
        struct sockaddr_storage clientaddress, socklen_t addrlen);
    
    private: 
    void fin_packet(net_utilities net_utils, int childSocket,
        struct sockaddr_storage clientaddress, socklen_t addrlen);
    void write_to_analysis_file(std::vector<int>& cwnd_sizes);
};

#endif // FILE_SENDER_H