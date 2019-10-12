#ifndef LZ4IMAGEDECOMPRESSOR_H
#define LZ4IMAGEDECOMPRESSOR_H
#include "iimagedecompressor.h"

#include <utility>
#include <lz4.h>
namespace comp
{
    #define QUALITY "JPEG_QUALITY"
}

class lz4ImageDecompressor : public IImageDecompressor
{
public:
    lz4ImageDecompressor();
    virtual ~lz4ImageDecompressor();

    virtual void setParameters(parameterMap& para);

    inline virtual cv::Mat decompress(uint8_t* ptrDat,dataHeaderHandling::dataHeader hdr, bool& ok)
    {
        cv::Mat img;
        ok = decompressHelper(img,ptrDat,hdr);
        return img;
    }
    inline virtual cv::Mat decompress(uint8_t* ptrDat,dataHeaderHandling::dataHeader hdr)
    {
        cv::Mat img;
        decompressHelper(img,ptrDat,hdr);
        return img;
    }

    inline bool decompressHelper(cv::Mat& img,uint8_t* ptrDat,dataHeaderHandling::dataHeader hdr)
    {
        img = cv::Mat (hdr.height,hdr.width,hdr.cvType);
        int dst_size = img.rows*img.cols*img.channels();
        int decSize = LZ4_decompress_safe(reinterpret_cast<char*>(ptrDat),reinterpret_cast<char*>(img.data),hdr.length,dst_size);
        return decSize==dst_size;
    }
private:

};

#endif // LZ4IMAGEDECOMPRESSOR_H
