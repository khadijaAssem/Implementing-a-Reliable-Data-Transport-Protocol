/*
** server.c -- a stream socket server demo
*/
#include "server.h"
#include "file_sender.h"
/*  GET /file-path HTTP/1.1
	Host: host-name:(port-number)  */


net_utilities net_utils;

int server::run(char PORT[], float random_generator_seed_value, float probability_of_datagram_loss)
{   
	// RUN AS : ./my_server portNumber random_generator_seed_value probability_of_datagram_loss
	int rv;
	struct addrinfo hints, *servinfo; // servinfo Where getaddrInfo output will be stored

	memset(&hints, 0, sizeof hints); // make sure the struct is empty
	hints.ai_family = AF_UNSPEC; // use IPv4 or IPv6, whichever (AF_INET or AF_INET6)
	hints.ai_socktype = SOCK_DGRAM; // UDP sockets
	hints.ai_flags = AI_PASSIVE; // assign the address of my local host to the socket structures

	// localhost was null but I used local host instead
	if ((rv = getaddrinfo("localhost", PORT, &hints, &servinfo)) != 0) { // Returns non zero if error
        printf("getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	int yes=1;
	int sockfd;  // listen on sock_fd (descriptor)

    struct addrinfo *serveraddress;
	// loop through all the results and bind to the first we can
	for(serveraddress = servinfo; serveraddress != NULL; serveraddress = serveraddress->ai_next) {

		sockfd = net_utils.createSocket();

        // lose the pesky "Address already in use" error message
		if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
				sizeof(int)) == -1) {
			printf("server: setsockopt error\n");
			exit(1);
		}

		if (bind(sockfd, serveraddress->ai_addr, serveraddress->ai_addrlen) == -1) {
			close(sockfd);
			printf("server: bind error\n");
			continue;
		}
        
		printf("Successful bind at: %s\n", net_utils.printIP(serveraddress).c_str());

		break;
	}

	freeaddrinfo(servinfo); // all done with this structure

    // reached the end of the linkedlist and didn't break so never binded
	if (serveraddress == NULL)  {
		printf("server: failed to bind\n");
		exit(1);
	}

	printf("server: waiting for connections...\n");
	net_utils = net_utilities(random_generator_seed_value, probability_of_datagram_loss);

	while(1) {  // main accept() loop

		struct sockaddr_storage clientaddress; // connector's address information
		socklen_t addrlen = sizeof clientaddress;

		struct packet pkt;

		net_utils.recievePacket(sockfd, &clientaddress, addrlen, &pkt);

        printf("server: got connection from %s\n", net_utils.printIP(clientaddress).c_str());

		if (pkt.seqno != DUMMY_VALUE) {
		
			net_utils.sendAck(sockfd, pkt.seqno, clientaddress, addrlen);

			file_sender handle_packet;
			utilities utils;
			pid_t pid = fork();
			if (pid == 0) { // START_IF
				pid = getpid();
				printf("%d : Child Process Handling Request\n", pid);
			    printf("File Name: %s\n", pkt.data);
				std::string fileTXT = utils.read_data_from_path("../Server/public/" + std::string(pkt.data));

				if (fileTXT == "") {
					printf("FILE NOT FOUND OR EMPTY\n");
					signal(SIGCHLD,SIG_IGN);
				}
				std::vector<struct packet> pkts = utils.create_file_packets(fileTXT);
				auto start = std::chrono::system_clock::now();
				// handle_packet.stop_and_wait(net_utils, pkts, clientaddress, addrlen);
				handle_packet.go_back_n(net_utils, pkts, clientaddress, addrlen);

				auto end = std::chrono::system_clock::now();
				printf("For probability = %f, time = %ld\n", probability_of_datagram_loss, 
					std::chrono::duration_cast<std::chrono::microseconds> (end - start).count());
				printf("-----------------------------------------------------------\n");
			} // END_IF
		}

        // ready to communicate on socket descriptor new_fd!
        // Will use send() for sending files and HTTP responses and recv() for recieving the request

	}

	return 0;
}