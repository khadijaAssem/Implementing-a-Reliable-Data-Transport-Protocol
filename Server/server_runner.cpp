#include "server.h"

int main(int argc, char *argv[]) {
    // RUN AS : my_server portNumber random_generator_seed_value probability_of_datagram_loss
    server s;
    // All ports below 1024 are RESERVED (unless you’re the superuser)! You can have any port number above that, right up to 65535
    char PORT[] = "3490"; // set default value of PORT
    float random_generator_seed_value = 1000;
    float probability_of_datagram_loss = 0.1; //(real number in the range [0.0 , 1.0])

	if (argc < 3) {
		printf("WILL USE PORTNUMBER: %s\n", PORT);
        printf("WILL USE RANDOM GENERATOR SEED VALUE: %f\n", random_generator_seed_value);
        printf("WILL USE DATAGRAM LOSS PROBABILITY: %f\n", probability_of_datagram_loss);
		printf("IF YOU WANT TO CHANGE THE DEFAULT RUN THE FOLLOWING\n");
        printf("./my_server portNumber random_generator_seed_value probability_of_datagram_loss\n");
        printf("THANKS :)\n--------------------------------------------------------------------------------\n");
	}
    if (argc >= 3){
        strcpy(PORT, argv[1]); // get port number from user
        random_generator_seed_value = std::stof(argv[2]);
        probability_of_datagram_loss = std::stof(argv[3]);
    }

    s.run(PORT, random_generator_seed_value, probability_of_datagram_loss);
}