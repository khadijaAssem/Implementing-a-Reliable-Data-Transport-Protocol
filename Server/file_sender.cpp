#include "file_sender.h"

utilities utils;


bool file_sender::go_back_n(net_utilities net_utils, std::vector<struct packet> &pkts, 
    struct sockaddr_storage clientaddress, socklen_t addrlen) 
{
    uint32_t MSS = 1, cwnd_base = 0, past_ACK = DUMMY_VALUE,
						 duplicate_acks = 0, last_sent_packet = 0;
    uint32_t cwnd = 1 * MSS;
    int ssthresh = INT16_MAX; // 64K
    
    int childSocket = net_utils.createSocket(2, 0);
    bool done = false;
    int state = SLOW_START;
    // Start transmitting file
    int loop = 1;
    std::vector<int> cwnd_sizes;
    while (!done) {
        loop++;
        cwnd_sizes.push_back(cwnd);
        printf("CWND = %d, CWND_BASE = %d\n", cwnd, cwnd_base);
        if (last_sent_packet < (cwnd + cwnd_base) && last_sent_packet<pkts.size())
            printf("Sending from %d to %d\n", last_sent_packet, (cwnd + cwnd_base));
        // Send from base to cwnd size 
        for (last_sent_packet; last_sent_packet < (cwnd + cwnd_base) && last_sent_packet<pkts.size(); last_sent_packet++) {
            printf("Will send packet %d\n", last_sent_packet);
            if (net_utils.sendPacket(childSocket, &pkts[last_sent_packet], &clientaddress, addrlen))
                printf("Sent packet %d\n", last_sent_packet);
        }

        struct ack_packet ACK;
        bool recieved = net_utils.recieveAck(childSocket, clientaddress, addrlen, &ACK);

        // TIMEOUT
        if (!recieved) {
            printf("TIMEOUT\n");
            duplicate_acks = 0;
            ssthresh = cwnd/2;
            if (state == SLOW_START) {
                cwnd = 1 * MSS;
            }
            else if (state == FAST_RECOV) {
                cwnd = 1;
                state = SLOW_START;
            }
            else if (state == CONG_AVOID) {
                cwnd = 1 * MSS;
                state = SLOW_START;
            }
            last_sent_packet = cwnd_base - 1;
            continue;
        }
        
        // DUPLICATE_ACKS
        if (ACK.seqno == past_ACK) {
            printf("Recieved duplicate ACK\n");
            if (state == SLOW_START || state == CONG_AVOID) {
                duplicate_acks ++;
                if (duplicate_acks == 3) {
                    // Three duplicate acks
                    printf("Three duplicate ACK\n");
                    ssthresh = cwnd/2;
                    cwnd = ssthresh + 3 * MSS;
                    state = FAST_RECOV;
                    last_sent_packet = cwnd_base - 1;
                }

            }
            else if (state == FAST_RECOV) {
                // cwnd = cwnd + MSS;
                ssthresh = cwnd/2;
                cwnd = ssthresh + 1;
                last_sent_packet = cwnd_base - 1;
                duplicate_acks = 0;
            }
            continue;
        }
        
        // LAST_ACK_RECIEVED
        if (ACK.seqno == pkts[pkts.size()-1].seqno) {
            done = true;
            printf("Last packet and ACK done\n");
            break;
        }
        
        // NEW_ACK
        printf("Recieved right ACK\n");
        duplicate_acks = 0; // FOR ALL STATES
        past_ACK = ACK.seqno;
        cwnd_base  = ACK.seqno + 1;

        if (state == SLOW_START) {
            cwnd = cwnd + MSS;
            if (cwnd >= ssthresh) {
                state = CONG_AVOID;
                printf("switched to congestion avoidance\n");
            }
        }
        else if (state == CONG_AVOID) {
            cwnd = cwnd + MSS * (MSS/cwnd);
            // cwnd += cwnd;
            // cwnd += 1;
        }
        else if (state == FAST_RECOV) {
            cwnd = ssthresh;
            state = CONG_AVOID;
        }					
    } // END_WHILE
    printf("Closing child socket\n");
    printf("loops = %d", loop);
    write_to_analysis_file(cwnd_sizes);
    printf("\n");
    fin_packet(net_utils, childSocket, clientaddress, addrlen);
    close(childSocket);
    return true;
}

bool file_sender::stop_and_wait(net_utilities net_utils, std::vector<struct packet> &pkts, 
    struct sockaddr_storage clientaddress, socklen_t addrlen) 
{
    int childSocket = net_utils.createSocket(3, 0);
    int base = 0;

    while (base < pkts.size()) {
        if (net_utils.sendPacket(childSocket, &pkts[base], &clientaddress, addrlen)) {
            printf("Sent packet %d\n", base);
        }
        struct ack_packet ACK;
        bool recieved = net_utils.recieveAck(childSocket, clientaddress, addrlen, &ACK);

        /* TIMEOUT */
        if(!recieved || ACK.seqno == DUMMY_VALUE || ACK.seqno < base){
            continue;
        }
        base ++;
        if (base == pkts.size()) {
            printf("Last packet and ACK done\n");
            break;
        }
    }
    fin_packet(net_utils, childSocket, clientaddress, addrlen);
    printf("Closing child socket\n");
    close(childSocket);
    return true;
}
/* ================================= PRIVATE ================================= */
void file_sender::fin_packet(net_utilities net_utils, int childSocket,
    struct sockaddr_storage clientaddress, socklen_t addrlen) 
{
    struct packet fin_packet;
    fin_packet.seqno = FIN_SEQNUM;
    fin_packet.cksum = 0;
    fin_packet.len = MAXPCKTSIZE;

    if (net_utils.sendPacket(childSocket, &fin_packet, &clientaddress, addrlen)) {
        printf("Sent fin packet\n");
    }

    struct ack_packet ACK;
    bool recieved = net_utils.recieveAck(childSocket, clientaddress, addrlen, &ACK);

    /* TIMEOUT */
    if(!recieved ){ // || ACK.seqno != FIN_SEQNUM
        file_sender::fin_packet(net_utils, childSocket, clientaddress, addrlen);
    }

}

void file_sender::write_to_analysis_file(std::vector<int>& cwnd_sizes) 
{
    std::ofstream MyFile;
    MyFile.open("Analysis.txt", std::ios_base::app);
    // Write to the file
    for(auto cwnd : cwnd_sizes)
        MyFile << std::string(std::to_string(cwnd)) + ",";
    // Close the file
    MyFile.close();
}