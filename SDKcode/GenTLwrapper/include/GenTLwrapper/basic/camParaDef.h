/*
 * @Author: Kian Liu
 * @Date: 2022-03-04 19:58:39
 * @LastEditTime: 2022-04-22 14:27:05
 * @LastEditors: Kian Liu
 * @Description:
 * @FilePath: /DYV_SDK/GenTLwrapper/basic/inc/camParaDef.h
 */
#pragma once
#include "stdio.h"
#include "string.h"
#ifdef __cplusplus
extern "C"
{
#endif
    enum distortion
    {
        DISTORTION_UNKNOWN = 0,       /**< Rectilinear images. No distortion compensation required. */
        DISTORTION_BROWN_CONRADY = 1, /**< Unmodified Brown-Conrady distortion model */

        DISTORTION_MODIFIED_BROWN_CONRADY = 2, /**< Equivalent to Brown-Conrady distortion, except that tangential
                                                   distortion is applied to radially distorted points */
        DISTORTION_INVERSE_BROWN_CONRADY = 3,  /**< Equivalent to Brown-Conrady distortion, except undistorts image
                                                  instead  of distorting it */
        DISTORTION_FTHETA = 4,                 /**< F-Theta fish-eye distortion model */

        DISTORTION_KANNALA_BRANDT4 = 5, /**< Four parameter Kannala Brandt distortion model */

        DISTORTION_COUNT = 6, /**< Number of enumeration values. Not a valid input: intended to be used in for-loops. */
    };
#pragma pack(push, 1)
    typedef struct stream_intrinsics
    {
        int width;        /**< Width of the image in pixels */
        int height;       /**< Height of the image in pixels */
        float cx;         /**<Horizontal coordinate of the principal point of the image */
        float cy;         /**< Vertical coordinate of the principal point of the image */
        float fx;         /**< Focal length of the image plane, as a multiple of pixel width */
        float fy;         /**< Focal length of the image plane, as a multiple of pixel height */
        distortion model; /**< Distortion model of the image */
        float coeffs[5];  /**< Distortion coefficients. Order for Brown-Conrady: [k1, k2, p1, p2, k3]. Order for F-Theta
                             Fish-eye: [k1, k2, k3, k4, 0]. Other models are subject to their own interpretations */
        stream_intrinsics(float *Kdata, float *distortCoeffdata, int _width, int _height)
        {
            model = distortion::DISTORTION_BROWN_CONRADY;
            fx = Kdata[0];
            cx = Kdata[2];
            fy = Kdata[4];
            cy = Kdata[5];
            memcpy(coeffs, distortCoeffdata, 4 * sizeof(float));
            coeffs[4] = distortCoeffdata[4];
            width = _width;
            height = _height;
        }
        stream_intrinsics(int _width, int _height, float _cx, float _cy, float _fx, float _fy, distortion _model,
                          float k1, float k2, float p1, float p2, float k3)
            : width(_width), height(_height), cx(_cx), cy(_cy), fx(_fx), fy(_fy), model(_model)
        {
            coeffs[0] = k1;
            coeffs[1] = k2;
            coeffs[2] = p1;
            coeffs[3] = p2;
            coeffs[4] = k3;
        }
        stream_intrinsics()
        {
            memset(coeffs, 0, 5 * sizeof(float));
        }
        stream_intrinsics(const stream_intrinsics &intrics)
        {
            width = intrics.width;
            height = intrics.height;

            cx = intrics.cx;
            cy = intrics.cy;
            fx = intrics.fx;
            fy = intrics.fy;

            model = intrics.model;

            memcpy(coeffs, intrics.coeffs, 5 * sizeof(float));
        }
    } intrinsics;

    typedef struct stream_extrinsics
    {
        float rotation[9];    /**< Row-major 3x3 rotation matrix */
        float translation[3]; /**< Three-element translation vector, in meters */
    } extrinsics;
#pragma pack(pop)

    void print_intrinsics(intrinsics *pIntrin);
    void print_extrinsics(extrinsics *pExtrin);

#ifdef __cplusplus
}
#endif
