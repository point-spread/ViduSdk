#pragma once
#include "GenICam/GenTL.h"

#include <cstdint>

class PDimage
{
  public:
    PDimage();

    /**
     * @brief use data fill image
     *
     * @param[in] data image data
     * @param[in] width image width
     * @param[in] height image height
     * @param[in] pixel_format image pixel format, use PFNC
     */
    void FillImgae(void *data, uint32_t width, uint32_t height, uint64_t pixel_format);

    /**
     * @brief get image data
     *
     * @return void * image data
     */
    void *GetImageData() const;

    /**
     * @brief get image width
     *
     * @return uint32_t image width
     */
    uint32_t GetImageWidth() const;

    /**
     * @brief get image height
     *
     * @return uint32_t image height
     */
    uint32_t GetImageHeight() const;

    /**
     * @brief get image pixel format
     *
     * @return uint64_t image pixel format, definition of using PFNC
     */
    uint64_t GetImagePixelFormat() const;

    /**
     * @brief get image type
     *
     * @return int32_t image type, definition of using OpenCV
     */
    int32_t GetImageCVType() const;

  private:
    void *data_;

    uint32_t width_;
    uint32_t height_;
    uint64_t pixel_format_;
};
