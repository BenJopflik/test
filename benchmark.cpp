#include "build/bank.capnp.h"

#include <capnp/ez-rpc.h>
#include <kj/debug.h>
#include <vector>
#include <array>
#include <cassert>
#include <iostream>
#include <thread>
#include <iostream>

static const std::vector<std::array<uint64_t, 4>> clients = 
{
    {0x1, 0x2, 0x3, 0x4},
    {0x2, 0x3, 0x4, 0x5},
    {0x3, 0x4, 0x5, 0x6},
    {0x4, 0x5, 0x6, 0x7}
};

static std::vector<::capnp::Data::Reader> readers;

void init_clients(uint16_t port)
{
    capnp::EzRpcClient client(std::string("localhost:").append(std::to_string(port)));
    ClientManager::Client client_manager= client.getMain<ClientManager>();

    auto& waitScope = client.getWaitScope();

    for (const auto & client : clients)
    {
        readers.emplace_back((const uint8_t *)client.data(), 32);

        auto request = client_manager.createUserRequest();
        request.setId(readers.back());
        request.setAmount(1000);

        auto promise = request.send();
        assert(promise.wait(waitScope).getError() == ::ClientManager::ErrorCode::OK);
    }
}

std::vector<std::thread> spawn_threads(const uint64_t number_of_threads, const uint64_t number_of_requests, const uint16_t port)
{
    std::vector<std::thread> threads(number_of_threads);
    auto thread_function = [&readers](const uint16_t port, const uint64_t number_of_requests)
    {
        capnp::EzRpcClient client_(std::string("localhost:").append(std::to_string(port)));
        Transfer::Client client = client_.getMain<Transfer>();

        auto& waitScope = client_.getWaitScope();
    
        const size_t number_of_readers = readers.size();
        size_t ind = 0;
        for (size_t i = 0; i < number_of_requests; ++i)
        {
            auto request = client.transferRequest();
            request.setSrc(readers[ind]);
            ind = (ind + 1) % number_of_readers;
            request.setDst(readers[ind]);
            request.setAmount(1);

            auto promise = request.send();
            promise.wait(waitScope).getError();
            //std::cerr << std::this_thread::get_id() << " " << i << std::endl;
        }
    };

    for (auto & thread : threads)
        thread = std::thread(thread_function, port, number_of_requests);

    return threads;
}


int main(int argc, const char * argv[])
{
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " Port" << std::endl;
        exit(1);
    }

    int port = std::stoi(argv[1]);
    if (port < 0)
    {
        std::cerr << "invalid port" << std::endl;	
        return 1;
    }
    
    init_clients(port + 1); 
   
    //auto threads = spawn_threads(std::thread::hardware_concurrency(), 100000, port);
    auto threads = spawn_threads(4, 100000, port);
    
    for (auto & thread : threads)
        if (thread.joinable())
            thread.join();

    return 0;
}

