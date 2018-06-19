#pragma once

#include "build/bank.capnp.h"



template <typename BankImpl>
class ClientManagerProcessor final : public ::ClientManager::Server
{
public:
    ClientManagerProcessor(std::shared_ptr<BankImpl> bank_impl) : m_bank_impl(bank_impl)
    {
        
    }

    ~ClientManagerProcessor() = default;

    ::kj::Promise<void> createUser(CreateUserContext context) override
    {
        auto return_result = [&context](::ClientManager::ErrorCode error) {context.getResults().setError(error); return ::kj::READY_NOW;};
    
        auto params = context.getParams();
        if (!params.hasId())
            return return_result(::ClientManager::ErrorCode::MALFORMED);
        
        auto id = params.getId();
        static const size_t ID_SIZE = 32; // TODO move it from here
        if (id.size() != ID_SIZE)
            return return_result(::ClientManager::ErrorCode::MALFORMED);

        auto amount = params.getAmount();
        auto result = m_bank_impl->create_user(id.begin(), amount);

        return return_result(to_proto_error_code(result));
    }

    ::kj::Promise<void> deleteUser(DeleteUserContext context) override
    {
        auto return_result = [&context](::ClientManager::ErrorCode error) {context.getResults().setError(error); return ::kj::READY_NOW;};
    
        auto params = context.getParams();
        if (!params.hasId())
            return return_result(::ClientManager::ErrorCode::MALFORMED);
        
        auto id = params.getId();
        static const size_t ID_SIZE = 32; // TODO move it from here
        if (id.size() != ID_SIZE)
            return return_result(::ClientManager::ErrorCode::MALFORMED);
        
        auto result = m_bank_impl->delete_user(id.begin());

        return return_result(to_proto_error_code(result));
    }

private:
    ::ClientManager::ErrorCode to_proto_error_code(const typename BankImpl::ErrorCode code) const
    {
        switch (code)	
        {
            case BankImpl::ErrorCode::OK:
                return ::ClientManager::ErrorCode::OK;

            case BankImpl::ErrorCode::ALREADY_EXISTS:
                return ::ClientManager::ErrorCode::ALREADY_EXISTS;

            default:
                throw std::runtime_error("unknown error code"); // TODO refact kj::throw
        }
    }

private:
    std::shared_ptr<BankImpl> m_bank_impl;

};
