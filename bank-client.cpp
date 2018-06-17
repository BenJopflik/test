
#include "build/bank.capnp.h"
#include <capnp/ez-rpc.h>
#include <kj/debug.h>
#include <math.h>
#include <iostream>



int main(int argc, const char* argv[]) 
{
  if (argc != 2) 
  {
    std::cerr << "usage: " << argv[0] << " HOST:PORT\n"
        "Connects to the Calculator server at the given address and "
        "does some RPCs." << std::endl;
    return 1;
  }

  capnp::EzRpcClient client(argv[1]);
  Bank::Client bank_client = client.getMain<Bank>();

  // Keep an eye on `waitScope`.  Whenever you see it used is a place where we
  // stop and wait for the server to respond.  If a line of code does not use
  // `waitScope`, then it does not block!
  auto& waitScope = client.getWaitScope();

  {
    std::cout << "Transfering a literal... ";
    std::cout.flush();

    // Set up the request.
    auto request = bank_client.transferRequest();
    request.setSrc(::capnp::Data::Reader((const uint8_t *)"0123456789012345678901234567890", 32)); // zero byte
    request.setDst(::capnp::Data::Reader((const uint8_t *)"0123456789012345678901234567891", 32)); // zero byte
    request.setAmount(199);

    // Send it, which returns a promise for the result (without blocking).
    auto promise = request.send();

    std::cout << "result = " << static_cast<uint16_t>(promise.wait(waitScope).getError()) << std::endl;
  }
	return 0;
}

