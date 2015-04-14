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
#include <sys/wait.h>
#include <map>
#include <string>
#include <iostream>

using namespace std;

int main () {
	//initialize the variables and structs
	map<string, string> dataSet; 
	struct addrinfo hints;
	struct addrinfo* server1info; 
	int clientno = 1;

	//read data from server1.txt and store in Map
	//this converts data in .txt file to standard input
	freopen("server1.txt", "r", stdin);

	//read into map
	while (!cin.fail()) {
		string key, value;
		cin >> key >> value;
		dataSet.insert(make_pair(key, value));
	}

	//----------------------- Taken from Beej's Guide -----------------------//

	memset(&hints, 0, sizeof hints); // make sure the struct is empty

	//set the hints in the hints struct (we want the UDP socket)
	hints.ai_socktype = SOCK_DGRAM;	//we need UDP
	hints.ai_family = AF_UNSPEC; //don't care what kind
	hints.ai_flags = AI_PASSIVE; 	//hopefully this will fill in nunki's IP here
	
	//-------------------------------------------------------------------------//

	//set the hints to get a datagram socket
	hints.ai_socktype = SOCK_DGRAM;

	//get the server listening on port 21564
	int status = getaddrinfo(NULL, "21564", &hints, &server1info);
	if (status != 0) {
		cout << "Failed to retrieve server1:getaddrinfo" << endl;
		return 1;
	}

	//NOTE: all of the code below has to run twice - 
	//we can either terminate it after two runs or have it run constantly until ctrl+C is hit
	//i chose to have it run indefinitely 

	while (true) {
		//create socket and get socket descriptor
		int sockfd = socket(server1info->ai_family, server1info->ai_socktype, server1info->ai_protocol);
		if (sockfd == -1) {
			cout << "Failed to create server1:UDPsocket" << endl;
			return 1;
		}

		//bind the socket to port 21564
		int bindStat = bind(sockfd, server1info->ai_addr, server1info->ai_addrlen);
		if (bindStat == -1) {
			cout << "Failed to bind server1:UDPsocket" << endl;
			return 1;
		}

		//----------------------- Taken from Beej's Guide -----------------------//

		//create a new storage struct
		struct sockaddr_storage new_addr;
		socklen_t addr_size= sizeof new_addr;
	
		//-------------------------------------------------------------------------//

		//UDP socket so we don't have a connect or accept function
		//recieve data
		char* buf[9];
		int recStat = recv(sockfd, buf, 9, 0);
		if (recStat == -1) {
			cout << "Failed to receive server1:recv" << endl;
			return 1;
		}

		cout << buf << endl;

		close (sockfd);

	}


	return 0;
}
