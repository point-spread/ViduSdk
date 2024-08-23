/*
 * @Author: Kian Liu
 * @Date: 2021-12-01 14:21:04
 * @LastEditTime: 2022-03-28 14:24:41
 * @LastEditors: Kian Liu
 * @Description:
 * @FilePath: /DYV_SDK/common/timeTest.h
 */
#pragma once
#include <chrono>
#include <stdio.h>
#include <time.h>
#include <utility>
#include <vector>
class timeTest
{
    std::vector<std::pair<int, uint64_t>> tp;

  public:
    timeTest()
    {
        tp.reserve(20);
    }
    void stamp(int lineNum)
    {
        tp.emplace_back(lineNum, std::chrono::duration_cast<std::chrono::microseconds>(
                                     std::chrono::high_resolution_clock::now().time_since_epoch())
                                     .count());
    };

    void report()
    {
        for (size_t i = 1; i < tp.size(); i++)
        {
            // printf("run time sec %zu line %d~line %d: %f ms\n", i, tp[i - 1].first, tp[i].first,
            //        (double)(tp[i].second - tp[i - 1].second) / 1000.0f);
        }
        tp.clear();
    }
};
