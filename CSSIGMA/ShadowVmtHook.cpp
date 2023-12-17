#include "ShadowVmtHook.h"
#include <Windows.h>
#include <cstdarg>
#include <memoryapi.h>
#include <cstring>

ShadowVmtHook::ShadowVmtHook(const void* ptr_to_class, size_t vtableSize, size_t amountPassed, ...)
{
    va_list args;
    va_start(args, amountPassed);

    baseclass = const_cast<void*>(ptr_to_class); //just doing this so the functions sig is a bit more clear

    newvmt = reinterpret_cast<void**>(VirtualAlloc(NULL, vtableSize * sizeof(void*), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE));
    void** vtable = *reinterpret_cast<void***>(baseclass);

    std::memcpy(newvmt, vtable, vtableSize * sizeof(void*));

    for (size_t i = 0; i < amountPassed; i++)
    {
        VirtualMethod j = va_arg(args, VirtualMethod);
        newvmt[j.index] = j.fn;
    }


    //set the vtable pointer to our new vtable
    *reinterpret_cast<void***>(baseclass) = newvmt;

    va_end(args);

    oldvmt = vtable;
}

ShadowVmtHook::~ShadowVmtHook()
{
    *reinterpret_cast<void***>(baseclass) = oldvmt;
    VirtualFree(newvmt, 0, MEM_RELEASE);
}
