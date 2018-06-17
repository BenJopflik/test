#pragma once

#include <functional>
#include <shared_mutex>
#include <unordered_map>

#include "client.hpp"
#include "client_id.hpp"

namespace std
{

    template <>
    struct hash<ClientId>
    {
        size_t operator () (const ClientId & val) const
        {
            return val.hash();
        }
    };
}

class BankImpl final
{
        public:
        enum class ErrorCode : uint16_t
        {
            OK = 0,
            INVALID_SRC,
            INVALID_DST,
            INSUFFICIENT_FUNDS,
            ALREADY_EXISTS,
        };

        ErrorCode transfer(const uint8_t * src_id, const uint8_t * dst_id, uint64_t amount)
        {
            using Ptr = decltype(m_clients)::mapped_type;
            Ptr src;
            Ptr dst;

            {
                    std::shared_lock<std::shared_mutex> lock(m_clients_mutex);

                    auto src_iter = m_clients.find(ClientId(src_id));  
                    if (src_iter == m_clients.end())
                            return ErrorCode::INVALID_SRC;
                    src = src_iter->second;

                    auto dst_iter = m_clients.find(ClientId(dst_id));  
                    if (dst_iter == m_clients.end())
                            return ErrorCode::INVALID_SRC;
                    dst = dst_iter->second;
            } 

            if (!src->update_balance(amount))
                    return ErrorCode::INSUFFICIENT_FUNDS;
            
            return ErrorCode::OK;
        }


        ErrorCode create_user(const uint8_t * src_id, uint64_t amount)
        {
                std::unique_lock<std::shared_mutex> lock(m_clients_mutex);
                if (!m_clients.emplace(ClientId(src_id, true), std::make_shared<Client>(amount)).second)
                        return ErrorCode::ALREADY_EXISTS;
                return ErrorCode::OK; 
        }

        ErrorCode delete_user(const uint8_t * src_id)
        {
                std::unique_lock<std::shared_mutex> lock(m_clients_mutex);
                m_clients.erase(ClientId(src_id));
                return ErrorCode::OK; 
        }

        private:
            std::shared_mutex m_clients_mutex;
            std::unordered_map<ClientId, std::shared_ptr<Client>> m_clients;
};
