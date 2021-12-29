#include "utilities.h"

int sequenceNumber = 0;

void utilities::save_data_to_path(char *buffer, std::string path) 
{
	std::string s = std::string(buffer);
    printf("Writitng to path %s\n",path.c_str());
	std::ofstream MyFile;
    MyFile.open(path, std::ios_base::app);
    /* Sheltaha 3alashan msh bab3at packet */
	// s = s.substr(s.find(CONTENT_LENGTH) + std::string(CONTENT_LENGTH).length());
    // int cLen = stoi(s.substr(0,s.find("\n")));
    // s = s.substr(s.find("\n")+3, cLen-1);
    // Write to the file
    MyFile << s;
    // Close the file
    MyFile.close();
}

std::string utilities::read_data_from_path(std::string path) 
{
	std::ifstream MyReadFile(path);
    if (MyReadFile.good()) {
        std::string myText;
        std::string totText;
        totText = "";
        while (getline (MyReadFile, myText))
            totText += myText  + "\n"; // Write to the file
        if (totText.length() >= 1)
			totText.substr(0, totText.length()-1);
        MyReadFile.close(); // Close the file

        return totText;
    }
    printf("FILE NOT FOUND: %s\n", path.c_str());
    return "";
}

struct messege_content utilities::request_postprocessing(char command[]) 
{
/*  GET /file-path HTTP/1.1
	Host: host-name:(port-number)  */
	/* 0 GET
	1 /file-path
	2 HTTP/1.1
	3 Host
	4 host-name
	5 (port-number) */

	struct messege_content cmd;
	strcpy(cmd.request_msg, command);
	
	int lArgs = 0;
    char *args[100];
    char *token = strtok(command, " ");
    while (token != NULL) { 
        args[lArgs++] = token;
        token = strtok(NULL, " |\n|:"); 
    } 
    args[lArgs] = NULL;
	strcpy(cmd.request, args[0]);
	strcpy(cmd.file_path, args[1]);
	strcpy(cmd.host_name, args[4]);
	strcpy(cmd.port_number, args[5]);
	return cmd;
}

struct messege_content utilities::request_preprocessing(char command[])
{

    // client_get file-path host-name (port-number)
    // client_post file-path host-name (port-number)

    int lArgs = 0;
    char *args[5];
    char *token = strtok(command, " "); 
    while (token != NULL) { 
        args[lArgs++] = token;
        token = strtok(NULL, " "); 
    } 

    args[lArgs] = NULL;
    struct messege_content cmd;
    char extra_data[MAXDATASIZE];
    char request[MAXDATASIZE];
    char *host_name = args[2], *port_number = args[3];
    char *file_path = args[1];
    if (strcmp(args[0] ,"client_get") == 0) {
        strcpy(cmd.request, GET_REQUEST);
        strcpy(request ,"GET /");
    }
    else if (strcmp(args[0] ,"client_post") == 0) {
        strcpy(cmd.request, POST_REQUEST);
        strcpy(request ,"POST /");
        strcpy(extra_data, (char *)read_data_from_path(("local/" + std::string(file_path))).c_str());
    }
    else if (strcmp(args[0] ,"client_file") == 0) {
        strcpy(cmd.request, FILE_REQUEST);
        strcpy(request ,"FILE /");
        strcpy(extra_data, (std::string(file_path).c_str()));
    }

    strcat(request,file_path); strcat(request," HTTP/1.1\n"); strcat(request,"Host: ");
    strcat(request,host_name); strcat(request,":"); strcat(request,port_number); strcat(request,"\n");

    if (strcmp(cmd.request ,POST_REQUEST) == 0) {
        strcat(request, CONTENT_LENGTH);
        int length_extra_data = std::string(extra_data).length();
        strcat(request, std::to_string(length_extra_data).c_str());
        strcat(request,ENDREQUEST); strcat(request,ENDREQUEST);
        strcat(request, extra_data);
    }
    strcat(request,ENDREQUEST);
    strcpy(cmd.file_path, file_path);
    strcpy(cmd.host_name, host_name);
    strcpy(cmd.port_number, port_number);
    strcpy(cmd.request_msg, request);

    struct packet pckt;
    // memcpy(pckt.data, "" ,MAXDATASIZE);
    pckt.seqno = sequenceNumber;
    pckt.len = MAXPCKTSIZE;
    pckt.cksum = 0;
    strcpy(pckt.data, std::string(file_path).c_str());
    printf("=================> %s\n", pckt.data);

    cmd.pckt = pckt;
    sequenceNumber += 1;
    return cmd;
}

std::vector<struct packet> utilities::create_file_packets(std::string fileTXT)
{
    printf("FILE SIZE = %ld \n", fileTXT.length());
    // Buid vector of packets
    std::vector<struct packet> pkts;

    int cnt = 0;
    for (unsigned i = 0; i < fileTXT.length(); i += (MAXDATASIZE-1)) {
        struct packet pkt;
        pkt.seqno = sequenceNumber;
        pkt.len = MAXPCKTSIZE;
        pkt.cksum = 0;
        strcpy(pkt.data, fileTXT.substr(i, (MAXDATASIZE-1)).c_str());
        pkts.push_back(pkt);
        sequenceNumber += 1;
        cnt += 1;
    }
    
    return pkts;
}