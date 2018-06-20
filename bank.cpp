#include "bank.hpp"

auto Bank::transfer(const uint8_t * src_id, const uint8_t * dst_id, uint64_t amount) -> ErrorCode
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
            return ErrorCode::INVALID_DST;
        dst = dst_iter->second;
    } 
    // ? how should it act if a user become deleted while this operation
    if (!src->withdraw(amount))
        return ErrorCode::INSUFFICIENT_FUNDS;
    
    dst->deposit(amount);

    return ErrorCode::OK;
}


auto Bank::create_user(const uint8_t * src_id, uint64_t amount) -> ErrorCode
{
    std::unique_lock<std::shared_mutex> lock(m_clients_mutex);

    if (!m_clients.emplace(ClientId(src_id), std::make_shared<Client>(amount)).second)
        return ErrorCode::ALREADY_EXISTS;

    return ErrorCode::OK; 
}

auto Bank::delete_user(const uint8_t * src_id) -> ErrorCode
{
    std::unique_lock<std::shared_mutex> lock(m_clients_mutex);

    m_clients.erase(ClientId(src_id));

    return ErrorCode::OK; 
}
