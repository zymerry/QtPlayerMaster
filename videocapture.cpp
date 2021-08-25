#include "videocapture.h"

VideoCapture::VideoCapture(int _width, int _height, AVPixelFormat _pix_fmt, int _fps)
    :width(_width), height(_height), fps(_fps), cap_pix_fmt(_pix_fmt), out_pix_fmt(AV_PIX_FMT_YUV420P)
{

}

VideoCapture::~VideoCapture()
{

}

int VideoCapture::VideoCaptureInit()
{
    //1、创建frame
    cap_frame = CreateFrame(width, height, cap_pix_fmt);
    sw_frame = CreateFrame(width, height, out_pix_fmt);

    //2、创建packet
    pkt = av_packet_alloc();
    av_init_packet(pkt);

    //3、初始化图像转换 yuyv422->yuv420p (因为采集的是yuyv422, h264编码时需要yuv420p)
    int sws_flags = SWS_BICUBIC; //差值算法,双三次
    //设置转换context
    if (img_convert_ctx == nullptr)
    {
        img_convert_ctx = sws_getContext(width, height,
            (AVPixelFormat)cap_pix_fmt,
            width,
            height,
            (AVPixelFormat)out_pix_fmt,
            sws_flags, NULL, NULL, NULL);
        if (img_convert_ctx == NULL)
        {
            av_log(NULL, AV_LOG_ERROR, "Cannot initialize the conversion context\n");
            return -1;
        }
    }

    //4、打开video device
    OpenVideoDevice();
    return 0;
}

int VideoCapture::VideoCaptureDeinit()
{
    if (cap_frame) {
        DestoryFrame(&cap_frame);
        cap_frame = nullptr;
    }
    if (sw_frame) {
        DestoryFrame(&sw_frame);
        sw_frame = nullptr;
    }

    av_packet_free(&pkt);

    CloseVideoDevice();
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

int VideoCapture::CloseVideoDevice()
{
    if (fmt_ctx) {
        avformat_close_input(&fmt_ctx);
    }
    return 0;
}

int VideoCapture::VideoCaptureFrame(AVFrame **out_frame)
{
        int ret = 0;
        int numBytes = 0;
        uint8_t *out_buffer = nullptr;

        numBytes = avpicture_get_size(out_pix_fmt, width, width);
        out_buffer = (uint8_t *) av_malloc(numBytes * sizeof(uint8_t));

        //读取一帧数据到pkt
        av_read_frame(fmt_ctx, pkt);

        ret = avpicture_fill((AVPicture*)out_frame, out_buffer, (AVPixelFormat)out_pix_fmt, 1280, 720);
        ret = avpicture_fill((AVPicture*)cap_frame, (unsigned char*)pkt->data, (AVPixelFormat)cap_pix_fmt, 1280, 720);

        //yuyv422 -> yuv420p
        ret = sws_scale(img_convert_ctx, cap_frame->data, cap_frame->linesize,
            0, height, sw_frame->data, sw_frame->linesize);

        *out_frame = sw_frame;
}

AVFrame* VideoCapture::CreateFrame(int w, int h, AVPixelFormat pix_fmt)
{
    int ret = -1;
    AVFrame *frame = nullptr;
    frame = av_frame_alloc();
    if (!frame)
    {
        printf("Error, No Memory!\n");
        return nullptr;
    }

    //设置参数
    frame->width = w;
    frame->height = h;
    frame->format = pix_fmt;

    //alloc inner memory
    ret = av_frame_get_buffer(frame, 32); //按32位对齐 【视频必须是32位对齐】
    if (ret < 0)
    {
        printf("Error, Failed to alloc buffer for frame!\n");
        return nullptr;
    }

    return frame;
}

int VideoCapture::DestoryFrame(AVFrame **frame)
{
    av_frame_free(frame);
}
