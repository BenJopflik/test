#pragma once

#include <functional>
#include <shared_mutex>
#include <unordered_map>
#include <sstream>

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
 
class Bank final
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

    ErrorCode transfer(const uint8_t * src_id, const uint8_t * dst_id, uint64_t amount);
    ErrorCode create_user(const uint8_t * src_id, uint64_t amount);
    ErrorCode delete_user(const uint8_t * src_id);

private:
    std::shared_mutex m_clients_mutex;
    std::unordered_map<ClientId, std::shared_ptr<Client>> m_clients;

};
