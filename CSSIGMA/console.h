#pragma once
#include <string>
#include <cstdarg>

namespace c {

    int printf(const char* fmt, ...);

    std::string* get();
}