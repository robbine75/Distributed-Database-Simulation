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

string tcpServer2 (string rec);

int main () {

	cout << "===================================================== \n" << endl; 


	//============================= INIT. ==================================//

	//initialize the variables and structs
	map<string, string> dataSet; 
	struct addrinfo hints;
	struct addrinfo* server1info; 
	int clientno = 0;

	//======================================================================//

	//======================= READING DATA FROM TXT =========================//

	//read data from server1.txt and store in Map
	//this converts data in .txt file to standard input
	freopen("server1.txt", "r", stdin);

	//read into map
	while (!cin.fail()) {
		string key, value;
		cin >> key >> value;
		dataSet.insert(make_pair(key, value));
	}

	//======================================================================//


	//========================= IP AND SERVER INFO ========================//
	//-------------------- Borrowed from Beej's Guide ---------------------//

	memset(&hints, 0, sizeof hints); // make sure the struct is empty

	//set the hints in the hints struct (we want the UDP socket)
	hints.ai_socktype = SOCK_DGRAM;	//we need UDP
	hints.ai_family = AF_INET; //don't care what kind
	
	//get the server listening on port 21564
	int status = getaddrinfo("localhost", "21564", &hints, &server1info);
	if (status != 0) {
		cout << "Failed to retrieve server1:getaddrinfo" << endl;
		return 1;
	}

	char* s1IP = inet_ntoa(((struct sockaddr_in*)(server1info->ai_addr))->sin_addr);


	cout << "Server 1 has UDP port number " << ntohs(((struct sockaddr_in*)(server1info->ai_addr))->sin_port); 
	cout << " and IP address " << s1IP << endl;
	cout << "\n";

	//======================================================================//

		//========================== SOCKET CREATION ==========================//

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

		//======================================================================//


	//======================== CLIENT COMMUNICATION ========================//

	//NOTE: all of the code below has to run twice - 
	//we can either terminate it after two runs or have it run constantly until ctrl+C is hit
	//i chose to have it run indefinitely 

	while (true) {

		clientno++;

		//============================= RECV. DATA ============================//

		//UDP socket so we don't have a connect or accept function
		//recieve data into buffer and store client information in storage struct
		char buf[9];
		struct sockaddr_in clientinfo;
		socklen_t addr_sz = sizeof clientinfo;

		int recStat = recvfrom(sockfd, buf, 9, 0, (struct sockaddr *)&clientinfo, &addr_sz);
		if (recStat == -1) {
			cout << "Failed to receive server1:recv" << endl;
			return 1;
		}

		buf[9] = '\0';
		string rec(buf);
		string result = "";

		cout << "Server 1 has received a request with key " << buf << " ";		
		cout << "from Client " << clientno << " with port number " << ntohs(((struct sockaddr_in*)(&clientinfo))->sin_port);
		cout << " and IP address " << inet_ntoa(((struct sockaddr_in*)(&clientinfo))->sin_addr) << endl;
		cout << "\n";


		//======================================================================//


		//================================ POST ================================//

		//find the value in the GET request
		if (rec.substr(0, 3) == "GET") {

			rec = rec.substr(4, rec.length());	

			//search for the key in the data map
			map<string, string>::iterator find = dataSet.find(rec);

			//if server 1 has it, it will reply with the POST response
			bool serv2 = false;
			if (find != dataSet.end()) {
				result = "POST " + (find->second);
				serv2 = false;
			}

			//if it doesn't have it, we must communicate with Server 2 
			else {
				result = tcpServer2(rec);
				serv2 = true;
				//add the key-value pair to server1's database
				string addKey = result.substr(5, result.length());
				dataSet.insert(make_pair(rec, addKey));
			}

			//Now send result back to the client

			const char* sendMsg = result.c_str();
			int len = strlen(sendMsg);

			int sendStat = sendto(sockfd, sendMsg, len, 0, (struct sockaddr *)&clientinfo, addr_sz);
			if (sendStat == -1) {
				cout << "Failed to send server1:send" << endl;
				return 1;
			}
			else if (sendStat != len) {
				cout << "Not all bytes sent server1:sendto" << endl;
				return 1;
			}

			if (!serv2) {
				cout << "Server 1 sends the reply " << sendMsg << " to Client " << clientno;
				cout << " with port number " << ntohs(((struct sockaddr_in*)(&clientinfo))->sin_port);
				cout << " and IP address " << inet_ntoa(((struct sockaddr_in*)(&clientinfo))->sin_addr) << endl;
				cout << "\n";
			}
			else {
				cout << "Server 1 sent reply " << sendMsg << " to Client " << clientno;
				cout << " with port number " << ntohs(((struct sockaddr_in*)(&clientinfo))->sin_port);
				cout << " and IP address " << inet_ntoa(((struct sockaddr_in*)(&clientinfo))->sin_addr) << endl;
				cout << "\n";
			}
		}

		cout << "===================================================== \n" << endl; 

		//======================================================================//

	}

	close (sockfd);


	return 0;
}

string tcpServer2 (string rec) {

	string fail = "";

	//to communicate with server 2, server 1 sets up a TCP connection w/ server 2

	//========================= IP AND SERVER INFO ========================//
	//-------------------- Borrowed from Beej's Guide ---------------------//

	struct addrinfo hints2;
	struct addrinfo *server2info;

    memset(&hints2, 0, sizeof hints2); // make sure the struct is empty
    hints2.ai_family = AF_INET;
    hints2.ai_socktype = SOCK_STREAM;

	//get addrinfo for server1
	//struct serverinfo will store the information
	int status = getaddrinfo("localhost", "22564", &hints2, &server2info);
	if (status != 0) {
		cout << "Failed to retrieve server1:getaddrinfo" << endl;
		return fail;
	}

	char* IPs2 = inet_ntoa(((struct sockaddr_in*)(server2info->ai_addr))->sin_addr);

	//======================================================================//

	//========================== SOCKET CREATION ==========================//

	//create socket and get socket descriptor
	int socktcp = socket(AF_INET, SOCK_STREAM, 0);
	if (socktcp == -1) {
		cout << "Failed to create server1:TCPsocket" << endl;
		return fail;
	}

	//========================== SOCKET BINDING ==========================//

	struct sockaddr_in c;

	//------- CODE BORROWED FROM https://www.cs.rutgers.edu/~pxk/417/notes/sockets/index.html ------//

	memset((char *)&c, 0, sizeof(c));
	c.sin_family = AF_INET;
	c.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	c.sin_port = htons(0);
	socklen_t addr_len= sizeof c;

	//----------------------------------------------------------------------------------------------//

	//bind the socket to port 22564
	int bindStat = bind(socktcp, (struct sockaddr *)&c, addr_len);
	if (bindStat == -1) {
		cout << "Failed to bind server1:TCPsocket" << endl;
		return fail;
	}

	//============================ GETSOCKNAME ============================//

	struct sockaddr_in store;
	socklen_t storesize = sizeof store;

	int getsockstat = getsockname(socktcp, (struct sockaddr *)&store, &storesize);
	if (getsockstat == -1) {
		cout << "Failed to retrieve port number server1:getsockname" << endl;
		return fail;
	}

	//======================================================================//

	//======================================================================//


	//=========================== CONNECT TO S2 ============================//

	//establish a TCP connection with server 2

	int connStatus = connect(socktcp, (struct sockaddr *)(server2info->ai_addr), server2info->ai_addrlen);
	if (connStatus == -1) {
		cout << "Failed to connect server1:TCPsocket" << endl;
		return fail;
	}

	//======================================================================//


	//============================== SEND ==================================//

	//if server 2 accepts connection, we can begin sending it a GET request

	string strmsg = "GET " + rec;
	const char* msg = strmsg.c_str();
	int len = strlen(msg);

	int sendStatTCP = write(socktcp, msg, len);
	if (sendStatTCP == -1) {
		cout << "Failed to send server1:write" << endl;
		return fail;
	}
	else if (sendStatTCP != len) {
		cout << "Not all bytes sent server1:write" << endl;
		return fail;
	}

	cout << "Server 1 sends the request " << msg << " to Server 2" << endl;
	cout << "\n";

	cout << "The TCP port number is " << ntohs(((struct sockaddr_in*)(&store))->sin_port);
	cout << " and the IP address is " << inet_ntoa(((struct sockaddr_in*)(&c))->sin_addr) << endl;
	cout << "\n";

	//======================================================================//

	//============================ RECEIVE =================================//

	char res[12];

	int readStat = read(socktcp, res, 12);
	if (readStat == -1) {
		cout << "Failed to receive server1:read" << endl;
		return fail;
	}
	else if (readStat == 0) {
		cout << "Closing server1:read" << endl;
		close(socktcp);
		return fail;
	}
	else if (readStat != int(strlen(res))) {
		cout << "Not all bytes received server1:read" << endl;
		return fail;
	}

	cout << "Server 1 receieved the value " << res << " from Server 2 ";
	cout << "with port number " << ntohs(((struct sockaddr_in*)(server2info->ai_addr))->sin_port);
	cout << " and IP address " << IPs2 << endl;
	cout << "\n";

	//======================================================================//

	res[12] = '\0';
	string result(res);

	//CLOSE SOCKET
	close(socktcp);

	cout << "Server 1 closed the connection with Server 2 \n" << endl;

	return result;

}
