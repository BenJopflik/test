#include "build/bank.capnp.h"
#include <kj/debug.h>
#include <capnp/ez-rpc.h>
#include <capnp/message.h>
#include <iostream>
#include <exception>

#include "bank.hpp"



template <typename BankImpl>
class Server final : public Bank::Server 
{
public:
    Server(std::shared_ptr<BankImpl> bank_impl) : m_bank_impl(bank_impl) {}

    ::kj::Promise<void> transfer(TransferContext context) override
    {
            // TODO check src/dst size should be 32
	auto params = context.getParams();
        auto src = params.getSrc();
        auto dst = params.getDst();
        auto ammount = params.getAmount();
        
        auto result = m_bank_impl->transfer(src.begin(), dst.begin(), ammount);
    
        context.getResults().setError(to_proto_error_code(result));
	return ::kj::READY_NOW;

//        return evaluateImpl(context.getParams().getExpression())
//        .then([KJ_CPCAP(context)](double value) mutable {
//      context.getResults().setValue(kj::heap<ValueImpl>(value));
     
    }

	::Bank::ErrorCode to_proto_error_code(const typename BankImpl::ErrorCode code) const
	{
		switch (code)	
		{
			case BankImpl::ErrorCode::OK:
				return ::Bank::ErrorCode::OK;

			case BankImpl::ErrorCode::INVALID_SRC:
				return ::Bank::ErrorCode::INVALID_SRC;

			case BankImpl::ErrorCode::INSUFFICIENT_FUNDS:
				return ::Bank::ErrorCode::INSUFFICIENT_FUNDS;

			default:
				throw std::runtime_error("unknown error code"); // TODO refact
		}
	}

    private:
        std::shared_ptr<BankImpl> m_bank_impl;

};



int main(int argc, const char* argv[]) 
{
        if (argc != 2) 
        {
                std::cerr << "usage: " << argv[0] << " ADDRESS[:PORT]\n"
                        "Runs the server bound to the given address/port.\n"
                        "ADDRESS may be '*' to bind to all local addresses.\n"
                        ":PORT may be omitted to choose a port automatically." << std::endl;
                return 1;
        }
	
	auto bank = std::make_shared<BankImpl>();

        // Set up a server.
        capnp::EzRpcServer server(kj::heap<Server<BankImpl>>(bank), argv[1]);

        // Write the port number to stdout, in case it was chosen automatically.
        auto& waitScope = server.getWaitScope();
        uint port = server.getPort().wait(waitScope);
        if (port == 0) 
        {
                // The address format "unix:/path/to/socket" opens a unix domain socket,
                // in which case the port will be zero.
                std::cout << "Listening on Unix socket..." << std::endl;
        } 
        else 
        {
                std::cout << "Listening on port " << port << "..." << std::endl;
        }

        // Run forever, accepting connections and handling requests.
        kj::NEVER_DONE.wait(waitScope);

        return 0;
}

