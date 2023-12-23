#pragma once
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <vector>
#include <memory>

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
		using Function = Return(*)(void*, Arguments...);
		return (reinterpret_cast<Function>(address))(classInstance, args...);
	}


	// get module name from address
	std::unique_ptr<char[]> GetModuleName(void* address) noexcept;

	// get void pointer to virtual function @ given index
	constexpr void* Get(void* vmt, const std::uint32_t index) noexcept
	{
		return (*static_cast<void***>(vmt))[index];
	}

	template <typename T>
	T read(void* address)
	{
		return *reinterpret_cast<T*>(address);
	}

	template <typename T>
	void write(void* address, T value)
	{
		*reinterpret_cast<T*>(address) = value;
	}

	void writeBytes(void* destination, const void* source, size_t size);

	BYTE* PatternScan(LPCSTR moduleName, const char* pattern) noexcept;


}
