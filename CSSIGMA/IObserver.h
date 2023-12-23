#pragma once

template<typename... Args>
class IObserver
{
public:
	virtual void Update(Args...) noexcept = 0; //guaranteed noexcept
};