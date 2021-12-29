CFLAGS = -O 
CC = g++ 

all: my_client my_server
 
my_client: client.o utilities.o client_runner.o net_utilities.o
	$(CC) $(CFLAGS) -o my_client client_runner.o client.o utilities.o net_utilities.o

my_server: server.o utilities.o server_runner.o net_utilities.o file_sender.o
	$(CC) $(CFLAGS) -pthread -o my_server server_runner.o server.o utilities.o net_utilities.o file_sender.o
 
client_runner.o: ../Client/client_runner.cpp
	$(CC) $(CFLAGS) -c ../Client/client_runner.cpp 

server_runner.o: ../Server/server_runner.cpp
	$(CC) $(CFLAGS) -pthread -c ../Server/server_runner.cpp 

client.o: ../Client/client.cpp
	$(CC) $(CFLAGS) -c ../Client/client.cpp 

server.o: ../Server/server.cpp
	$(CC) $(CFLAGS) -pthread -c ../Server/server.cpp
 
utilities.o: ../utilities.cpp
	$(CC) $(CFLAGS) -c ../utilities.cpp 

net_utilities.o: ../net_utilities.cpp
	$(CC) $(CFLAGS) -c ../net_utilities.cpp 
 
file_sender.o: ../Server/file_sender.cpp
	$(CC) $(CFLAGS) -c ../Server/file_sender.cpp 

clean: 
	rm -rf *o 

 