/*
 * @Author: Kian Liu
 * @Date: 2022-04-27 22:37:39
 * @LastEditTime: 2022-06-02 16:35:55
 * @LastEditors: Kian Liu
 * @Description:
 * @FilePath: /DYV_SDK/GenTLwrapper/inc/PDbuffer.h
 */
#pragma once
#include "GenTLwrapper/PDimage.h"
#include "GenTLwrapper/PDport.h"

class sPDstream;

/**
 * @brief Wrapper of the GenTL buffer handle
 *
 */
class PDbuffer
{
    bool releaseImage(PDHandle pHandle);
    PDHandle hBuffer = nullptr;
    PDHandle hStream = nullptr; // hBuffer intern will maintain the life of stream
    bool setHanle();
    friend class sPDstream;
    std::vector<PDimage> images_;

  public:
    PDbuffer(sPDstream *stream);

    operator PDHandle()
    {
        return hBuffer;
    }

    void *getPort()
    {
        return hBuffer;
    }

    ~PDbuffer();
    /**
     * @brief Get the Image Num
     *
     * @return uint32_t
     */
    uint32_t getImageNum();
    /**
     * @brief Get the image object
     *
     * @param id index of the image, be sure the id is small than getImageNum()
     * @return const PDimage&
     */
    const PDimage &getImage(uint32_t id = 0);

    void *GetBuffer() const;
};
