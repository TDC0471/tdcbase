#pragma once

namespace util
{
	template<typename T>
	void* funcToVoid(T func)
	{
		union
		{
			T fn;
			void* ptr;
		} u;
		u.fn = func;
		return u.ptr;
	}
}