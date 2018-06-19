#pragma once

#include <array>
#include <cstring>

class ClientId
{
public:
    explicit ClientId(const uint8_t * id)
    {
        std::memcpy((uint8_t *)m_id.data(), id, m_id.size() * sizeof(decltype(m_id)::value_type));
    }

    bool operator == (const ClientId & right) const
    {
        return m_id == right.m_id;
    }

    uint64_t hash() const
    {
        // replace with a normal hash function
        return m_id[0] ^ m_id[1] ^ m_id[2] ^ m_id[3];
    }

private:
    std::array<uint64_t, 4> m_id;
};
