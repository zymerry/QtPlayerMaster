#ifndef AUDIORESAMPLE_H
#define AUDIORESAMPLE_H

#include "ffmpeg_media.h"

/*
** @brief AudioCapture 音频重采样类
**  first call AudioResampleInit() init audio resample param
**  second call AudioResampleConvert(), get a frame pcm data after audio resample.
*/
class AudioResample
{
public:
    AudioResample(int _src_rate = 44100,
                  AVSampleFormat _src_format = AV_SAMPLE_FMT_S16,
                  int _src_ch_layout = AV_CH_LAYOUT_STEREO,
                  int _dst_rate = 44100,
                  AVSampleFormat _dst_format = AV_SAMPLE_FMT_FLTP,
                  int _dst_ch_layout = AV_CH_LAYOUT_STEREO);
    ~AudioResample();
    int AudioResampleInit();
    int AudioResampleDeinit();
    int AudioResampleConvert(AVFrame *src_frame, AVFrame **dst_frame);
private:
    int CreateDstFrame(int nb_samples);
    void DestoryDstFrame();
    int AudioResampleCreateBuffer();

private:
    //采样率
    int src_rate;
    int dst_rate;
    //采样格式
    AVSampleFormat src_format;
    AVSampleFormat dst_format;
    //通道数
    int src_ch_layout;
    int dst_ch_layout;
    //音频重采样上下文
    SwrContext *swr_ctx;
    //准备填充的数据
    uint8_t   **src_data;
    uint8_t   **dst_data;
    int         src_len;
    int         dst_len;
    AVFrame    *resample_frame {nullptr};
private:
    char error[1024] {0}; //用于错误打印
};

#endif // AUDIORESAMPLE_H
