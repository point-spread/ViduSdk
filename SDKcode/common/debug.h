/*
 * @Author: Kian Liu
 * @Date: 2021-11-10 21:11:40
 * @LastEditTime: 2022-04-14 10:33:37
 * @LastEditors: Kian Liu
 * @Description:
 * @FilePath: /DYV_SDK/common/debug.h
 */
#pragma once
#include <stdio.h>
#ifdef __linux__
#define ANSI_COLOR_RED "\033[0;32;31m"
#define ANSI_COLOR_GREEN "\033[0;32;32m"
#define ANSI_COLOR_YELLOW "\033[1;33m"
#define ANSI_COLOR_END "\033[m"
#define CONSOLE_COLOR_SET(x)
#define FOREGROUND_RED 1
#define FOREGROUND_YELLOW 2
#define FOREGROUND_GREEN 3
#endif

#ifdef _WIN32
#include <windows.h>
static HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
#define CONSOLE_COLOR_SET(x) SetConsoleTextAttribute(hConsole, (x))
#define ANSI_COLOR_RED
#define ANSI_COLOR_GREEN
#define ANSI_COLOR_YELLOW
#define ANSI_COLOR_END
#define FOREGROUND_YELLOW (FOREGROUND_GREEN | FOREGROUND_RED)
#undef min
#undef max
#endif

#ifdef ENABLE_PD_DEBUG
#define PD_ERROR(fmt, ...)                                                                                  \
    do                                                                                                      \
    {                                                                                                       \
        CONSOLE_COLOR_SET(FOREGROUND_RED);                                                                  \
        fprintf(stderr, ANSI_COLOR_RED "[ERROR]%s:%d (Func %s): \n" fmt ANSI_COLOR_END, __FILE__, __LINE__, \
                __FUNCTION__, ##__VA_ARGS__);                                                               \
    } while (0)
#else
#define PD_ERROR(fmt, ...)                                                                                       \
    do                                                                                                           \
    {                                                                                                            \
        CONSOLE_COLOR_SET(FOREGROUND_RED);                                                                       \
        fprintf(stderr, ANSI_COLOR_RED "[ERROR] (Func %s): \n" fmt ANSI_COLOR_END, __FUNCTION__, ##__VA_ARGS__); \
    } while (0)
#endif

#ifdef ENABLE_PD_DEBUG
#define PD_WARNING(fmt, ...)                                                                                  \
    do                                                                                                        \
    {                                                                                                         \
        CONSOLE_COLOR_SET(FOREGROUND_YELLOW);                                                                 \
        fprintf(stderr, ANSI_COLOR_YELLOW "[WARN]%s:%d (Func %s): \n" fmt ANSI_COLOR_END, __FILE__, __LINE__, \
                __FUNCTION__, ##__VA_ARGS__);                                                                 \
    } while (0)
#else
#define PD_WARNING(fmt, ...)                                                                                       \
    do                                                                                                             \
    {                                                                                                              \
        CONSOLE_COLOR_SET(FOREGROUND_YELLOW);                                                                      \
        fprintf(stderr, ANSI_COLOR_YELLOW "[WARN] (Func %s): \n" fmt ANSI_COLOR_END, __FUNCTION__, ##__VA_ARGS__); \
    } while (0)
#endif

#ifdef ENABLE_PD_DEBUG
#define PD_INFO(fmt, ...)                                                                                    \
    do                                                                                                       \
    {                                                                                                        \
        CONSOLE_COLOR_SET(FOREGROUND_GREEN);                                                                 \
        fprintf(stderr, ANSI_COLOR_GREEN "[INFO]%s:%d (Func %s): \n" fmt ANSI_COLOR_END, __FILE__, __LINE__, \
                __FUNCTION__, ##__VA_ARGS__);                                                                \
    } while (0)
#else
#define PD_INFO(fmt, ...)                                                                                         \
    do                                                                                                            \
    {                                                                                                             \
        CONSOLE_COLOR_SET(FOREGROUND_GREEN);                                                                      \
        fprintf(stderr, ANSI_COLOR_GREEN "[INFO] (Func %s): \n" fmt ANSI_COLOR_END, __FUNCTION__, ##__VA_ARGS__); \
    } while (0)
#endif

#define SHOW_TIME(fmt, ...)                                                                                    \
    do                                                                                                         \
    {                                                                                                          \
        extern unsigned long long gLatestTime;                                                                 \
        timeval tp;                                                                                            \
        gettimeofday(&tp, NULL);                                                                               \
        unsigned long long now = tp.tv_sec * 1000000 + tp.tv_usec;                                             \
        if (gLatestTime != 0)                                                                                  \
        {                                                                                                      \
            fprintf(stdout, ">>>>>>>>>Used Time: %s[%d], %s: %ld.%ld, %llu ms ", __FILE__, __LINE__, __func__, \
                    tp.tv_sec, tp.tv_usec, (now - gLatestTime)\1000);                                          \
            fprintf(stdout, fmt "\n", ##__VA_ARGS__);                                                          \
        }                                                                                                      \
        gLatestTime = now;                                                                                     \
    } while (0)

#ifdef ENABLE_PD_DEBUG
#define PD_DBG(fmt, ...)                                                                           \
    do                                                                                             \
    {                                                                                              \
        printf("[DEBUG]%s:%d (Func %s): \n" fmt, __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__); \
        fflush(nullptr);                                                                           \
    } while (0)
#else
#define PD_DBG(...)
#endif
