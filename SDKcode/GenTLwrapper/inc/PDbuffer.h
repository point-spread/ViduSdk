/*
 * @Author: Kian Liu
 * @Date: 2022-04-27 22:37:39
 * @LastEditTime: 2022-04-28 00:10:31
 * @LastEditors: Kian Liu
 * @Description:
 * @FilePath: /DYV_SDK/GenTLwrapper/inc/PDbuffer.h
 */
#pragma once
#include "../inc/PDport.h"
#include <opencv2/opencv.hpp>

class sPDstream;

/**
 * @brief Wrapper of the GenTL buffer handle
 *
 */
class PDbuffer
{
    bool releaseMat(PDHandle pHandle);
    PDHandle hBuffer = nullptr;
    PDHandle hStream = nullptr; // hBuffer intern will maintain the life of stream
    bool setHanle();
    friend class sPDstream;
    std::vector<cv::Mat> Mats;

  public:
    PDbuffer(sPDstream *stream);

    operator PDHandle()
    {
        return hBuffer;
    }
    ~PDbuffer();
    /**
     * @brief Get the Mat Num
     *
     * @return uint32_t
     */
    uint32_t getMatNum();
    /**
     * @brief Get the Mat object
     *
     * @param id index of the mat, be sure the id is small than getMatNum()
     * @return const cv::Mat&
     */
    const cv::Mat &getMat(uint32_t id = 0);
};
