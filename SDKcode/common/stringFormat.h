/*
 * @Author: Kian Liu
 * @Date: 2021-11-05 16:05:01
 * @LastEditTime: 2021-11-12 10:23:18
 * @LastEditors: Kian Liu
 * @Description:
 * @FilePath: /DYV_SDK/src/common/stringFormat.h
 */
#pragma once
#include "stringFormat.h"
#include <cstring>
#include <memory>   // For std::unique_ptr
#include <stdarg.h> // For va_start, etc.
#include <string>

template <typename I> std::string n2hexstr(const I &w, size_t hex_len = sizeof(I) << 1)
{
    static const char *digits = "0123456789ABCDEF";
    std::string rc(hex_len, '0');
    for (size_t i = 0, j = (hex_len - 1) * 4; i < hex_len; ++i, j -= 4)
        rc[i] = digits[(w >> j) & 0x0f];
    return rc;
}

inline std::string stringFormat(const std::string fmt_str, ...)
{
    int final_n, n = ((int)fmt_str.size()) * 2; /* Reserve two times as much as the length of the fmt_str */
    std::unique_ptr<char[]> formatted;
    va_list ap;
    while (1)
    {
        formatted.reset(new char[n]); /* Wrap the plain char array into the unique_ptr */
        // strcpy_s(&formatted[0], fmt_str.size()+1, fmt_str.c_str());
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
