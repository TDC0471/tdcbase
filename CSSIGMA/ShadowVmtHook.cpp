#include "ShadowVmtHook.h"
#include <Windows.h>
#include <memoryapi.h>
#include <cstring>

void* ShadowVmtHook::getOriginalFunction(size_t index)
{
    return oldvmt[index];
}

ShadowVmtHook::ShadowVmtHook(const void* ptr_to_class, size_t vtableSize, std::initializer_list<VirtualMethod>&& methods)
{

    baseclass = const_cast<void*>(ptr_to_class); //just doing this so the functions sig is a bit more clear

    newvmt = reinterpret_cast<void**>(malloc(sizeof(void*) * vtableSize));
    void** vtable = *reinterpret_cast<void***>(baseclass);

    std::memcpy(newvmt, vtable, vtableSize * sizeof(void*));

    for (VirtualMethod method : methods)
    {
        newvmt[method.index] = method.fn;
    }


    //set the vtable pointer to our new vtable
    *reinterpret_cast<void***>(baseclass) = newvmt;

    oldvmt = vtable;
}

void ShadowVmtHook::deleteHook()
{
    if (newvmt) {
        *reinterpret_cast<void***>(baseclass) = oldvmt;
        free(newvmt);
        newvmt = nullptr;
    }
}

ShadowVmtHook::~ShadowVmtHook()
{
    deleteHook();
}
