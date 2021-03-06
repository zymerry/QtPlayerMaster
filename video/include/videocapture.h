#ifndef VIDEOCAPTURE_H
#define VIDEOCAPTURE_H

#include "deviceinfo.h"
#include "ffmpeg_media.h"

/*
** @brief VideoCapture 视频采集类 输出cap_frame
** first call VideoCaptureInit() init video-capture param and open video-capture device, then call VideoCaptureFrame(), get a frame data.
** windows下需要使用【ffmpeg -list_options true -f dshow -i video="Integrated Webcam"】命令查看支持的采集参数
*/
class VideoCapture : public DeviceInfo
{
public:
    VideoCapture(int _width = 1280, int _height = 720, AVPixelFormat _pix_fmt = AV_PIX_FMT_YUYV422, int _fps = 30);
    ~VideoCapture();

    int VideoCaptureInit();
    int VideoCaptureDeinit();
    int VideoCaptureFrame(AVFrame **out_frame);

private:
    AVFrame* CreateFrame(int w, int h, AVPixelFormat pix_fmt);
    int DestoryFrame(AVFrame **frame);
    int OpenVideoDevice();
    int CloseVideoDevice();


private:
    AVFormatContext* fmt_ctx;
    AVFrame *cap_frame;
    AVFrame *sw_frame;
    AVPacket* pkt;
    int width;
    int height;
    int fps;
    AVPixelFormat cap_pix_fmt;
    AVPixelFormat out_pix_fmt;
    struct SwsContext* img_convert_ctx = nullptr; //用于图像格式转化
    std::string libName {"dshow"}; //windows下使用的dshow采集,mac下使用avfundation
private:
    char error[1024] {0}; //用于错误打印
};

#endif // VIDEOCAPTURE_H
