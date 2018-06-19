#include <iostream>
#include <exception>
#include <thread>
#include <vector>

#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <fcntl.h>


#include "transfer_processor.hpp"
#include "bank.hpp"

#include "build/bank.capnp.h"

int prepare_socket(const uint16_t port)
{

    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    char *hello = "Hello from server";
      
    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
      
    // Forcefully attaching socket to the port 8080
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                                                  &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( port );
      
    // Forcefully attaching socket to the port 8080
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    return server_fd;
}




int main(int argc, const char* argv[]) 
{
    if (argc != 2) 
    {
        std::cerr << "usage: " << argv[0] << " PORT\n";
        return 1;
    }

    auto bank = std::make_shared<Bank>();

    int port = std::stoi(argv[1]);
    if (port < 0)
    {
        std::cerr << "invalid port" << std::endl;	
        return 1;
    }

    std::vector<std::thread> threads(std::thread::hardware_concurrency());
    std::cerr << "hardware concurrency " << std::thread::hardware_concurrency() << std::endl;	
    auto thread_function = [&bank](const uint16_t port)
    {
        // Set up a server.
        capnp::EzRpcServer server(kj::heap<TransactionProcessor<Bank>>(bank), prepare_socket(port), port);

        // Write the port number to stdout, in case it was chosen automatically.
        auto& waitScope = server.getWaitScope();
        std::cerr << "listening on " << server.getPort().wait(waitScope) << std::endl;
        // Run forever, accepting connections and handling requests.
        kj::NEVER_DONE.wait(waitScope);
    };

    for (auto & thread : threads)
        thread = std::thread(thread_function, port);

    for (auto & thread : threads)
        if (thread.joinable())
            thread.join();

    return 0;
}

