#pragma once

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
    ShadowVmtHook() {} //delete later

    ShadowVmtHook(const void* ptr_to_class, size_t vtableSize, size_t amountPassed, ...);

    //ShadowVmtHook(const ShadowVmtHook&) = delete;
    //ShadowVmtHook& operator=(const ShadowVmtHook&) = delete;

    ~ShadowVmtHook();
};