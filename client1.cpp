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
	//initialize init. variables/structs
	map<string, string> searchData; 
	string inKey;
	struct addrinfo hints;
	struct addrinfo* serverinfo;

	//read data from server1.txt and store in Map
	//this converts data in .txt file to standard input
	freopen("client1.txt", "r", stdin);

	//read into map
	while (!cin.fail()) {
		string term, value;
		cin >> term >> value;
		searchData.insert(make_pair(term, value));
	}

	cout << "Enter your search query (USC, USCLA etc.): " << endl;
	cin >> inKey;

	//search map for search query:
	map<string, string>::iterator find = searchData.find(inKey);
	string key = it->second;

	cout << "The client1 has received a request with search word " << inKey << " which maps to key " << key << endl;

	string strMsg = "GET " + key;
	const char* sendMsg = strMsg.c_str();
	int len = strlen(sendMsg);

	//----------------------- Taken from Beej's Guide -----------------------//

	memset(&hints, 0, sizeof hints); // make sure the struct is empty

	//set the hints in the hints struct (we want the UDP socket)
	hints.ai_socktype = SOCK_DGRAM;	//we need UDP
	hints.ai_family = AF_UNSPEC; //don't care what kind
	hints.ai_flags = AI_PASSIVE; 	//hopefully this will fill in nunki's IP here

	//-------------------------------------------------------------------------//


	//get addrinfo for server1
	//struct serverinfo will store the information
	int status = getaddrinfo("nunki.usc.edu", "21564", &hints, &serverinfo);
	if (status != 0) {
		cout << "Failed to retrieve client1:getaddrinfo" << endl;
		return 1;
	}

	//create a socket and receive the socket descriptor
	//the arguments are INET, the type of socket (DGRAM) and the protocol, which can be retrieved from serverinfo
	int sockD = socket(serverinfo->ai_family, serverinfo->ai_socktype, serverinfo->ai_protocol);
	if (sockD == -1) {
		cout << "Failed to create client1:socket" << endl;
		return 1;
	}

	//now that we have the socket descriptor, we can connect to server1 from that socket
	int connStatus = connect(sockD, serverinfo->ai_addr, serverinfo->addrlen);
	if (connStatus == -1) {
		cout << "Failed to connect client1:connect" << endl;
		return 1;
	}

	//now we begin sending data
	//we check if it failed either if the number of bytes returned is not equal to the length sent or if -1 was returned
	int sentBytes = send(sockD, sendMsg, len, 0);
	if (sentBytes == -1) {
		cout << "Failed to send client1:send" << endl;
		return 1;
	}
	else if (sentBytes != len) {
		cout << "Not all bytes sent client1:send" << endl;
		return 1;
	}

	//convert server1's IP to an ints
	int s1IP = ntohs(serverinfo->ai_addr->sin_addr);

	cout << "Client1 sends the request " << sendMsg; 
	cout << " to Server1 with port number 21564 and IP address " << s1IP << endl;

	//to get port number of client
	//c is of type sockaddr_in but sockaddr* can point to it when typecast
	sockaddr_in c;
	getsock_status = getsockname(sockD, (struct sockaddr*)&c, &(c.sin_addr));

	//note that server1's IP and the clients are the same
	cout << "Client1's port number is " << ntohs(c.sin_port) << "and IP address is " << s1IP << endl;

	//intialize buffer to receive from server 1
	char* r[12];
	int recvBytes = recv(sockD, r, 12, 0);
	if (recvBytes == -1) {
		cout << "Failed to receive client1:recv" << endl;
		return 1;
	}
	else if (recv == 0) {
		cout << "Closing" << endl;
		close(sockD);
		return 0;
	}
	/*else if (recvBytes != r.length()) {
		cout << "Not all bytes received client1:recv" << endl;
		return 1;
	}*/

	//close the connection regardless
	close(sockD);

	//print closing prompts
	cout << "Client1 received the value " << r << " from Server1 with port number 21564 and IP address " << s1IP << endl;

	cout << "Client1’s port number is " << ntohs(c.sin_port) << " and IP address is " << s1IP << endl;

	return 0;
}
