/*
Author: Joao Matheus Nascimento Francolin Francolin
Class: ECE 4122
Last Date Modified: October 29, 2019

Description: This program behavies as a TCP client that will sned and listen for
packets to communicate with TCP servers. It takes a hostname and a port number as
its command line argumments. It spaws two threads that handles sending and reciving
pakets to TCP servers.
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
#include <string>
#include <cstring>
#include <atomic>
#include <mutex>
#include <chrono> 

// module load gcc/7.3.0
// g++ -lpthread client.cpp -o client

// Packet structure definition
struct tcpMessage
{
    unsigned char nVersion;
    unsigned char nType;
    unsigned short nMsgLen;
    char chMsg[1000];
};

// Global declarations
void sendMessage(int sockfd);
void receiveMessage(int sockfd);
std::atomic<bool> activeClient (true);
std::mutex mutex;

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
    exit(0);
}

// This thread is used to send messagens to a TCP server. It constructs the
// message by propting the user for console inputs.
void sendMessage(int sockfd)
{
    // local variables
    tcpMessage packet;
    char buffer[1000];
    int n;
    
    // Loop until user quits
    while (activeClient)
    {
        // Get user input
        std::this_thread::sleep_for (std::chrono::seconds(1));
        //mutex.lock();
        std::cout << ("Please enter command: ");
        memset(buffer, 0, 1000);
        fgets(buffer, sizeof(buffer), stdin);
        //mutex.unlock();
        
        // Tokenize string
        char* token;
        char* remainder;
        token = strtok_r(buffer, " ", &remainder);
        
        // Updade packet version
        if (strcmp(token,"v") == 0)
        {
            packet.nVersion = remainder[0];
            //std::cout << packet.nVersion << std::endl;
        }
        //  Update & send pachet message
        else if  (strcmp(token,"t") == 0)
        {
            token = strtok_r(remainder, " ", &remainder);
            packet.nType = token[0];
            packet.nMsgLen = strlen(remainder) - 1;
            strcpy(packet.chMsg, remainder);
            //std::cout << packet.nType << std::endl;
            //std::cout << packet.nMsgLen << std::endl;
            //std::cout << packet.chMsg << std::endl;
            
            // Send packet
            n = send(sockfd, &packet, sizeof(tcpMessage), 0);
            if (n < 0)
            {
                error("ERROR writing to socket");
            }
        }
        // Check for termination condition
        else if (strcmp(token, "q\n") == 0)
        {
            activeClient = false;
            exit(0);
        }
        // Default response for bad inputs
        else
        {
            std::cout << "Bad input" << std::endl;;
        }
    }
}

// This thread is responssable for handling pakets benn sent from the server.
// Its main purpose is to output recieved messagens into the console.
void receiveMessage(int sockfd)
{
    // local variables
    tcpMessage packet;
    int n;
    
    // Loop until user quits
    while (activeClient)
    {    
       // Attemp to receive packet
       n = recv(sockfd, &packet, sizeof(tcpMessage), 0);
       
       // Successful delivery
       if (n > 0)
       {
            //std::string str; strcpy(str, packet.chMsg);
            mutex.lock();
            std::cout << "\nReceived Msg Type: " << packet.nType << "; ";
            std::cout << "Msg: " << packet.chMsg << std::endl;
            mutex.unlock();
       } 
    }
}

// The main thread stablishes communication with the TCP server and spaws
// the other two worker threads.
int main(int argc, char *argv[])
{
    // Initialization
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    // Check user input
    if (argc < 3)
    {
        fprintf(stderr, "usage %s hostname port\n", argv[0]);
        exit(0);
    }
    
    // Initialyze socket
    sockInit();
    // Convert string to int
    portno = atoi(argv[2]);
    
    // Create socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        error("ERROR opening socket");
    }
    
    // Get server name
    server = gethostbyname(argv[1]);
    if (server == NULL)
    {
        fprintf(stderr, "ERROR, no such host\n");
        exit(0);
    }
    
    memset((char *)&serv_addr, sizeof(serv_addr), 0);
    // Zero out serv_addr variable
    serv_addr.sin_family = AF_INET;
    memmove((char *)&serv_addr.sin_addr.s_addr, (char *)server->h_addr, server->h_length);
    serv_addr.sin_port = htons(portno);

    // Try to connect with server    
    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
    {
        error("ERROR connecting");
    }
    
    // Start sender and listener threads
    std::thread threadSender(sendMessage, sockfd);
    std::thread threadListener(receiveMessage, sockfd);
    
    // Return when the thread execution is completed
    threadSender.join();
    threadListener.join();
    
    // Close socket
    std::cout << sockClose(sockfd) << std::endl;
    sockQuit();

    return 0;
}

