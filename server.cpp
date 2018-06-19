#include <iostream>
#include <exception>
#include <thread>
#include <vector>

#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <fcntl.h>


#include "transfer_processor.hpp"
#include "client_manager_processor.hpp"
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


std::vector<std::thread> spawn_transfer_threads(const uint64_t number_of_threads, std::shared_ptr<Bank> bank, const uint16_t port)
{
    std::vector<std::thread> threads(std::thread::hardware_concurrency());

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

    return threads;
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

    auto threads = spawn_transfer_threads(std::thread::hardware_concurrency(), bank, port);

    capnp::EzRpcServer client_manager(kj::heap<ClientManagerProcessor<Bank>>(bank), std::string("*").append(std::to_string(port)));

    // Write the port number to stdout, in case it was chosen automatically.
    auto& waitScope = client_manager.getWaitScope();

    // Run forever, accepting connections and handling requests.
    kj::NEVER_DONE.wait(waitScope);

    return 0;
}

