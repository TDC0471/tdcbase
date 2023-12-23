#pragma once
#include <vector>

struct VirtualMethod
{
    void* fn;
    unsigned int index;
};

class ShadowVmtHook
{
    void** oldvmt;
    void** newvmt;
    void* baseclass;

public:
    void* getOriginalFunction(size_t index);

    ShadowVmtHook() = default;
    ShadowVmtHook(const void* ptr_to_class, size_t vtableSize, std::initializer_list<VirtualMethod>&& methods); //could be constexpr

    ShadowVmtHook(const ShadowVmtHook&) = delete;
    ShadowVmtHook& operator=(const ShadowVmtHook&) = delete;

    //implement move constructor and assignment operator
    ShadowVmtHook(ShadowVmtHook&&) = default;
    ShadowVmtHook& operator=(ShadowVmtHook&&) = default;

    void deleteHook();

    ~ShadowVmtHook();
};