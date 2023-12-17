#include "memory.h"




void memory::NOPout(uint32_t&& address, size_t size)
{
    DWORD oldProtect;
    VirtualProtect((LPVOID)address, size, PAGE_EXECUTE_READWRITE, &oldProtect);
    memset((LPVOID)address, 0x90, size);
    VirtualProtect((LPVOID)address, size, oldProtect, &oldProtect);
}

void memory::writeBytes(uint32_t&& destination, const void* source, size_t size)
{
    LPVOID dest = reinterpret_cast<LPVOID>(destination);
    DWORD oldProtect;
    VirtualProtect(dest, size, PAGE_EXECUTE_READWRITE, &oldProtect);
    memcpy(dest, source, size);
    VirtualProtect(dest, size, oldProtect, &oldProtect);
}




BYTE* memory::PatternScan(LPCSTR moduleName, const char* pattern) noexcept
{
    constexpr static auto patternToByte = [](const char* pattern) noexcept -> const std::vector<std::int32_t>
    {
        std::vector<std::int32_t> bytes = std::vector<std::int32_t>{ };
        char* start = const_cast<char*>(pattern);
        const char* end = const_cast<char*>(pattern) + std::strlen(pattern);

        for (auto current = start; current < end; ++current)
        {
            if (*current == '?')
            {
                ++current;

                if (*current == '?')
                    ++current;

                bytes.push_back(-1);
            }
            else
                bytes.push_back(std::strtoul(current, &current, 16));
        }

        return bytes;
    };

    const HINSTANCE handle = GetModuleHandle(moduleName);

    if (!handle)
        return nullptr;

    const PIMAGE_DOS_HEADER dosHeader = reinterpret_cast<PIMAGE_DOS_HEADER>(handle);
    const PIMAGE_NT_HEADERS ntHeaders =
        reinterpret_cast<PIMAGE_NT_HEADERS>(reinterpret_cast<std::uint8_t*>(handle) + dosHeader->e_lfanew);

    const std::uintptr_t size = ntHeaders->OptionalHeader.SizeOfImage;
    const std::vector<std::int32_t> bytes = patternToByte(pattern);
    std::uint8_t* scanBytes = reinterpret_cast<std::uint8_t*>(handle);

    const std::size_t s = bytes.size();
    const std::int32_t* d = bytes.data();

    for (std::size_t i = 0ul; i < size - s; ++i)
    {
        bool found = true;

        for (std::size_t j = 0ul; j < s; ++j)
        {
            if (scanBytes[i + j] != d[j] && d[j] != -1)
            {
                found = false;
                break;
            }
        }

        if (found)
            return &scanBytes[i];
    }

    return nullptr;
}