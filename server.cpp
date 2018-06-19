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
    auto exit = [](const char * error_message)
    {
        perror(error_message);     
        ::exit(1);
    };

    // Creating socket file descriptor
    int fd = socket(AF_INET, SOCK_STREAM, 0);

    if (fd < 0)
        exit("socket");
      
    int val = 1;
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &val, sizeof(val)) < 0)
        exit("setsockopt");
    
    sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);
      
    if (bind(fd, (sockaddr *)&address, sizeof(address)) < 0)
        exit("bind");

    if (listen(fd, 10) < 0)
        exit("listen");
    
    return fd;
}


std::vector<std::thread> spawn_transfer_threads(const uint64_t number_of_threads, std::shared_ptr<Bank> bank, const uint16_t port)
{
    std::vector<std::thread> threads(number_of_threads);

    auto thread_function = [bank](const uint16_t port)
    {
        capnp::EzRpcServer server(kj::heap<TransactionProcessor<Bank>>(bank), prepare_socket(port), port);

        auto& waitScope = server.getWaitScope();

        std::cerr << "listening on " << server.getPort().wait(waitScope) << std::endl;

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

    //auto threads = spawn_transfer_threads(std::thread::hardware_concurrency(), bank, port);
    auto threads = spawn_transfer_threads(4, bank, port);

    capnp::EzRpcServer client_manager(kj::heap<ClientManagerProcessor<Bank>>(bank), std::string("*:").append(std::to_string(port + 1)));

    auto& waitScope = client_manager.getWaitScope();

    kj::NEVER_DONE.wait(waitScope);
    
    return 0;
}

