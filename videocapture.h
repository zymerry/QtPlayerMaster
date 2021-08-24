#ifndef VIDEOCAPTURE_H
#define VIDEOCAPTURE_H

#include "deviceinfo.h"
#include "ffmpeg_media.h"

class VideoCapture : public DeviceInfo
{
public:
    VideoCapture(int _width = 1280, int _height = 720, AVPixelFormat _pix_fmt = AV_PIX_FMT_YUYV422, int _fps = 30);
    ~VideoCapture();

    int VideoCaptureInit();
    int VideoCaptureDeinit();
    int VideoCaptureFrame();

private:
    int CreateEncFrame();
    int DestoryEncFrame();
    int OpenVideoDevice();
    int CloseVideoDevice();

private:
    AVFormatContext* fmt_ctx;
    AVFrame* enc_frame;
    AVPacket* pkt;
    int width;
    int height;
    int fps;
    AVPixelFormat pix_fmt;
    std::string libName {"dshow"}; //windows下使用的dshow采集,mac下使用avfundation
private:
    char error[1024] {0}; //用于错误打印
};

#endif // VIDEOCAPTURE_H
