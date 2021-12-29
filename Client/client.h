#pragma once
#ifndef CLIENT_H
#define CLIENT_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <arpa/inet.h>
#include <unistd.h>

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>

#include "../utilities.h"
#include "../net_utilities.h"

// #define PORT "3490" // the port client will be connecting to 

class client
{
    public:
    int run(char HOSTNAME[], char PORT[], char COMMANDFILE[]);

};

#endif // CLIENT_H