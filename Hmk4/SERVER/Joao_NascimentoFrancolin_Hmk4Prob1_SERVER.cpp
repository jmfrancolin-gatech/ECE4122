/*
Author: Joao Matheus Nascimento Francolin Francolin
Class: ECE 4122
Last Date Modified: October 29, 2019

Description: This program behavies as a TCP server that will listen for connection
requests and communicate with TCP clients. It takes a port number as a command line
argumment, and it is capable of spwnning multiple threads to handle simultaneous
requests from different clients.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h> 
#include <iostream>

// Assume POSIX-style sockets
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>

// Additional libraries
#include <thread>
#include <atomic>
#include <string>
#include <mutex>
#include <vector>
#include <algorithm>
#include <set>

// module load gcc/7.3.0
// g++ -lpthread server.cpp -o server

// Structures definition
struct tcpMessage
{
    unsigned char nVersion;
    unsigned char nType;
    unsigned short nMsgLen;
    char chMsg[1000];
};

struct clientInfo
{
    char* IP;
    int port;
    int sockfd;
};

// Already defined --
//    struct sockaddr_in {
//        short            sin_family;   // e.g. AF_INET
//        unsigned short   sin_port;     // e.g. htons(3490)
//        struct in_addr   sin_addr;     // see struct in_addr, below
//        char             sin_zero[8];  // zero this if you want to
//    };

// Global declarations
std::vector<std::thread> clients;
std::vector<clientInfo> clientsMetadata;
std::set<int> activePorts;
std::atomic<bool> activeServer (true);
std::mutex mutex;
char lastMessage[1000];
void terminate();
int listenner_sockfd;

typedef int SOCKET;

// Socket initialization
int sockInit(void)
{
    return 0;
}

// Socket termination
int sockQuit(void)
{
    return 0;
}

// Close socket
int sockClose(SOCKET sock)
{
    int status = 0;

    status = shutdown(sock, SHUT_RDWR);
    if (status == 0) 
    { 
        status = close(sock);
    }
    return status;
}

// Standard error function
void error(const char *msg)
{
    perror(msg);
    exit(1);
}

// A new instance of this thread is created everytime a new client is connected
// Its main purpose is to send and receive mensagens between the server and the client
void workerThread(int sockfd, char* IP,  int port)
{    
    // Instanciate local variables
    tcpMessage packet;
    memset(&packet, 0,  sizeof(tcpMessage));
    int n;
    
    while (activeServer)
    {
        // blocks until the client connected on this socket sends a message
        n = recv(sockfd, &packet, sizeof(tcpMessage), 0);
        
        //if (n == 0)
            //exit(0);
        
        // Ignore message if nVersion == 1
        if (packet.nVersion != '1' && n > 0)
        {
                    
            // Lock block of code
            mutex.lock();
            // Update last message received
            strcpy(lastMessage, packet.chMsg);
            // Unlock block
            mutex.unlock();
        
            // Resend to all but you if nType == 0
            if (packet.nType == '0')
            {
                // foward the message to all connected clients (excluding sender)
                for (int i = 0; i < clientsMetadata.size(); i++)
                {
                    int new_sockfd = clientsMetadata[i].sockfd;
                    
                    // compare all stored sockfds with the one given as a function arg
                    if (new_sockfd != sockfd)
                    {
                        //
                        
                        n = send(new_sockfd, &packet, sizeof(tcpMessage), 0);
                    }
                }
            }
            // Reverse and resend to you of nType == 1
            else if (packet.nType == '1')
            { 
                // reverse message and foward it only to itself
                int len = strlen(packet.chMsg);
                packet.chMsg[len - 1] = 0;
                std::reverse(packet.chMsg, packet.chMsg + strlen(packet.chMsg));
                
                // Lock block of code
                mutex.lock();
                // Update last message received
                strcpy(lastMessage, packet.chMsg);
                // Unlock block
                mutex.unlock();
                
                n = send(sockfd, &packet, sizeof(tcpMessage), 0);
            }
        }
    }
}

// This thread is responssable to create a new worker thread for every new client
void listenIncomingClients(int portno)
{
    // variables for TCP socjets
    int sockfd, newsockfd;
    socklen_t clilen;
    struct sockaddr_in serv_addr, cli_addr;
    int n;
    // client relevant metadata
    char* IP;
    int port;
        
    sockInit();
    // Create the socket
    //int socket(int domain, int type, int protocol);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    listenner_sockfd = sockfd;
    
    // Make sure the socket was created
    if (sockfd < 0)
    {
        error("ERROR opening socket");
    }
    // Zero out the variable serv_addr
    // void * memset(void * ptr, int value, size_t num);
    memset((char *)&serv_addr, 0, sizeof(serv_addr));

    // Initialize the serv_addr
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
            
    // Convert port number from host to network
    serv_addr.sin_port = htons(portno);
    
    // Bind the socket to the port number
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
    {
        error("ERROR on binding");
    }

    // Run loop until flag signal
    while (activeServer)
    {
        // Attemp connection
        listen(sockfd, 5);
        clilen = sizeof(cli_addr);
        
        // blocks until a client connects to the server
        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
        
        if (newsockfd > 0)
        {
            // get relevant info from client
            IP = inet_ntoa(cli_addr.sin_addr);
            port = ntohs(cli_addr.sin_port);
            
            // Lock block of code
            mutex.lock();
            
            // Update port and IP
            clientInfo new_client;
            new_client.IP = IP;
            new_client.port = port;
            new_client.sockfd = newsockfd;
            
            // Check if port has not being binded
            if (!(activePorts.find(port) != activePorts.end()))
            {
                // This is a new clint
                activePorts.insert(port);
                
                // Save its metadata
                clientsMetadata.push_back(new_client);   
            }
     
            // Spaw new worker threads and store them into a vector
            clients.push_back(std::thread(workerThread, newsockfd, IP, port));    
            
            // Unlock block
            mutex.unlock(); 
        } 
    }
}

// The main function handles console I/O
int main(int argc, char *argv[])
{
    // Check command line args
    if (argc < 2)
    {
        fprintf(stderr, "ERROR, no port provided\n");
        exit(1);
    }
    
    // Convert the port number string to an int
    // int atoi (const char * str);
    int portno = atoi(argv[1]);

    // Create a listenning thread   
    std::thread threadListener(listenIncomingClients, portno);

    // create char buffer
    char buffer[1000];
    
    // Loop until user quits
    while (activeServer)
    {
        // Get user input
        std::cout << ("Please enter command: ");
        memset(buffer, 0, 1000);
        fgets(buffer, sizeof(buffer), stdin);
        
        if (strcmp(buffer,"0\n") == 0)
        {   
            // prints to the console the last message received
            std::cout << "Last Message: " << lastMessage << std::endl;
            
        }
        else if (strcmp(buffer,"1\n") == 0)
        {
            // prints to the console a list of all connected clients
            std::cout << "Number of Clients: \t" << clientsMetadata.size() << std::endl;
            std::cout << "IP Address\tPort" << std::endl;

            for (int i = 0; i < clientsMetadata.size(); i++)
            {
                std::cout << clientsMetadata[i].IP << "\t";
                std::cout << clientsMetadata[i].port << std::endl;
            }
        }
        else if (strcmp(buffer,"q\n") == 0)
        {
            activeServer = false;        
        }
        else
        {
            // Default response for bad inputs
            std::cout << "Bad input" << std::endl;
        }
    }
    
    // Close workers sockets
    for (int i = 0; i < clientsMetadata.size(); i++)
    {
        sockClose(clientsMetadata[i].sockfd);
    }
    // Join workers threads
    for (int i = 0; i < clients.size(); i++)
    {   
        if (clients[i].joinable())
        {
            clients[i].join();
        }
    }
    // Close listenning socket
    sockClose(listenner_sockfd);
    // Join listenning socket
    threadListener.join();
    // Quick socket
    sockQuit();
    
    return 0;
}

