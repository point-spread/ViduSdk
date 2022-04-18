/*
 * @Author: Kian Liu
 * @Date: 2021-11-09 20:15:00
 * @LastEditTime: 2021-11-09 20:29:14
 * @LastEditors: Kian Liu
 * @Description:
 * @FilePath: /DYV_SDK/src/common/stringFormat.cpp
 */
#include "stringFormat.h"
#include <cstring>
#include <memory>   // For std::unique_ptr
#include <stdarg.h> // For va_start, etc.

std::string stringFormat(const std::string fmt_str, ...)
{
    int final_n, n = ((int)fmt_str.size()) * 2; /* Reserve two times as much as the length of the fmt_str */
    std::unique_ptr<char[]> formatted;
    va_list ap;
    while (1)
    {
        formatted.reset(new char[n]); /* Wrap the plain char array into the unique_ptr */
        //strcpy_s(&formatted[0], fmt_str.size()+1, fmt_str.c_str());
        strcpy(&formatted[0], fmt_str.c_str());
        va_start(ap, fmt_str);
        final_n = vsnprintf(&formatted[0], n, fmt_str.c_str(), ap);
        va_end(ap);
        if (final_n < 0 || final_n >= n)
            n += abs(final_n - n + 1);
        else
            break;
    }
    return std::string(formatted.get());
}
