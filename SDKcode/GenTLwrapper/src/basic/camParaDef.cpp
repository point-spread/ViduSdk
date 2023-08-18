/*
 * @Author: Kian Liu
 * @Date: 2022-03-07 20:04:29
 * @LastEditTime: 2022-04-14 17:08:03
 * @LastEditors: Kian Liu
 * @Description:
 * @FilePath: /DYV_SDK/GenTLwrapper/basic/src/camParaDef.cpp
 */
#include "GenTLwrapper/basic/camParaDef.h"

void print_extrinsics(extrinsics *pExtrin)
{
    printf("R|T \n");
    printf("%f, %f, %f, %f\n", pExtrin->rotation[0], pExtrin->rotation[1], pExtrin->rotation[2],
           pExtrin->translation[0]);
    printf("%f, %f, %f, %f\n", pExtrin->rotation[3], pExtrin->rotation[4], pExtrin->rotation[5],
           pExtrin->translation[1]);
    printf("%f, %f, %f, %f\n", pExtrin->rotation[6], pExtrin->rotation[7], pExtrin->rotation[8],
           pExtrin->translation[2]);
    printf("%f, %f, %f, %f\n", 0.0, 0.0, 0.0, 1.0);
}

void print_intrinsics(intrinsics *pIntrin)
{
    printf("width %d, height %d\n", pIntrin->width, pIntrin->height);
    printf("K: \n   %f, %f, %f,\n   %f, %f, %f,\n   %f, %f, %f\n", pIntrin->fx, 0.0, pIntrin->cx, 0.0, pIntrin->fy,
           pIntrin->cy, 0.0, 0.0, 1.0);

    printf("distortion:\n");
    const char *modelName[distortion::DISTORTION_COUNT] = {"UNKNOWN",       "BROWN",  "MODIFIED_BROWN",
                                                           "INVERSE_BROWN", "FTHETA", "KANNALA_BRANDT4"};

    if (pIntrin->model >= distortion::DISTORTION_COUNT)
    {
        pIntrin->model = distortion::DISTORTION_UNKNOWN;
    }
    printf("   model: %s\n", modelName[pIntrin->model]);
    if (pIntrin->model == distortion::DISTORTION_BROWN_CONRADY)
    {
        printf("   distortion k1: %f, k2: %f, k3: %f, p1: %f, p2: %f\n", pIntrin->coeffs[0], pIntrin->coeffs[1],
               pIntrin->coeffs[4], pIntrin->coeffs[2], pIntrin->coeffs[3]);
    }
    else if (pIntrin->model == distortion::DISTORTION_KANNALA_BRANDT4)
    {
        printf("   distortion k1: %f, k2: %f, k3: %f, k4: %f\n", pIntrin->coeffs[0], pIntrin->coeffs[1],
               pIntrin->coeffs[2], pIntrin->coeffs[3]);
    }
}
