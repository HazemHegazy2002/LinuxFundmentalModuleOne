#include <stdio.h>       // Standard I/O functions
#include <stdlib.h>      // Standard library functions, including memory allocation, process control, conversions, etc.
#include <unistd.h>      // POSIX API functions, including read, write, close, etc.
#include <string.h>      // String manipulation functions
#include <sys/types.h>   // Definitions for data types used in system calls
#include <sys/socket.h>  // Definitions for socket functions and structures
#include <netinet/in.h>  // Definitions for internet address family
#include <netdb.h>       // Definitions for network database operations
#include <iostream>      // Standard C++ I/O streams

// Define a constant for the maximum buffer size used for communication
constexpr unsigned int MAX_BUFFER = 128;

int main(int argc, char *argv[])
{
    /*
     * atoi() converts the command-line argument from a string to an integer.
     * argv[2] contains the port number passed as an argument when running the program.
     * SERVER_PORT will hold the integer value of the port number.
     */
    int SERVER_PORT = atoi(argv[2]);

    /*
     * socket() creates a new socket and returns a file descriptor (sockfd) for it.
     * AF_INET specifies that the socket will use the IPv4 protocol.
     * SOCK_STREAM indicates that the socket will use TCP, which provides reliable, connection-based communication.
     * The third argument is set to 0, which tells the system to choose the appropriate protocol for the given socket type.
     * If socket() fails, it returns -1, and the program will output an error message and terminate.
     */
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        std::cerr << "socket error" << std::endl; // Output an error message if socket creation fails
        return 1; // Exit the program with an error code
    }

    /*
     * gethostbyname() resolves the hostname passed as argv[1] into an IP address.
     * The function returns a pointer to a hostent structure, which contains the server's IP address and other information.
     * If gethostbyname() returns NULL, it means the hostname could not be resolved, and an error message is displayed.
     */
    struct hostent *server = gethostbyname(argv[1]);
    if (server == nullptr)
    {
        std::cerr << "gethostbyname, no such host" << std::endl; // Output an error message if the hostname is invalid
    }

    /*
     * Declare and initialize a sockaddr_in structure, which will store the server's address.
     * This structure is specifically designed to handle addresses in the IPv4 address family.
     */
    struct sockaddr_in serv_addr;

    /*
     * bzero() clears the serv_addr structure by setting all its bytes to zero.
     * This ensures that no garbage values are present in any of its fields before assigning the actual values.
     */
    bzero((char *)&serv_addr, sizeof(serv_addr));

    /*
     * Set the address family to AF_INET, indicating that the socket will use IPv4 addresses.
     */
    serv_addr.sin_family = AF_INET;

    /*
     * bcopy() copies data from one memory location to another.
     * It copies the IP address of the server (obtained from gethostbyname()) into the serv_addr structure.
     * server->h_addr is the source, which points to the server's IP address.
     * serv_addr.sin_addr.s_addr is the destination, where the IP address will be stored in the sockaddr_in structure.
     * server->h_length indicates the number of bytes to copy, which is the length of the IP address.
     */
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);

    /*
     * Set the port number where the server is listening.
     * htons() (Host TO Network Short) converts the port number from host byte order (little-endian) to network byte order (big-endian).
     * Network protocols use big-endian format, so this conversion is necessary for compatibility.
     */
    serv_addr.sin_port = htons(SERVER_PORT);

    /*
     * connect() attempts to establish a connection to the server specified by the serv_addr structure.
     * sockfd is the socket file descriptor created earlier.
     * (struct sockaddr *)&serv_addr casts the address of serv_addr to a generic struct sockaddr pointer, which is required by the connect function.
     * sizeof(serv_addr) specifies the size of the serv_addr structure.
     * If connect() returns a value less than 0, it means the connection attempt failed, and an error message is displayed.
     */
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        std::cerr << "connection error" << std::endl; // Output an error message if the connection fails
        return 3; // Exit the program with an error code
    }

    /*
     * Create a string buffer to store the message to be sent to the server.
     * The buffer size is defined by MAX_BUFFER (128 characters).
     */
    std::string writeBuffer(MAX_BUFFER, 0);

    // Inform the user that they can start sending messages to the server
    std::cout << "Enter messages to send to the server. Type 'exit' to quit." << std::endl;

    /*
     * Start an infinite loop to continuously read user input and send messages to the server.
     * The loop will only terminate when the user types "exit".
     */
    while (1)
    {
        // Read a line of input from the user and store it in writeBuffer
        getline(std::cin, writeBuffer);

        // If the user types "exit", break the loop and close the connection
        if (writeBuffer == "exit")
        {
            break;
        }

        /*
         * write() sends the data stored in writeBuffer to the server through the socket.
         * writeBuffer.c_str() converts the string to a C-style string (null-terminated char array).
         * strlen(writeBuffer.c_str()) returns the length of the string (excluding the null terminator).
         * If write() returns a value less than 0, it indicates that an error occurred while sending the data.
         */
        if (write(sockfd, writeBuffer.c_str(), strlen(writeBuffer.c_str())) < 0)
        {
            std::cerr << "write to socket" << std::endl; // Output an error message if writing to the socket fails
            return 4; // Exit the program with an error code
        }
    }

    // Close the socket to terminate the connection with the server
    close(sockfd);

    // Return 0 to indicate that the program has executed successfully
    return 0;
}
