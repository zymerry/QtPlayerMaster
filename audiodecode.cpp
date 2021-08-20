#include "audiodecode.h"

AudioDecode::AudioDecode(std::string _decoder_name)
    :decoder_name(_decoder_name)
{

}

AudioDecode::~AudioDecode()
{

}

int AudioDecode::AudioDecodeInit(int _decode_ch_layout, int _sample_rate, AVSampleFormat _decode_format)
{
    AVCodec *avodec = avcodec_find_decoder_by_name(decoder_name.c_str());
    if (!avodec) {
        av_log(NULL, AV_LOG_ERROR, "audio decode find decoder %s failed.\n", decoder_name.c_str());
        return -1;
    }

    decodec_ctx = avcodec_alloc_context3(avodec);
    if (!decodec_ctx) {
        av_log(NULL, AV_LOG_ERROR, "audio decode alloc decodec ctx failed.\n");
        return -1;
    }

    decodec_ctx->sample_rate = _sample_rate;
    decodec_ctx->channel_layout = _decode_ch_layout;
    if (0 != avcodec_open2(decodec_ctx, avodec, NULL)) {
        av_log(NULL, AV_LOG_ERROR, "audio decode avcodec_open2 failed.\n");
        return -1;
    }
    ch_layout = _decode_ch_layout;
    decode_format = _decode_format;

    AudioDecodeCreateDstFrame();

    return 0;
}

int AudioDecode::AudioDecodeDeinit()
{
    if (decodec_ctx) {
        avcodec_close(decodec_ctx);
        avcodec_free_context(&decodec_ctx);
    }

    AudioDecodeDestoryFrame();

    return 0;
}

int AudioDecode::AudioDecodePacket(AVPacket *src_packet, AVFrame **dst_frame)
{
    int ret = avcodec_send_packet(decodec_ctx, src_packet);
    //ret >= 0说明数据设置成功了

    if (ret >= 0) {
        ret = avcodec_receive_frame(decodec_ctx, dec_frame);
        if (ret < 0) {
            //说明没有帧了，要继续送数据
            if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
                return EAGAIN;
            }
            else {
                return -1;//其他的值，说明真失败了。
            }
        }
        //获取到一个packet
        break;
    }
    *dst_frame = dec_frame;
    av_packet_unref(src_packet);
    return 0;
}

int AudioDecode::AudioDecodeCreateDstFrame()
{
    dec_frame = av_frame_alloc();
    dec_frame->channel_layout = ch_layout;
    dec_frame->format = decode_format;
    dec_frame->nb_samples = 1024;
    int ret = av_frame_get_buffer(dec_frame, 0);
    if (ret != 0) {
        av_log(NULL, AV_LOG_ERROR, "open input failure.[%d]\n", AVERROR(ret));
        return -1;
    }
    printf("audio decode create frame size = %d\n",
        1024 * av_get_bytes_per_sample(AV_SAMPLE_FMT_FLTP) * av_get_channel_layout_nb_channels(ch_layout));
    return 0;
}

void AudioDecode::AudioDecodeDestoryFrame()
{
    av_frame_free(&dec_frame);
}
