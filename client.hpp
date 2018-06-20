#pragma once

#include <cstdint>
#include <mutex>
#include "spinlock.hpp"

class Client
{
public:
    Client(uint64_t amount = 0) : m_amount(amount) {}
    ~Client() = default;

    uint64_t balance() const 
    {
        std::lock_guard<Spinlock> lock(m_spinlock);
        return m_amount;
    }

    bool withdraw(uint64_t amount)
    {
        std::lock_guard<Spinlock> lock(m_spinlock);
        if (m_amount < amount)
            return false;

        m_amount -= amount;

        return true;
    }

    void deposit(uint64_t amount)
    {
        std::lock_guard<Spinlock> lock(m_spinlock);
        m_amount += amount;	
    }

private:
    mutable Spinlock m_spinlock;
    uint64_t m_amount = 0;

};
