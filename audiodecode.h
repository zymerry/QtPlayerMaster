#ifndef AUDIODECODE_H
#define AUDIODECODE_H

#include <string>
#include "ffmpeg_media.h"

/*
** @brief AudioEncode 音频解码类 输入packet 输出frame
** first call AudioDecodeInit() init Audio decode param and open decoder, then call AudioDecodePacket(), get a frame data.
*/
class AudioDecode
{
public:
    AudioDecode(std::string _decoder_name = "aac");
    ~AudioDecode();

    int AudioDecodeInit(int decode_ch_layout, int sample_rate, AVSampleFormat decode_format);
    int AudioDecodeDeinit();
    int AudioDecodePacket(AVPacket *src_packet, AVFrame **dst_frame);

private:
    int AudioDecodeCreateDstFrame();
    void AudioDecodeDestoryFrame();

private:
    AVCodecContext *decodec_ctx;
    AVFrame        *dec_frame;
    std::string    decoder_name{"aac"};
    int            ch_layout;
    AVSampleFormat decode_format;
};

#endif // AUDIODECODE_H
