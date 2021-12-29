#pragma once
#ifndef SERVER_H
#define SERVER_H

#include "../utilities.h"
#include "../net_utilities.h"
// #include "file_sender.h"

class server 
{
    public:
    int run(char PORT[], float random_generator_seed_value, float probability_of_datagram_loss);
};

#endif // SERVER_H