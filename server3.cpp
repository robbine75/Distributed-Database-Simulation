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

	cout << "===================================================== \n" << endl; 

	//======================= READING DATA FROM TXT =========================//

	map<string, string> dataSet; 
	//read data from server1.txt and store in Map
	//this converts data in .txt file to standard input
	freopen("server3.txt", "r", stdin);

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
		cout << "Failed to create server3:socket" << endl;
		return 1;
	}

	//------- CODE BORROWED FROM https://www.cs.rutgers.edu/~pxk/417/notes/sockets/index.html ------//
	struct sockaddr_in c;
	memset((char *)&c, 0, sizeof(c));
	c.sin_family = AF_INET;
	c.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	c.sin_port = htons(23564);
	socklen_t addr_len= sizeof c;

	//----------------------------------------------------------------------------------------------//

	//bind the socket to port 22564
	int bindStat = bind(sockfd, (struct sockaddr *)&c, addr_len);
	if (bindStat == -1) {
		cout << "Failed to bind server3:TCPsocket" << endl;
		return 1;
	}

	cout << "Sever 3 has TCP port number " << ntohs(((struct sockaddr_in*)(&c))->sin_port);
	cout << " and IP address " << inet_ntoa(((struct sockaddr_in*)(&c))->sin_addr) << endl;
	cout << "\n";

	//======================================================================//


	//============================ SOCKET LISTEN ===========================//

	//backlog = 3 is good enough for this project
	int lis = listen(sockfd, 3);
	if (lis == -1) {
		cout << "Failed to listen server3:listen" << endl;
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
			cout << "Failed to accept server3:accept" << endl;
			return 1;
		}

		//======================================================================//


		//================================ READ ================================//

		char reqMsg[9];

		int readbytes = read(newSockfd, reqMsg, 9);
		if (readbytes == -1) {
			cout << "Failed to receive server2:read" << endl;
			return 1;
		}
		else if (readbytes == 0) {
			cout << "Closing server2:read" << endl;
			close(newSockfd);
			return 0;
		}
		else if (readbytes != int(strlen(reqMsg))) {
			cout << "Not all bytes received server2:read" << endl;
			return 1;
		}

		reqMsg[9] = '\0';

		cout << "Server 3 has received a request with key " << reqMsg << " ";		
		cout << "from Server 2 with port number " << ntohs(((struct sockaddr_in*)(&incoming))->sin_port);
		cout << " and IP address " << inet_ntoa(((struct sockaddr_in*)(&incoming))->sin_addr) << endl;
		cout << "\n";

		//======================================================================//


		//=========================== LOOKUP AND WRITE ==========================//

		string req(reqMsg);
		req = req.substr(4, req.length());
		
		//look up request in map
		map<string, string>::iterator find = dataSet.find(req);
		string result;

		if (find != dataSet.end()) {
			//this means server 2 has the value in it's tables and can now send it back to server 1
			result = "POST " + (find->second);
		}

		else {
			result = "";
			cout << "Unable to find request" << endl;
			return 1;
		}

		const char* msg = result.c_str();
		int len = strlen(msg); 

		int writebytes = write(newSockfd, msg, len);
		if (writebytes == -1) {
			cout << "Failed to send server3:write" << endl;
			return 1;
		}
		else if (writebytes != len) {
			cout << "Not all bytes sent server3:write" << endl;
			return 1;
		}

		cout << "Server 3 sends the reply " << msg << " to Server 2";
		cout << " with port number " << ntohs(((struct sockaddr_in*)(&incoming))->sin_port);
		cout << " and IP address " << inet_ntoa(((struct sockaddr_in*)(&incoming))->sin_addr) << endl;
		cout << "\n";

		cout << "===================================================== \n" << endl; 

		//======================================================================//

	}	

	return 0;
}
