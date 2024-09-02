#include <stdio.h>   // Standard I/O functions
#include <stdlib.h>  // Standard library functions like atoi
#include <string.h>  // String manipulation functions like bzero
#include <unistd.h>  // UNIX standard functions like close
#include <sys/types.h>  // Data types used in system calls
#include <sys/socket.h> // Definitions for socket functions
#include <netinet/in.h> // Structures for internet domain addresses
#include <iostream>     // Input/output stream functions for C++
#include <arpa/inet.h>  // Definitions for internet operations (e.g., inet_pton)
#include <thread>       // Standard library support for threading
#include <vector>       // Standard template library (STL) support for dynamic arrays (vectors)

constexpr unsigned int MAX_BUFFER = 128;         // Maximum buffer size for reading data from the client
constexpr unsigned int MSG_REPLY_LENGTH = 18;    // Length of the message reply from the server to the client

// Function to handle client communication
void handleclient(int clientsockfd)
{
    // Buffer to store the data received from the client
    char buffer[MAX_BUFFER];

    // Reply message to send to the client upon connection
    std::string reply = "You are connected!";

    // Send the reply message to the client
    write(clientsockfd, reply.c_str(), reply.size());

    // Infinite loop to keep communicating with the client
    while (1)
    {
        // Read data from the client socket into the buffer
        int bytesRead = read(clientsockfd, buffer, sizeof(buffer) - 1);

        // Check if there was an error in reading data
        if (bytesRead < 0)
        {
            std::cerr << "read from socket error" << std::endl;
            break; // Exit the loop if there is an error
        }
        else
        {
            // Null-terminate the buffer to ensure it's a valid C-style string
            buffer[bytesRead] = '\0';

            // Print the received message to the console
            std::cout << "Got the message: " << buffer << std::endl;
        }
    }

    // Close the client socket after communication ends
    close(clientsockfd);
}

int main(int argc, char *argv[])
{
    // Check if the user has provided both IP address and port number as arguments
    if (argc < 3)
    {
        std::cerr << "Usage: " << argv[0] << " <IP_ADDRESS> <PORT>" << std::endl;
        return 1;
    }

    // Extract the server IP address and port number from command-line arguments
    const char *SERVER_IP = argv[1];
    int SERVER_PORT = atoi(argv[2]);

    /*
    Create a socket to listen for incoming connections
    AF_INET: IPv4 address family
    SOCK_STREAM: Stream socket type (TCP)
    0: Default protocol for SOCK_STREAM (TCP)
    */
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    // Check if socket creation was successful
    if (sockfd < 0)
    {
        std::cerr << "open socket error" << std::endl;
        return 1;
    }

    // Option value for setsockopt to allow address reuse
    int optval = 1;

    /*
    Set socket options to reuse the address
    SOL_SOCKET: Socket-level option
    SO_REUSEADDR: Allow address reuse
    */
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval, sizeof(int));

    // Structures to store server and client address information
    struct sockaddr_in serv_addr, cli_addr;

    // Clear the serv_addr structure to ensure no garbage values
    bzero((char *)&serv_addr, sizeof(serv_addr));

    // Set the address family to IPv4
    serv_addr.sin_family = AF_INET;

    /*
    Convert the IP address from text to binary form
    inet_pton: Convert IP address from text (presentation) to binary (network)
    */
    if (inet_pton(AF_INET, SERVER_IP, &serv_addr.sin_addr) <= 0)
    {
        std::cerr << "Invalid address/Address not supported: " << SERVER_IP << std::endl;
        return 2;
    }

    // Set the port number for the server to listen on (converting to network byte order)
    serv_addr.sin_port = htons(SERVER_PORT);

    /*
    Bind the socket to the specified IP address and port number
    sockfd: The socket file descriptor
    serv_addr: The server address structure
    */
    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        std::cerr << "bind error" << std::endl;
        return 3;
    }

    /*
    Start listening for incoming connections on the socket
    5: Maximum number of pending connections in the queue
    */
    listen(sockfd, 5);

    // Vector to store threads for handling multiple clients
    std::vector<std::thread> threads;

    // Store the length of the client address structure
    socklen_t clilen = sizeof(cli_addr);

    /*
    Accept a connection from a client
    sockfd: The socket file descriptor
    cli_addr: The client address structure
    clilen: The length of the client address structure
    */
    int newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);

    // Check if the connection was successfully accepted
    if (newsockfd < 0)
    {
        std::cerr << "accept error" << std::endl;
        return 4;
    }

    // Print the client's IP address and port number upon successful connection
    std::cout << "server: got connection from = "
              << inet_ntoa(cli_addr.sin_addr)      // Convert client's IP address to string
              << " and port = " << ntohs(cli_addr.sin_port)  // Convert client's port number to host byte order
              << std::endl;

    // Create a new thread to handle communication with the connected client
    threads.emplace_back(handleclient, newsockfd);

    // Join all the threads before exiting
    for (auto &t : threads)
    {
        if (t.joinable())
        {
            t.join();
        }
    }

    // Close the listening socket
    close(sockfd);
    return 0;
}
