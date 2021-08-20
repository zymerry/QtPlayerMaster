#ifndef AUDIOENCODE_H
#define AUDIOENCODE_H

#include <map>
#include <string>
#include "ffmpeg_media.h"

class AudioEncode
{
    std::map<int, int> sample_index = {
        { 96000, 0x0 }, { 88200, 0x1 }, { 64000, 0x2 }, { 48000, 0x3 }, { 44100, 0x4 }, { 32000, 0x5 },
        { 24000, 0x6 }, { 22050, 0x7 }, { 16000, 0x8 }, { 12000, 0x9 }, { 11025, 0xA }, { 8000 , 0xB }
    };

public:
    AudioEncode(std::string _encoder_name = "aac");
    ~AudioEncode();
    int  audioEncodeInit(AVSampleFormat encode_format, int encode_ch_layout, int sample_rate, int bit_rate, int profile);

    /* encode a packet */
    int audioEncode(AVFrame *frame, AVPacket **pakcet);

public:
    /* @briedf : 调用 audioEncode 编码完一帧aac 数据后， 需要调用该函数添加 adts 头
    ** @aac_buffer: 由 user 提供一个 buffer 长度为 7，头的数据填充到该 buffer 中
    ** @frame_len： audioEncode 编码后的一帧数据的长度
    */
    void aac_add_header(char *aac_buffer, int frame_len);
    void aac_add_header(char * aac_header, int profile, int sample_rate, int channels, int frame_len);

private:
    void audio_set_encodec_ctx(AVSampleFormat encode_format, int encode_ch_layout, int sample_rate, int bit_rate, int profile);

private:
    std::string     encoder_name;
    AVCodecContext  *encodec_ctx;
    AVPacket        enc_packet;
    int				profile;
    int				channels;
    int				sample_rate;
};

#endif // AUDIOENCODE_H
