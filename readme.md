Name: Sai Ruthvik Peddawandla
Student ID: 9146546564

### EE 450: Socket Programming Final Project

##### 1. What I've done
+ Created a 3-server distributed hash table model using C++ and the necessary socket programming libraries
+ Client-Server communications were conducted through bidirectional UDP sockets
+ Server-Server communications were conducted through TCP sockets
+ Ports were dynamically obtained on the client side  
+ Servers & Clients are run as separate processes on nunki.usc.edu
+ Clients request information by submitting a search query to Server 1
+ The Servers act in a recursive manner to obtain the corresponding value and return it to the client
+ Note: All IPs have been retrieved using 'localhost' as the host name

#### 2. My Code Files
+ client1.cpp
	- Creates and fills a map that stores key-value pairs based on the information in client1.txt
	- Queries the user for a search command and matches it to a key within the map
	- Retreives address information of Server 1 (well-known port number 21564)
	- Creates UDP socket and submits GET request (of the key) to the server using the necessary functions
	- Recieves a POST message from Server 1 with the associated value
	- Closes the UDP socket and terminates
	- At any time an error is detected, the program will output the location of the error and exit

+ client2.cpp
	- Does exactly the same as above except it reads data from client2.txt

+ server1.cpp
	- Gets it's own address information using getaddrinfo("localhost",...)
	- The UDP port number is well-known (static) - 21564
	- Binds socket to port & IP and enters a while loop that will let it communicate with as many clients as necessary
	- Note that Server 1 will only cease to operate when ctrl+C is pressed by the user
	- Inside the while loop, Server 1 will receive the GET message from the Client
	- If it has the requested value, it will respond with a POST message and the corresponding value
	- If not, it will call a function that sets up a TCP connection with Server 2
	- In this function, it will obtain Server 2's address info (although the port number is well-known: 22564)
	- Then, a new TCP socket is created and bound to Server 1. It will forward the GET request through this socket
	- After receiving the POST value, it will return to the main function and forward the POST message to the Client
	- Before returning, it will close the TCP socket
	- At any time an error is detected, the program will output the location of the error and exit


+ server2.cpp
	- Creates a TCP socket with well-known port number 22564 and listens for connection requests
	- In a while loop, it accepts all incoming connections and a new socket is created
	- Translates the GET request and responds with a POST message or calls a function to setup a TCP connection with Server 3 correspondingly
	- The function operates exactly like the one in Server 1
	- Once the TCP connection with Server 3 is terminated, the POST message is returned to Server 1
	- At any time an error is detected, the program will output the location of the error and exit

+ server3.cpp
	- Creates a TCP socket with well-known port number 23564 and listens for connection requests
	- In a while loop, it accepts all incoming connections and a new socket is created
	- Receives the GET request and responds with the corresponding POST value
	- At any time an error is detected, the program will output the location of the error and exit

#### 3. How to Run
+ A Makefile is included. This will compile all of the separate files of code
+ In three separate windows, please run ./server1, ./server2 and ./server3
+ In a fourth window, ./client1 may be run
+ Client 1 will terminate when its job is done
+ In the same window, ./client2 can be run now

#### 4. Format of Exchanged Messages
+ The messages were sent and received between Clients and Servers as char*
+ Many of the operations within the client/server were performed by converting these char*s to strings

#### 5. Idiosyncrasies
+ None that I have encountered
+ If no value is found, the receieve function in Server 2 fails since the error message causes the buffer to overflow but the project document said that we do not need to account for this edge case

#### 6. Reused Code
+ I have clearly identified where I have reused code in the project
+ The main areas are
	- The getaddrinfo() sections have a few (2-4) lines from Beej's Socket Programming Guide
	- The bind() sections have a few (2-4) lines from this website: https://www.cs.rutgers.edu/~pxk/417/notes/sockets/index.html
	- These lines are basically information to preload a sockaddr with
