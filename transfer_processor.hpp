#pragma once

#include "build/bank.capnp.h"

#include <kj/debug.h>
#include <capnp/ez-rpc.h>
#include <capnp/message.h>
#include <memory>

template <typename BankImpl>
class TransactionProcessor final : public ::Transfer::Server
{
public:
    TransactionProcessor(std::shared_ptr<BankImpl> bank_impl) : m_bank_impl(bank_impl) 
    {}
    
    
    ::kj::Promise<void> transfer(TransferContext context) override
    {
        auto return_result = [&context](::Transfer::ErrorCode error) {context.getResults().setError(error); return ::kj::READY_NOW;};

        auto params = context.getParams();
        if (!params.hasSrc() || !params.hasDst())
            return return_result(::Transfer::ErrorCode::MALFORMED);

        auto src = params.getSrc();
        auto dst = params.getDst();
        static const size_t ID_SIZE = 32; // TODO move it from here
        if (src.size() != ID_SIZE || dst.size() != ID_SIZE)
            return return_result(::Transfer::ErrorCode::MALFORMED);
        
        if (src == dst)
            return return_result(::Transfer::ErrorCode::INVALID_DST);

        auto amount = params.getAmount();
        auto result = m_bank_impl->transfer(src.begin(), dst.begin(), amount);

        return return_result(to_proto_error_code(result));
    }

    ::Transfer::ErrorCode to_proto_error_code(const typename BankImpl::ErrorCode code) const
    {
        switch (code)	
        {
            case BankImpl::ErrorCode::OK:
                return ::Transfer::ErrorCode::OK;

            case BankImpl::ErrorCode::INVALID_SRC:
                return ::Transfer::ErrorCode::INVALID_SRC;

            case BankImpl::ErrorCode::INVALID_DST:
                return ::Transfer::ErrorCode::INVALID_DST;

            case BankImpl::ErrorCode::INSUFFICIENT_FUNDS:
                return ::Transfer::ErrorCode::INSUFFICIENT_FUNDS;

            default:
                throw std::runtime_error("unknown error code"); // TODO refact kj::throw
        }
    }

private:
    std::shared_ptr<BankImpl> m_bank_impl;

};


