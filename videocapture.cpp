#include "videocapture.h"

VideoCapture::VideoCapture(int _width, int _height, AVPixelFormat _pix_fmt, int _fps)
    :width(_width), height(_height), fps(_fps), pix_fmt(_pix_fmt)
{

}

VideoCapture::~VideoCapture()
{

}

int VideoCapture::VideoCaptureInit()
{
    //1、创建frame

    //2、创建packet
    return 0;
}

int VideoCapture::OpenVideoDevice()
{
    int ret = 0;
    AVInputFormat *inputFmt = av_find_input_format(libName.c_str());
    if (!inputFmt) {
        av_log(NULL, AV_LOG_ERROR, "find lib %s failure.\n", libName.c_str());
        return -1;
    }

    ret = avformat_open_input(&fmt_ctx, AudioDeviceInfo.toUtf8().data(), inputFmt, NULL);
    if (ret != 0) {
        av_strerror(ret, error, 128);
        av_log(NULL, AV_LOG_ERROR, "open input failure.[%d][%s]\n", AVERROR(ret), error);
        return -1;
    }
    av_dump_format(fmt_ctx, 0, AudioDeviceInfo.toUtf8().data(), 0);
    return 0;
}

int VideoCapture::CreateEncFrame()
{
    int ret = -1;
    enc_frame = av_frame_alloc();
    if (!enc_frame)
    {
        printf("Error, No Memory!\n");
        return ret;
    }

    //设置参数
    enc_frame->width = width;
    enc_frame->height = height;
    enc_frame->format = pix_fmt;

    //alloc inner memory
    ret = av_frame_get_buffer(enc_frame, 32); //按32位对齐 【视频必须是32位对齐】
    if (ret < 0)
    {
        printf("Error, Failed to alloc buffer for frame!\n");
        return ret;
    }

    return ret;
}
