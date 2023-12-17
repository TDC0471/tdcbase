#pragma once
#include <string>
#include <cstdarg>

namespace c {
#define MAXPRINTSIZE 1024
    std::string console{};

    int printf(const char* fmt, ...) {
        va_list args;
        va_start(args, fmt);
        char buffer[MAXPRINTSIZE];
        int ret = vsnprintf(buffer, sizeof(buffer), fmt, args);
        va_end(args);

        if (ret >= 0) {
            console += buffer;
        }

        return ret;
    }
}