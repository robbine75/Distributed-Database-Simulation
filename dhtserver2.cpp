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

string tcpServer3 (string request);

int main () {

	cout << "===================================================== \n" << endl; 

	//======================= READING DATA FROM TXT =========================//

	map<string, string> dataSet; 

	//read data from server1.txt and store in Map
	//this converts data in .txt file to standard input
	freopen("server2.txt", "r", stdin);

	//read into map
	while (!cin.fail()) {
		string key, value;
		cin >> key >> value;
		dataSet.insert(make_pair(key, value));
	}

	//======================================================================//


	//========================== SOCKET CREATION ==========================//

	//first we'll have to create a socket
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1) {
		perror("Failed to create server2:socket");
		return 1;
	}

	//------- CODE BORROWED FROM https://www.cs.rutgers.edu/~pxk/417/notes/sockets/index.html ------//
	struct sockaddr_in c;
	memset((char *)&c, 0, sizeof(c));
	c.sin_family = AF_INET;
	c.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	c.sin_port = htons(22564);
	socklen_t addr_len= sizeof c;

	//----------------------------------------------------------------------------------------------//

	//bind the socket to port 22564
	int bindStat = bind(sockfd, (struct sockaddr *)&c, addr_len);
	if (bindStat == -1) {
		perror("Failed to bind server2:TCPsocket");
		return 1;
	}

	//======================================================================//

	cout << "Sever 2 has TCP port number " << ntohs(((struct sockaddr_in*)(&c))->sin_port);
	cout << " and IP address " << inet_ntoa(((struct sockaddr_in*)(&c))->sin_addr) << endl;
	cout << "\n";


	//============================ SOCKET LISTEN ===========================//

	//backlog = 3 is good enough for this project
	int lis = listen(sockfd, 3);
	if (lis == -1) {
		perror("Failed to listen server2:listen");
		return 1;
	}

	//======================================================================//

	//set up connection with all the incoming requests
	//we're going to have to do all of these steps more than once (possibly)
	while (true) {

		//=============================== ACCEPT ===============================//

		struct sockaddr_in incoming;
		socklen_t clientlen = sizeof incoming;

		int newSockfd = accept(sockfd, (struct sockaddr *)&incoming, &clientlen);
		if (newSockfd == -1) {
			perror("Failed to accept server2:accept");
			return 1;
		}

		//======================================================================//


		//================================ READ ================================//

		char reqMsg[9];

		int readbytes = read(newSockfd, reqMsg, 9);
		if (readbytes == -1) {
			perror("Failed to receive server2:read");
			return 1;
		}
		else if (readbytes == 0) {
			perror("Closing server2:read");
			close(newSockfd);
			return 0;
		}
		else if (readbytes != int(strlen(reqMsg))) {
			perror("Not all bytes received server2:read");
			return 1;
		}

		reqMsg[9] = '\0';

		cout << "Server 2 has received a request with key " << reqMsg << " ";		
		cout << "from Server 1 with port number " << ntohs(((struct sockaddr_in*)(&incoming))->sin_port);
		cout << " and IP address " << inet_ntoa(((struct sockaddr_in*)(&incoming))->sin_addr) << endl;
		cout << "\n";

		//======================================================================//


		//=========================== LOOKUP AND WRITE ==========================//

		string req(reqMsg);
		req = req.substr(4, req.length());
		
		//look up request in map
		map<string, string>::iterator find = dataSet.find(req);
		string result;
		bool serv3 = false;

		if (find != dataSet.end()) {
			//this means server 2 has the value in it's tables and can now send it back to server 1
			result = "POST " + (find->second);
			serv3 = false;
		}

		else {
			result = tcpServer3(req);
			serv3 = true;
			//Add new key-value pair to database
			string addKey = result.substr(5, result.length());
			dataSet.insert(make_pair(req, addKey));
		}

		const char* msg = result.c_str();
		int len = strlen(msg); 

		int writebytes = write(newSockfd, msg, len);
		if (writebytes == -1) {
			perror("Failed to send server2:write");
			return 1;
		}
		else if (writebytes != len) {
			perror("Not all bytes sent server2:write");
			return 1;
		}

		if (!serv3) {
			cout << "Server 2 sends the reply " << msg << " to Server 1";
			cout << " with port number " << ntohs(((struct sockaddr_in*)(&incoming))->sin_port);
			cout << " and IP address " << inet_ntoa(((struct sockaddr_in*)(&incoming))->sin_addr) << endl;
			cout << "\n";
		}
		else {
			cout << "Server 2 sent reply " << msg << " to Server 1";
			cout << " with port number " << ntohs(((struct sockaddr_in*)(&incoming))->sin_port);
			cout << " and IP address " << inet_ntoa(((struct sockaddr_in*)(&incoming))->sin_addr) << endl;
			cout << "\n";
		}

		cout << "===================================================== \n" << endl; 

		//close(newSockfd);

		//======================================================================//

	}	

	return 0;
}

string tcpServer3 (string request) {
	string fail = "";

	//to communicate with server 3, server 2 sets up a TCP connection w/ server 2

	//========================= IP AND SERVER INFO ========================//
	//-------------------- Borrowed from Beej's Guide ---------------------//

	struct addrinfo hints;
	struct addrinfo *server3info;

    memset(&hints, 0, sizeof hints); // make sure the struct is empty
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

	//get addrinfo for server1
	//struct serverinfo will store the information
	int status = getaddrinfo("localhost", "23564", &hints, &server3info);
	if (status != 0) {
		perror("Failed to retrieve server2:getaddrinfo");
		return fail;
	}

	char* IPs3 = inet_ntoa(((struct sockaddr_in*)(server3info->ai_addr))->sin_addr);

	//======================================================================//

	//========================== SOCKET CREATION ==========================//

	//create socket and get socket descriptor
	int socktcp = socket(AF_INET, SOCK_STREAM, 0);
	if (socktcp == -1) {
		perror("Failed to create server2:TCPsocket");
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
		perror("Failed to bind server2:TCPsocket");
		return fail;
	}

	//============================ GETSOCKNAME ============================//

	struct sockaddr_in store;
	socklen_t storesize = sizeof store;

	int getsockstat = getsockname(socktcp, (struct sockaddr *)&store, &storesize);
	if (getsockstat == -1) {
		perror("Failed to retrieve port number server2:getsockname");
		return fail;
	}

	//======================================================================//

	//======================================================================//


	//=========================== CONNECT TO S2 ============================//

	//establish a TCP connection with server 2

	int connStatus = connect(socktcp, (struct sockaddr *)(server3info->ai_addr), server3info->ai_addrlen);
	if (connStatus == -1) {
		perror("Failed to connect server2:TCPsocket");
		return fail;
	}

	//======================================================================//


	//============================== SEND ==================================//

	//if server 2 accepts connection, we can begin sending it a GET request

	string strmsg = "GET " + request;
	const char* msg = strmsg.c_str();
	int len = strlen(msg);

	int sendStatTCP = write(socktcp, msg, len);
	if (sendStatTCP == -1) {
		perror("Failed to send server2:write");
		return fail;
	}
	else if (sendStatTCP != len) {
		perror("Not all bytes sent server2:write");
		return fail;
	}

	cout << "Server 2 sends the request " << msg << " to Server 3" << endl;
	cout << "\n";

	cout << "The TCP port number is " << ntohs(((struct sockaddr_in*)(&store))->sin_port);
	cout << " and the IP address is " << inet_ntoa(((struct sockaddr_in*)(&c))->sin_addr) << endl;
	cout << "\n";

	//======================================================================//

	//============================ RECEIVE =================================//

	char res[12];

	int readStat = read(socktcp, res, 12);
	if (readStat == -1) {
		perror("Failed to receive server1:read");
		return fail;
	}
	else if (readStat == 0) {
		perror("Closing server1:read");
		close(socktcp);
		return fail;
	}
	else if (readStat != int(strlen(res))) {
		perror("Not all bytes received server1:read");
		return fail;
	}

	cout << "Server 2 receieved the value " << res << " from Server 3 ";
	cout << "with port number " << ntohs(((struct sockaddr_in*)(server3info->ai_addr))->sin_port);
	cout << " and IP address " << IPs3 << endl;
	cout << "\n";

	//======================================================================//

	res[12] = '\0';
	string result(res);

	close(socktcp);

	cout << "Server 2 closed the TCP connection with Server 3" << endl;
	cout << "\n";

	return result;

}
