#ifndef VIDEOENCODE_H
#define VIDEOENCODE_H

#include <string>
#include "ffmpeg_media.h"

/*
** @brief VideoEncode 视频采集类 输入cap_frame 输出enc_pkt
** first call OpenVideoEncode() set video enc param and open video encoder, then call VideoEncodePkt(), get a packet data.
*/
class VideoEncode
{
public:
    VideoEncode(int _width = 1280, int _height = 720, AVPixelFormat _enc_pix_fmt = AV_PIX_FMT_YUV420P,
                int _profile = FF_PROFILE_H264_HIGH_444, int _level = 50);
    ~VideoEncode();
    int OpenVideoEncode();
    int CloseVideoEncode();
    int VideoEncodePkt(AVFrame *frame,AVPacket **out_pkt);
    void UnrefEncPkt();

private:
    AVCodecContext *enc_ctx;
    int width;
    int height;
    AVPixelFormat enc_pix_fmt;
    int profile;
    int level;
    AVPacket *enc_pkt;
    std::string encoder_lib{"libx264"};
};

#endif // VIDEOENCODE_H
