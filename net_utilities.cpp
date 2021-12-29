#include "net_utilities.h"

/* CONSTRUCTOR */
net_utilities::net_utilities() 
{
    /* RETURN */
}

net_utilities::net_utilities(float random_generator_seed_value, float probability_of_datagram_loss) 
{
    random_generator_seed = random_generator_seed_value;
    prob_of_datagram_loss = probability_of_datagram_loss;
    srand(random_generator_seed);

}

/* For Server */
bool net_utilities::sendAck(int sockfd, uint32_t seqNum, 
        struct sockaddr_storage toaddress, socklen_t addrlen) 
{
    printf("SENDING ACK NOW\n");

    struct ack_packet ACK;

	ACK.seqno = seqNum;
	ACK.len = ACKPCKTSIZE;
	ACK.cksum = 0;

    if (sendto(sockfd, &ACK, ACKPCKTSIZE, 0,
        (struct sockaddr *)&toaddress, addrlen) == -1) {
        printf("server: send ack error\n");
        exit(0);
    }
    return true;
}

/* For Client */
bool net_utilities::sendAck(int sockfd, uint32_t seqNum, 
        struct addrinfo *toaddress)
{
    // printf("SENDING ACK NOW\n");

    struct ack_packet ACK;

	ACK.seqno = seqNum;
	ACK.len = ACKPCKTSIZE;
	ACK.cksum = 0;

    if (sendto(sockfd, &ACK, ACKPCKTSIZE, 0,
        toaddress->ai_addr, toaddress->ai_addrlen) == -1) {
        printf("server: send error\n");
        exit(0);
    }  

    return true;
}

/* For Client */
bool net_utilities::recieveAck(int sockfd, struct addrinfo *fromaddress, struct ack_packet *ACK) 
{
    int numbytes = 0;
    if ((numbytes = recvfrom(sockfd, ACK, ACKPCKTSIZE, 0,
		fromaddress->ai_addr, &fromaddress->ai_addrlen)) == -1) {
		perror("client ACK recvfrom: ");
		// exit(1);
        return false;
	}

	if (numbytes > 0) {
		printf("Recieved ACK of seqno = %d\n", ACK->seqno);// ACK->seqno);
        return true;
        // return ACK->seqno;
	}
    return false;
}

/* For Server */
bool net_utilities::recieveAck(int sockfd, struct sockaddr_storage fromaddress, socklen_t addrlen, struct ack_packet *ACK)
{
    int numbytes = 0;
    if ((numbytes = recvfrom(sockfd, ACK, ACKPCKTSIZE, 0,
        (struct sockaddr *)&fromaddress, &addrlen)) == -1) {
        perror("server ACK recvfrom: ");
        // exit(1);
        return false;
    }

    if (numbytes > 0) {
        printf("Recieved ACK of seqno = %d\n", ACK->seqno);// ACK->seqno);
        return true;
        // return ACK->seqno;
    }

    return false;
}

std::string net_utilities::printIP(struct addrinfo *address)
{
    char ipstr[INET6_ADDRSTRLEN]; // To convert ip to string
    inet_ntop(address->ai_family, 
				get_in_addr((struct sockaddr *)&address->ai_addr), ipstr, sizeof ipstr);
    return std::string(ipstr);
}

std::string net_utilities::printIP(struct sockaddr_storage address)
{
    char ipstr[INET6_ADDRSTRLEN]; // To convert ip to string
    inet_ntop(address.ss_family,
            get_in_addr((struct sockaddr *)&address),
            ipstr, sizeof ipstr);
    return std::string(ipstr);
}

int net_utilities::createSocket(int timeoutRecieve, int timeoutSend)
{
    int sockfd;
    if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
		printf("Failed to create child server socket... close program!\n");
		exit(0);
	}
    sockfd = setTimeout(timeoutRecieve, timeoutSend, sockfd, 0);
    return sockfd;
}

int net_utilities::createSocket()
{
    int sockfd;
    if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
		printf("Failed to create child server socket... close program!\n");
		exit(0);
	}
    return sockfd;
}

/* For Server */
bool net_utilities::recievePacket(int sockfd, 
        struct sockaddr_storage *fromaddress, socklen_t addrlen, struct packet *pkt) 
{
    int numbytes;

    if ((numbytes = recvfrom(sockfd, pkt, MAXPCKTSIZE, 0, (struct sockaddr *)fromaddress, &addrlen)) == -1) {
		printf("server packet recvfrom: ");
		// exit(1);
        return false;
	}
    if (numbytes == 0) {
        pkt->seqno = DUMMY_VALUE;
        return false;
    }
    return true;
}

/* For Client */
bool net_utilities::recievePacket(int sockfd, struct addrinfo *fromaddress, struct packet *pkt)
{
    int numbytes = 0;

    if ((numbytes = recvfrom(sockfd, pkt, MAXPCKTSIZE, 0,
        fromaddress->ai_addr, &fromaddress->ai_addrlen)) == -1) {
        printf("client packet recvfrom:");
        // exit(1);
        return false;
    }
    if (numbytes == 0) {
        pkt->seqno = DUMMY_VALUE;
        return false;
    }
    return true;
}

/* For Client */
bool net_utilities::sendPacket(int sockfd, struct packet *pkt, struct addrinfo *toaddress)
{
    if (sendto(sockfd, pkt, MAXPCKTSIZE, 0,
        toaddress->ai_addr, toaddress->ai_addrlen) == -1) {
        printf("client: send error\n");
        exit(0);
    }
    return true;   
}

/* For Server */
bool net_utilities::sendPacket(int sockfd, struct packet *pkt, 
    struct sockaddr_storage *toaddress, socklen_t addrlen)
{
    if (loose_packet() && !FIRST_PACKET_SENDER) {
        FIRST_PACKET_SENDER = false;
        printf("WILL LOOSE PACKET\n");
        return false;
    }
    FIRST_PACKET_SENDER = false;
    if (sendto(sockfd, pkt, MAXPCKTSIZE, 0,
		(struct sockaddr *)toaddress, addrlen) == -1) {
		printf("server: send error\n");
    	return false;
	}

    return true;
}

int net_utilities::setTimeout(int timeoutRecieve, int timeoutSend, int sockfd, int usec)
{
    struct timeval timeout;      
    timeout.tv_sec = timeoutRecieve;
    timeout.tv_usec = usec;
    
    if (setsockopt (sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout,
                sizeof timeout) < 0)
        perror("setsockopt failed\n");

    timeout.tv_sec = timeoutSend;
    timeout.tv_usec = usec;

    if (setsockopt (sockfd, SOL_SOCKET, SO_SNDTIMEO, &timeout,
                sizeof timeout) < 0)
        perror("setsockopt failed\n");

    return sockfd;
}

/* ================================= PRIVATE ================================= */

// get sockaddr, IPv4 or IPv6:
void *net_utilities::get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

bool net_utilities::loose_packet() 
{
    float randNum = ((float)rand() / INT_MAX);
    if (randNum <= prob_of_datagram_loss){
        return true;
    }
    else
        return false;
}