#pragma once
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <vector>

namespace memory
{

	// call virtual function @ given index
	template <typename Return, typename ... Arguments>
	constexpr Return VCall(void* vmt, const std::uint32_t index, Arguments ... args) noexcept
	{
		using Function = Return(__thiscall*)(void*, decltype(args)...);
		return (*static_cast<Function**>(vmt))[index](vmt, args...);
	}

	// call non-virtual function @ given address
	template <typename Return, typename... Arguments>
	constexpr Return OCall(void* classInstance, void* address, Arguments... args) noexcept {
		return ((Return(__thiscall*)(void*, Arguments...))address)(classInstance, args...);
	}


	// get void pointer to virtual function @ given index
	constexpr void* Get(void* vmt, const std::uint32_t index) noexcept
	{
		return (*static_cast<void***>(vmt))[index];
	}

	template <typename T>
	T read(uint32_t address)
	{
		return *reinterpret_cast<T*>(address);
	}

	template <typename T>
	void write(uint32_t address, T value)
	{
		*reinterpret_cast<T*>(address) = value;
	}

	void NOPout(uint32_t&& address, size_t size);

	void writeBytes(uint32_t&& destination, const void* source, size_t size);

	BYTE* PatternScan(LPCSTR moduleName, const char* pattern) noexcept;


}
