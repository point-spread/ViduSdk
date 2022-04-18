/*
 * @Author: Kian Liu
 * @Date: 2021-11-05 16:05:01
 * @LastEditTime: 2021-11-12 10:23:18
 * @LastEditors: Kian Liu
 * @Description:
 * @FilePath: /DYV_SDK/src/common/stringFormat.h
 */
#pragma once
#include <string>
std::string stringFormat(const std::string fmt_str, ...);

template <typename I> std::string n2hexstr(I w, size_t hex_len = sizeof(I) << 1)
{
    static const char *digits = "0123456789ABCDEF";
    std::string rc(hex_len, '0');
    for (size_t i = 0, j = (hex_len - 1) * 4; i < hex_len; ++i, j -= 4)
        rc[i] = digits[(w >> j) & 0x0f];
    return rc;
}
