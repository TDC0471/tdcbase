#include "console.h"


#define MAXPRINTSIZE 1024
std::string console{};

 int c::printf(const char* fmt, ...)
 {
     va_list args;

     va_start(args, fmt);
     vprintf(fmt, args);
     va_end(args);

     va_start(args, fmt);
     char buffer[MAXPRINTSIZE];
     int ret = vsnprintf(buffer, sizeof(buffer), fmt, args);
     va_end(args);

     if (ret >= 0) {
         console += buffer;
     }



     return ret;
 }

 std::string* c::get()
 {
     return &console;
 }
