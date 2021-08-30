#include "videodecode.h"

VideoDecode::VideoDecode(int _width, int _height, AVPixelFormat _dec_pix_fmt)
                :width(_width), height(_height), dec_pix_fmt(_dec_pix_fmt)
{

}

VideoDecode::~VideoDecode()
{

}

int VideoDecode::OpenVideDecode()
{
    int ret = 0;
    AVCodec* codec = NULL;

    codec = avcodec_find_decoder(codec_id);
    if (!codec)
    {
        printf("Codec libx264 not found\n");
        exit(1);
    }

    dec_ctx = avcodec_alloc_context3(codec);
    if (!dec_ctx)
    {
        printf("Counld out allocate video codec context\n\n");
        exit(1);
    }

    //设置分辨率
    dec_ctx->width = width;
    dec_ctx->height = height;

    //YUV格式
    dec_ctx->pix_fmt = dec_pix_fmt;

    ret = avcodec_open2(dec_ctx, codec, NULL);
    if (ret < 0)
    {
        //printf("Counld not open codec: %s\n", av_err2str(ret));
        printf("Counld not open codec: %d\n", ret);
        exit(1);
    }

    CreateDecFrame();

    return ret;
}

int VideoDecode::CreateDecFrame()
{
    int ret = -1;
    dec_frame = av_frame_alloc();
    if (!dec_frame)
    {
        av_log(NULL, AV_LOG_ERROR, "Error, No Memory!\n");
        return ret;
    }

    //设置参数
    dec_frame->width = width;
    dec_frame->height = height;
    dec_frame->format = dec_pix_fmt;

    //alloc inner memory
    ret = av_frame_get_buffer(dec_frame, 32); //按32位对齐 【视频必须是32位对齐】
    if (ret < 0)
    {
        if (dec_frame)
            av_frame_free(&dec_frame);
        av_log(NULL, AV_LOG_ERROR, "Error, Failed to alloc buffer for frame!\n");
        return ret;
    }

    return ret;
}

int VideoDecode::VideoDecodeFrame(AVPacket *pkt, AVFrame **out_frame)
{
    int ret = -1;

    ret = avcodec_send_packet(dec_ctx, pkt);
    if (ret < 0)
    {
        av_log(NULL, AV_LOG_ERROR, "avcodec_send_packet error!\n");
        return ret;
    }

    while (ret >= 0)
    {
        ret = avcodec_receive_frame(dec_ctx, dec_frame);
        //编码器数据不足时，返回EAGAIN，或者到数据结尾时返回AVERROR_EOF
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
            break;
        if (ret < 0) {
            printf("error, failed to encode.\n");
            break;
        }
        //输出frame
        *out_frame = dec_frame;
    }
}
