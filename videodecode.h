#ifndef VIDEODEECODE_H
#define VIDEODEECODE_H

#include <string>
#include "ffmpeg_media.h"

class VideoDecode
{
public:
    VideoDecode(int _width = 1280, int _height = 720, AVPixelFormat _dec_pix_fmt = AV_PIX_FMT_YUV420P);
    ~VideoDecode();
    int OpenVideDecode();
    int VideoDecodeFrame(AVPacket *pkt, AVFrame **out_frame);

private:
    int CreateDecFrame();
private:
    int width;
    int height;
    AVPixelFormat dec_pix_fmt;
    AVCodecContext *dec_ctx;
    AVCodecID codec_id = AV_CODEC_ID_H264;
    AVFrame *dec_frame;
};

#endif // VIDEODEECODE_H
