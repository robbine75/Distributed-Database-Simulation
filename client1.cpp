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
	map<string, string> searchData; 
	//read data from server1.txt and store in Map
	//this converts data in .txt file to standard input
	freopen("client1.txt", "r", stdin);

	//read into map
	while (!cin.fail()) {
		string term, value;
		cin >> term >> value;
		searchData.insert(make_pair(term, value));
	}

	

	return 0;
}
