#include "videoencode.h"

VideoEncode::VideoEncode(int _width, int _height, AVPixelFormat _enc_pix_fmt, int _profile, int _level)
                        :width(_width), height(_height), enc_pix_fmt(_enc_pix_fmt), profile(_profile), level(_level)
{

}

VideoEncode::~VideoEncode()
{

}

int VideoEncode::OpenVideoEncode()
{
    int ret = 0;
    AVCodec* codec = NULL;

    codec = avcodec_find_encoder_by_name(encoder_lib.c_str());
    if (!codec)
    {
        printf("Codec libx264 not found\n");
        exit(1);
    }

    enc_ctx =  avcodec_alloc_context3(codec);
    if (!enc_ctx)
    {
        av_log(NULL, AV_LOG_ERROR, "Counld out allocate video codec context\n\n");
        return ret;
    }

    //SPS
    enc_ctx->profile = profile;
    enc_ctx->level = level; //50表示LEVELE是5.0

    //设置分辨率
    enc_ctx->width = width;
    enc_ctx->height = height;

    //GOP
    enc_ctx->gop_size = 250;
    //图像有很多变化的时候可以自动插入一个I帧，即最小的gop size
    enc_ctx->keyint_min = 25; //option 可选

    //设置B帧的数量
    enc_ctx->max_b_frames = 3;//option 可选
    enc_ctx->has_b_frames = 1;//option 可选

    //设置参考帧的数量
    enc_ctx->refcounted_frames = 3; //option 可选

    //输入的YUV格式
    enc_ctx->pix_fmt = enc_pix_fmt;

    //设置码率
    enc_ctx->bit_rate = 400000; //600kbps

    //设置帧率
    AVRational time_base_t = { 1, 25 };
    AVRational framerate_t = { 25, 1 };
    enc_ctx->time_base = time_base_t; //帧与帧之间的间隔是time_base
    enc_ctx->framerate = framerate_t;  //帧率, 每秒25帧

    ret = avcodec_open2(enc_ctx, codec, NULL);
    if (ret < 0)
    {
        av_log(NULL, AV_LOG_ERROR, "Counld not open codec: %d\n", ret);
        return ret;
    }

    enc_pkt = av_packet_alloc();
    av_init_packet(enc_pkt);

    return ret;
}

int VideoEncode::CloseVideoEncode()
{
    avcodec_close(enc_ctx);
    avcodec_free_context(&enc_ctx);
}

int VideoEncode::VideoEncodePkt(AVFrame *frame, AVPacket **out_pkt)
{
    int ret = -1;
    if (frame)
        printf("send frame to encoder, pts = %lld\n", frame->pts);
    //把AVFrame传给编码器，送原始数据给编码器进行编码
    ret = avcodec_send_frame(enc_ctx, frame);
    if (ret < 0)
    {
        av_log(NULL, AV_LOG_ERROR, "avcodec_send_frame error!\n");
        return ret;
    }

    //从编码器获取编码好的数据
    //从编码器获取编码是连续输出的，有可能喂frame数据给编码器的是编码器并不输出
    //有可能一次输出好几个编码好的数据。
    while (ret >= 0)
    {
        ret = avcodec_receive_packet(enc_ctx, enc_pkt);
        //编码器数据不足时，返回EAGAIN，或者到数据结尾时返回AVERROR_EOF
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
            break;
        if (ret < 0) {
            av_log(NULL, AV_LOG_ERROR, "error, failed to encode.\n");
            break;
        }
        //outfile输出h264的编码
        *out_pkt = enc_pkt;
        //av_packet_unref(newpkt);//减少newpkt的引用计数
    }
    return ret;
}

//调用VideoEncodePkt后需调用该函数，减少enc_pkt的引用计数
void VideoEncode::UnrefEncPkt()
{
    av_packet_unref(enc_pkt);
}
