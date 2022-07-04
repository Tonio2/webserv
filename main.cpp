#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <string>
#include "Request.hpp"
#include "Response.hpp"

int main()
{
    int clientSocket;
    int bytesRecv;
    Request req;
    Response rep;
    std::cout << "Creating server socket..." << std::endl;
    int listening = socket(AF_INET, SOCK_STREAM, 0);
    if (listening == -1)
    {
        std::cerr << "Can't create a socket!";
        return -1;
    }

    struct sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(80);
    inet_pton(AF_INET, "0.0.0.0", &hint.sin_addr);

    std::cout << "Binding socket to sockaddr..." << std::endl;
    if (bind(listening, (struct sockaddr *)&hint, sizeof(hint)) == -1) 
    {
        std::cerr << "Can't bind to IP/port";
        return -2;
    }

    std::cout << "Mark the socket for listening..." << std::endl;
    if (listen(listening, SOMAXCONN) == -1)
    {
        std::cerr << "Can't listen !";
        return -3;
    }

    sockaddr_in client;
    socklen_t clientSize = sizeof(client);

    char buf[4096];
    while (true) {

        std::cout << "Accept client call..." << std::endl;
        clientSocket = accept(listening, (struct sockaddr *)&client, &clientSize);

        std::cout << "Received call..." << std::endl;
        if (clientSocket == -1)
        {
            std::cerr << "Problem with client connecting!";
            return -4;
        }

        std::cout << "Client address: " << inet_ntoa(client.sin_addr) << " and port: " << client.sin_port << std::endl;
        // clear buffer
        memset(buf, 0, 4096);

        // wait for a message
        bytesRecv = read(clientSocket, buf, 4096);
        
        // display message
        std::cout << "-------------------Received--------------------\n" << buf << "\n";

        //Parse request
        req.fill_request(buf);

        //Create response
        rep.fill_response(req);

        // return message
        write(clientSocket, rep.res_to_str().c_str(), bytesRecv);

        // close socket
        close(clientSocket);
    }
    return 0;
}