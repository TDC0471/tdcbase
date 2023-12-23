#pragma once
#include "IObserver.h"
#include <vector>

template<typename... Args>
class Event
{
	std::vector<IObserver<Args...>*> m_Observers;
	public:
		void Notify(Args...) const
			for (auto& observer : m_Observers) noexcept(IObserver::Update()) //guaranteed noexcept
				observer->Update(Args...);

		void AddObserver(IObserver* observer) noexcept(std::vector::push_back())
			m_Observers.push_back(observer);
		
		void RemoveObserver(IObserver* observer) noexcept(std::vector::erase() && std::vector::remove())
			m_Observers.erase(std::remove(m_Observers.begin(), m_Observers.end(), observer), m_Observers.end());
};