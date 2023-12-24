#pragma once
#include "IObserver.h"
#include <vector>
#include <mutex>

template<typename... Args>
class Event
{
    mutable std::mutex m_Mutex;
    std::vector<IObserver<Args...>*> m_Observers;

public:
    void Notify(Args... args) const noexcept {
        std::lock_guard<std::mutex> lock(m_Mutex);
        for (auto& observer : m_Observers)
            observer->Update(args...);
    }

    void AddObserver(IObserver<Args...>* observer) noexcept {
        std::lock_guard<std::mutex> lock(m_Mutex);
        m_Observers.push_back(observer);
    }

    void RemoveObserver(IObserver<Args...>* observer) noexcept {
        std::lock_guard<std::mutex> lock(m_Mutex);
        m_Observers.erase(std::remove(m_Observers.begin(), m_Observers.end(), observer), m_Observers.end());
    }
};
