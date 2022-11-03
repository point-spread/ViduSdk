#pragma once
#include <vector>

template <typename T> bool purgeSptrVector(std::vector<T> &pList)
{
    size_t count = 0;
    for (auto &&pItem : pList)
    {
        if (pItem)
        {
            pList[count].swap(pItem);
            count++;
        }
    }
    pList.resize(count);
    return true;
}
