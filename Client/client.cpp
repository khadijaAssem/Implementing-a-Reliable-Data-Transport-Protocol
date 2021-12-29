/*
** client.c -- a stream socket client demo
*/

#include "client.h"

char buffer[MAXDATASIZE];
utilities util;
net_utilities net_utils;

int client::run(char HOSTNAME[], char PORT[], char COMMANDFILE[])
{
    // RUN AS : ./my_client hostname portNumber commands

    std::vector<char *> vec_of_commands;
    std::string path = "../Client/" + std::string(COMMANDFILE);
    std::string cmds = util.read_data_from_path(path);

    char cc[1000];strcpy(cc, cmds.c_str());
    char *token = strtok(cc, "\n"); 
    while (token != NULL) { 
        vec_of_commands.push_back(token);
        token = strtok(NULL, "\n"); 
    } 

    printf("Client will send\n");

    struct messege_content cmd[vec_of_commands.size()];
    for (int i=0;i<vec_of_commands.size();i++){
        printf("%s\n", vec_of_commands[i]);
        cmd[i] = util.request_preprocessing(vec_of_commands[i]);
    }

    printf("-------------------------------------------------------\n");

	int sockfd; 
	struct addrinfo hints, *servinfo;
	int rv;
	char ipstr[INET6_ADDRSTRLEN];  
    int yes=1;  

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;

	if ((rv = getaddrinfo(HOSTNAME, PORT, &hints, &servinfo)) != 0) {
		printf("getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

    struct addrinfo *clientaddress;
	// loop through all the results and bind to the first we can
	for(clientaddress = servinfo; clientaddress != NULL; clientaddress = clientaddress->ai_next) {
        // open client socket

        sockfd = net_utils.createSocket(2, 0);

        // lose the pesky "Address already in use" error message
		if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
				sizeof(int)) == -1) {
			printf("server: setsockopt error\n");
			exit(1);
		}

		printf("Client: successful bind at: %s\n", net_utils.printIP(clientaddress).c_str());

		break;
	}

    // If can't bind on clientaddress
	if (clientaddress == NULL) {
		printf("client: failed to connect\n");
		return 2;
	}

    freeaddrinfo(servinfo); // all done with this structure

    struct addrinfo *serveraddress;
    for (int i=0;i<vec_of_commands.size();i++)
    {
        /* Bafakar afta7 thread l kol command */
        /* build the server Addr structure */

        if ((rv = getaddrinfo(cmd[i].host_name, cmd[i].port_number, &hints, &serveraddress)) != 0) {
            printf("getaddrinfo: %s\n", gai_strerror(rv));
            return 1;
        }

        if (serveraddress == NULL) {
            printf("client: can't find server\n");
            return 2;
        }

        printf("Client: successful found server at: %s\n", net_utils.printIP(serveraddress).c_str());

        printf("Will send %s\n", cmd[i].pckt.data);
        printf("PKT SIZE %ld\n", sizeof cmd[i].pckt);
      
        net_utils.sendPacket(sockfd, &cmd[i].pckt, serveraddress);
        struct ack_packet ACK;
        net_utils.recieveAck(sockfd, serveraddress, &ACK);
    }

    char fileText[MAXFILESIZE];
    strcpy(fileText, "");
    int numbytes = 0;
    uint32_t expected_seq_no = 0;
    int timeouts_threshold_for_client_to_stop = 0;
    path = "../Client/local/" + std::string(cmd[0].file_path);
    // net_utils.setTimeout(sockfd, 0, 0, 1000000);
    while(true)
    {
        struct packet pkt;
        
        // Timeout
        if (!net_utils.recievePacket(sockfd, serveraddress, &pkt)) {
            printf("TIMEOUT\n");
            // break;
            timeouts_threshold_for_client_to_stop ++;
            net_utils.sendAck(sockfd, expected_seq_no - 1, serveraddress);
        }

        if (pkt.seqno == FIN_SEQNUM) {
            net_utils.sendAck(sockfd, FIN_SEQNUM, serveraddress);
            break;
        }

        if (pkt.seqno == DUMMY_VALUE) continue;

        timeouts_threshold_for_client_to_stop = 0;

        if (pkt.seqno == expected_seq_no) {
            expected_seq_no++;
            printf("RECIEVED FILE PACKET of sequence number = %d\n", pkt.seqno);
            // strcat(fileText, std::string().c_str());    
            util.save_data_to_path(pkt.data, path);
            net_utils.sendAck(sockfd, pkt.seqno, serveraddress);
        }
        else {
            net_utils.sendAck(sockfd, expected_seq_no - 1, serveraddress);
        }
    }
	close(sockfd);

	return 0;
}