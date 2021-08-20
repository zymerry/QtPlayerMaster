#include "audioencode.h"

AudioEncode::AudioEncode(std::string _encoder_name)
    :encoder_name(_encoder_name)
{

}

AudioEncode::~AudioEncode()
{
    avcodec_close(encodec_ctx);
    avcodec_free_context(&encodec_ctx);
}

int AudioEncode::audioEncodeInit(AVSampleFormat _encode_format, int _encode_ch_layout, int _sample_rate, int _bit_rate, int _profile)
{
    AVCodec *codec = avcodec_find_encoder_by_name(encoder_name.c_str());
    if (!codec) {
        av_log(NULL, AV_LOG_ERROR, "audio not find encoder : %s\n", encoder_name.c_str());
        return -1;
    }

    encodec_ctx = avcodec_alloc_context3(codec);
    if (!encodec_ctx) {
        av_log(NULL, AV_LOG_ERROR, "avcodec alloc ctx failed.\n");
        return -1;
    }

    audio_set_encodec_ctx(_encode_format, _encode_ch_layout, sample_rate, _bit_rate, profile);

    int ret = avcodec_open2(encodec_ctx, codec, NULL);
    if (ret != 0) {
        av_log(NULL, AV_LOG_ERROR, "avcodec open 2 failed.\n");
        return -1;
    }

    av_init_packet(&enc_packet);
    profile = _profile;
    channels = av_get_channel_layout_nb_channels(_encode_ch_layout);
    sample_rate = _sample_rate;

    return 0;
}

void AudioEncode::audio_set_encodec_ctx(AVSampleFormat encode_format, int encode_ch_layout, int sample_rate, int bit_rate, int profile)
{
    encodec_ctx->sample_fmt = encode_format;
    encodec_ctx->channel_layout = encode_ch_layout;
    encodec_ctx->sample_rate = sample_rate;
    encodec_ctx->bit_rate = bit_rate;
    encodec_ctx->profile = profile;
    encodec_ctx->channels = av_get_channel_layout_nb_channels(encode_ch_layout);
}

int AudioEncode::audioEncode(AVFrame *frame, AVPacket **out_pakcet)
{
    int ret = avcodec_send_frame(encodec_ctx, frame);
    //ret >= 0说明数据设置成功了
    while (ret >= 0) {
        ret = avcodec_receive_packet(encodec_ctx, &enc_packet);
        //要一直获取知道失败，因为可能会有好多帧需要吐出来，
        if (ret < 0) {
            //说明没有帧了，要继续送数据
            if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
                return EAGAIN;
            }
            else {
                return -1;//其他的值，说明真失败了。
            }
        }
        break;  //获取到一个packet
    }
    *out_pakcet = &enc_packet;

    return 0;
}

/* |- syncword(12) ...             | ID(v)(1)|    layer(2)  | protection_absent(1)|  (16bit)
** |- profile(2)   | private_bit(1)| sample_rate_index(4) | channel_nb(1)|(8bit) //通道数buf[2]只填了1bit,剩下的2bit在buf[3]
** | -channel_nb(2)|
*/
void AudioEncode::aac_add_header(char *aac_header, int frame_len)
{
    int sampling_frequency_index = sample_index.at(sample_rate);
    printf("add header sample rate :%d, index: %d\n", sample_rate, sampling_frequency_index);
    //sync word
    aac_header[0] = 0xff;         //syncword:0xfff                          高8bits
    aac_header[1] = 0xf0;         //syncword:0xfff                          低4bits
    //ID
    aac_header[1] |= (0 << 3);    //ID : 0 for MPEG-4 ;  1 for MPEG-2
    //layer
    aac_header[1] |= (0 << 1);    //Layer:0  always:00
    //protection absent
    aac_header[1] |= 1;           //protection absent:1    前两个字节的最低位为 1
    // also : aac_header[0] = 0xff; aac_header[1] = 0xf1;
    //profile
    aac_header[2] = (profile + 1) << 6;  //profile:profile  2bits
    //sampling_frequency_index
    aac_header[2] |= (sampling_frequency_index & 0x0f) << 2; //sampling_frequency_index 4bits 只有4bit要 &0x0f，清空高4位
    //private_bit
    aac_header[2] |= (0 << 1);        //private_bit: 0   1bits
    //channels
    aac_header[2] |= (channels & 0x04) >> 2;  //channel 高1bit: &0000 0100取channels的最高位
    aac_header[3] |= (channels & 0x03) << 6;  //&0000 0011取channels的低2位

    aac_header[3] |= (0 << 5);               //original：0                1bit
    aac_header[3] |= (0 << 4);               //home：0                    1bit
    aac_header[3] |= (0 << 3);               //copyright id bit：0        1bit
    aac_header[3] |= (0 << 2);               //copyright id start：0      1bit

    //frame len 一个ADTS帧的长度包括ADTS头和AAC原始流
    int adtsLen = frame_len + 7;
    aac_header[3] |= ((adtsLen & 0x1800) >> 11);           //frame length：value   高2bits
    aac_header[4] = (uint8_t)((adtsLen & 0x7f8) >> 3);     //frame length:value    中间8bits
    aac_header[5] = (uint8_t)((adtsLen & 0x7) << 5);       //frame length:value    低3bits
    //buffer fullness 0x7FF 说明是码率可变的码流
    aac_header[5] |= 0x1f;                                 //buffer fullness:0x7ff 高5bits
    aac_header[6] = 0xfc;      //?11111100?                  //buffer fullness:0x7ff 低6bits
    return ;
}

void AudioEncode::aac_add_header(char *aac_header, int profile, int sample_rate, int channels, int frame_len)
{
    int sampling_frequency_index = sample_index.at(sample_rate);
    printf("add header sample rate :%d, index: %d\n", sample_rate, sampling_frequency_index);
    //sync word
    aac_header[0] = 0xff;         //syncword:0xfff                          高8bits
    aac_header[1] = 0xf0;         //syncword:0xfff                          低4bits
                                  //ID
    aac_header[1] |= (0 << 3);    //ID : 0 for MPEG-4 ;  1 for MPEG-2
                                  //layer
    aac_header[1] |= (0 << 1);    //Layer:0  always:00
                                  //protection absent
    aac_header[1] |= 1;           //protection absent:1    前两个字节的最低位为 1
                                  // also : aac_header[0] = 0xff; aac_header[1] = 0xf1;
                                  //profile
    aac_header[2] = (profile) << 6;  //profile:profile  2bits
                                      //sampling_frequency_index
    aac_header[2] |= (sampling_frequency_index & 0x0f) << 2; //sampling_frequency_index 4bits 只有4bit要 &0x0f，清空高4位
                                                             //private_bit
    aac_header[2] |= (0 << 1);        //private_bit: 0   1bits
                                      //channels
    aac_header[2] |= (channels & 0x04) >> 2;  //channel 高1bit: &0000 0100取channels的最高位
    aac_header[3] |= (channels & 0x03) << 6;  //&0000 0011取channels的低2位

    aac_header[3] |= (0 << 5);               //original：0                1bit
    aac_header[3] |= (0 << 4);               //home：0                    1bit
    aac_header[3] |= (0 << 3);               //copyright id bit：0        1bit
    aac_header[3] |= (0 << 2);               //copyright id start：0      1bit

                                             //frame len 一个ADTS帧的长度包括ADTS头和AAC原始流
    int adtsLen = frame_len + 7;
    aac_header[3] |= ((adtsLen & 0x1800) >> 11);           //frame length：value   高2bits
    aac_header[4] = (uint8_t)((adtsLen & 0x7f8) >> 3);     //frame length:value    中间8bits
    aac_header[5] = (uint8_t)((adtsLen & 0x7) << 5);       //frame length:value    低3bits
                                                           //buffer fullness 0x1FF 说明是码率可变的码流
    aac_header[5] |= 0x1f;                                 //buffer fullness:0x1ff 高5bits
    aac_header[6] = 0xfc;      //111111 00                  //buffer fullness:0x1ff 低6bits
    //buffer fullness = 1FF, number_of_raw_data_blocks_in_frame = 0, 表示说ADTS帧中有一个AAC数据块
    return;
}
