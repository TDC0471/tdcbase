//#include "console.h"
//#include "imguiincl.h"
//
///*
//        normal,
//        error,
//        warning,
//        success
//*/
//
//#define MAXPRINTSIZE 1024
//std::string console{};
//
// int c::printf(printData&& printData, const char* fmt, ...)
// {
//     ImColor color{};
//     switch (printData.type)
//     {
//     default:
//         break;
//     } //set color (maybe do this in printData constructor?)
//
//     //printData.location.file_name() //get file name from source_location
//
//     va_list args;
//
//     va_start(args, fmt);
//     vprintf(fmt, args);
//     va_end(args);
//
//     va_start(args, fmt);
//     char buffer[MAXPRINTSIZE];
//     int ret = vsnprintf(buffer, sizeof(buffer), fmt, args);
//     va_end(args);
//
//     if (ret >= 0) {
//         console += buffer;
//     }
//
//
//
//     return ret;
// }
//
// std::string* c::get()
// {
//     return &console;
// }

#include "console.h"


#define MAXPRINTSIZE 1024
std::string console{};

int c::printf(const char* fmt, ...)
{
    va_list args;

#ifdef DEBUG
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args); 
#endif // DEBUG

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