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
#include <fstream>
#include <sstream>
#include <typeinfo>

using namespace std;

int main () {

	cout << "===================================================== \n" << endl; 

	//============================= INIT. ==================================//

	//initialize init. variables/structs
	map<string, string> searchData; 
	string inKey = "";
	struct addrinfo hints;
	struct addrinfo *serverinfo;

	//======================================================================//


	//======================= READING DATA FROM TXT =========================//

	//read data from server1.txt and store in Map
	ifstream ifile("client2.txt");

	while (!ifile.fail()) {
		string term, value, line;
		getline(ifile, line);
		stringstream ss(line);
		ss >> term >> value;
		searchData.insert(make_pair(term, value));
	}
	
	//======================================================================//


	//=========================== QUERY SEMANTICS ===========================//

	cout << "Enter your search query (USC, UCLA etc.): ";
	getline(cin, inKey, '\n');
	cout << endl;

	//search map for search query:
	map<string, string>::iterator find = searchData.find(inKey);
	string key = find->second;

	cout << "Client 2 has received a request with search word " << inKey << " which maps to key " << key << endl;
	cout << "\n";

	string strMsg = "GET " + key;
	const char* sendMsg = strMsg.c_str();
	int len = strlen(sendMsg);

	//======================================================================//


	//========================= IP AND SERVER INFO ========================//
	//-------------------- Borrowed from Beej's Guide ---------------------//

    memset(&hints, 0, sizeof hints); // make sure the struct is empty
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;

	//get addrinfo for server1
	//struct serverinfo will store the information
	int status = getaddrinfo("localhost", "21564", &hints, &serverinfo);
	if (status != 0) {
		cout << "Failed to retrieve client2:getaddrinfo" << endl;
		return 1;
	}

	char* IP = inet_ntoa(((struct sockaddr_in*)(serverinfo->ai_addr))->sin_addr);

	//======================================================================//


	//========================== SOCKET CREATION ==========================//

	//create a socket and receive the socket descriptor
	//the arguments are INET, the type of socket (DGRAM) and the protocol
	int sockD = socket(PF_INET, SOCK_DGRAM, 0);
	if (sockD == -1) {
		cout << "Failed to create client2:socket" << endl;
		return 1;
	}

	//======================================================================//

	//===================== PREP CLIENT'S SOCKADDR =======================//

	//here we're going to prep the sockaddr and get the port number
	struct sockaddr_in c;

	//------- CODE BORROWED FROM https://www.cs.rutgers.edu/~pxk/417/notes/sockets/udp.html ------//

	memset((char *)&c, 0, sizeof(c));
	c.sin_family = AF_INET;
	c.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	c.sin_port = htons(0);
	socklen_t addr_len= sizeof c;

	//--------------------------------------------------------------------------------------------//


	int bindStat = bind(sockD, (struct sockaddr*)&c, addr_len);
	  if (bindStat == -1) {
	  	cout << "Failed to bind client2:bind" << endl;
	  	return 1;
	  }

	//cout << inet_ntoa(((struct sockaddr_in*)(&c))->sin_addr) << " IS THIS WHY?" << endl;

	//======================================================================//


	//============================ GETSOCKNAME ============================//

	struct sockaddr_in store;
	socklen_t storesize = sizeof store;

	int getsockstat = getsockname(sockD, (struct sockaddr *)&store, &storesize);
	if (getsockstat == -1) {
		cout << "Failed to retrieve port number client2:getsockname" << endl;
		return 1;
	}

	//======================================================================//

	//============================== SEND ==================================//

	//now we begin sending data
	//we check if it failed either if the number of bytes returned is not equal to the length sent or if -1 was returned
	int sentBytes = sendto(sockD, sendMsg, len, 0, serverinfo->ai_addr, serverinfo->ai_addrlen);
	if (sentBytes == -1) {
		cout << "Failed to send client2:send" << endl;
		return 1;
	}
	else if (sentBytes != len) {
		cout << "Not all bytes sent client2:send" << endl;
		return 1;
	}

	cout << "Client 2 sends the request " << sendMsg; 
	cout << " to Server 1 with port number " << ntohs(((struct sockaddr_in*)(serverinfo->ai_addr))->sin_port);
	cout << " and IP address " << IP << endl;
	cout << "\n";

	//note that server1's IP and the clients should be the same
	cout << "Client 2's port number is " << ntohs(((struct sockaddr_in*)(&store))->sin_port) << " and IP address is ";
	cout << inet_ntoa(((struct sockaddr_in*)(&c))->sin_addr) << endl;
	cout << "\n";

	//======================================================================//


	//============================ RECEIVE =================================//

	//intialize buffer to receive from server 1
	struct sockaddr_storage s1;
	socklen_t s1size = sizeof s1;
	char r[12];

	int recvBytes = recvfrom(sockD, r, 12, 0, (struct sockaddr*)&s1, &s1size);
	if (recvBytes == -1) {
		cout << "Failed to receive client1:recv" << endl;
		return 1;
	}
	else if (recvBytes == 0) {
		cout << "Closing client1:recv" << endl;
		close(sockD);
		return 0;
	}
	else if (recvBytes != int(strlen(r))) {
		cout << "Not all bytes received client1:recv" << endl;
		return 1;
	}

	r[12] = '\0';

	//print closing prompts
	cout << "Client 2 received the value " << r;
	cout << " from Server 1 with port number " << ntohs(((struct sockaddr_in*)(serverinfo->ai_addr))->sin_port);
	cout << " and IP address " << IP << endl;
	cout << "\n";

	cout << "Client 2’s port number is " << ntohs(((struct sockaddr_in*)(&store))->sin_port) << " and IP address is ";
	cout << inet_ntoa(((struct sockaddr_in*)(&c))->sin_addr) << endl;
	cout << "\n";

	//======================================================================//

	cout << "===================================================== \n" << endl; 

	//CLOSE SOCKET
	close (sockD);

	return 0;
}
