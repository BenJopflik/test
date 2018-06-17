#pragma once

#include <cstdint>

class Client
{
        public:
                Client(uint64_t amount = 0) : m_amount(amount) {}
                ~Client() = default;
                
                uint64_t balance() const 
                {
                        std::shared_lock<std::shared_mutex> lock(m_mutex);
                        return m_amount;
                }

                bool update_balance(int64_t diff)
                {
                        std::unique_lock<std::shared_mutex> lock(m_mutex);

                    // TODO Add lock. can be called from different threds
                    if (diff < 0 && m_amount < diff) 
                            return false;
                    m_amount += diff;
                    return true;
                }

        private:
                mutable std::shared_mutex m_mutex;
                uint64_t m_amount = 0;
};
