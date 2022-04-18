/*
 * @Author: Kian Liu
 * @Date: 2022-03-03 15:56:51
 * @LastEditTime: 2022-03-03 17:43:34
 * @LastEditors: Kian Liu
 * @Description:
 * @FilePath: /DYV_SDK/GenTLwrapper/inc/buffer2cvMat.h
 */
#pragma once
#include <opencv2/opencv.hpp>
#include <vector>

bool buffer2cvMat(void *buffer, void *stream, std::vector<cv::Mat> &Mats);
