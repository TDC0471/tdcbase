//#pragma once
//#include <string>
//#include <cstdarg>
//#include <source_location>
//
//namespace c {
//
//    struct printData
//    {
//        printData(printType&& type, std::source_location location = std::source_location::current())  : type(type), location(location) {}
//
//        printType type;
//		std::source_location location;
//	};
//
//    enum class printType 
//    {
//        normal,
//		error,
//		warning,
//		success
//    };
//
//    int printf(printData&& printData, const char* fmt, ...); //make this type safe?
//
//    std::string* get();
//}

#pragma once
#include <cstdarg>
#include <string>

namespace c {

    int printf(const char* fmt, ...);

    std::string* get();
}