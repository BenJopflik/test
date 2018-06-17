#pragma once

#include <array>
#include <cstring>

class ClientId
{
        public:
                explicit ClientId(const uint8_t * id, bool store = false) : m_ptr(store ? m_id.data() : id)
                {
                        if (store)
                                std::memcpy(m_id.data(), id, m_id.size());
                }

                bool operator == (const ClientId & right) const
                {
                    for (int i = 0; i < 32; ++i)           
                            if (m_ptr[i] != right.m_ptr[i])
                                    return false;
                    return true;
                }
                
                uint64_t hash() const
                {
                        // replace with a normal hash function
                    const uint64_t * val = reinterpret_cast<const uint64_t *>(m_id.data());
                    return val[0] ^ val[1] ^ val[2] ^ val[3];
                }

        private:
                std::array<uint8_t, 32> m_id;
                const uint8_t * m_ptr;
};
