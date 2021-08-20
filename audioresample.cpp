#include "audioresample.h"

AudioResample::AudioResample(int _src_rate,
                                 AVSampleFormat _src_format,
                                 int _src_ch_layout,
                                 int _dst_rate,
                                 AVSampleFormat _dst_format,
                                 int _dst_ch_layout)
    :src_rate(_src_rate), dst_rate(_dst_rate),
      src_format(_src_format), dst_format(_dst_format),
       src_ch_layout(_src_ch_layout), dst_ch_layout(_dst_ch_layout)
{

}

AudioResample::~AudioResample()
{

}

int AudioResample::AudioResampleInit()
{
    swr_ctx = swr_alloc_set_opts(NULL,
        dst_ch_layout, dst_format, dst_rate,
        src_ch_layout, src_format, src_rate,
        0, NULL);
    if (!swr_ctx) {
        av_log(NULL, AV_LOG_ERROR, "create swr ctx fail.\n");
        return -1;
    }
    swr_init(swr_ctx);
    AudioResampleCreateBuffer();

    return 0;
}

int AudioResample::AudioResampleDeinit()
{
    swr_free(&swr_ctx);
    return 0;
}

int AudioResample::AudioResampleConvert(AVFrame *src_frame, AVFrame **dst_frame)
{
    // 往源缓存区拷贝数据 需要判断 src_frame 是plannar 还是 packed
    int planar = av_sample_fmt_is_planar(src_format);
    if (planar) {
        int channels = av_get_channel_layout_nb_channels(src_ch_layout);
        //TODO: aac解码后src_frame 的采样个数为1024，存储格式为plannar,
        //      linesize[0] 按源码的计算方式应该为 1024 * 4 = 4096。 但这里的
        //      linesize[0] = 8192，目前不懂为什么，但是每层的数据长度是 4096， 所以这里除以了channels:2
        //      可能解码的时候源码中没有区分plannar 和 packed.
        int per_plane_len = src_frame->linesize[0] / channels;
        for (int i = 0; i < channels; i++) {
            memcpy((void *)(src_data[0] + per_plane_len * i), src_frame->data[i], per_plane_len);
        }
    }
    else {
        memcpy((void *)src_data[0], src_frame->data[0], src_frame->linesize[0]);
    }

    int nb_samples = swr_convert(swr_ctx, dst_data, 1024, (const uint8_t **)src_data, 1024);
    int dst_linesize = 0;
    int dst_bufsize = av_samples_get_buffer_size(&dst_linesize, av_get_channel_layout_nb_channels(dst_ch_layout),
        nb_samples, dst_format, 1);

    printf("convert dst_bufsize = %d, dst_linesize = %d, nb_samples = %d\n", dst_bufsize, dst_linesize, nb_samples);

    if (resample_frame == nullptr) {
        CreateDstFrame(nb_samples);
    }
    printf("convert create dst frame->linesize[0] = %d\n", resample_frame->linesize[0]);

    // 从目的缓存区 往dstFrame拷贝数据 需要判断 dstFrame 是plannar 还是 packed
    planar = av_sample_fmt_is_planar(dst_format);
    if (planar) {
        int channels = av_get_channel_layout_nb_channels(dst_ch_layout);
        int per_plane_len = resample_frame->linesize[0];
        for (int i = 0; i < channels; i++) {
            memcpy(resample_frame->data[i], dst_data[0] + per_plane_len * i, per_plane_len);
        }
    } else {
        memcpy(resample_frame->data[0], dst_data[0], resample_frame->linesize[0]);
    }

    *dst_frame = resample_frame;
    return 0;
    return 0;
}

int AudioResample::AudioResampleCreateBuffer()
{
    // 创建重采样输出缓冲区
    av_samples_alloc_array_and_samples(
        &dst_data, //输出缓冲区地址
        &dst_len, //缓冲区大小
        av_get_channel_layout_nb_channels(dst_ch_layout), //通道个数 av_get_channel_layout_nb_channels(ch_layout)
        1024,    //采样个数 4096(字节)/2(采样位数16 bit)/2通道
        dst_format, //采样格式 根据这三个就能算出来需要的总字节, 和 采集的时候 frame->data 算的长度一致
        0);

    // 创建重采样输入缓冲区
    av_samples_alloc_array_and_samples(
        &src_data, //输出缓冲区地址
        &src_len, //缓冲区大小
        av_get_channel_layout_nb_channels(src_ch_layout), //通道个数 av_get_channel_layout_nb_channels(ch_layout)
        1024,    //采样个数 4096(字节)/2(采样位数16 bit)/2通道
        src_format, //采样格式 根据这三个就能算出来需要的总字节, 和 采集的时候 frame->data 算的长度一致
        0);
    printf("create sample dst data len = %d\n", dst_len);
    printf("create sample src data len = %d\n", src_len);
    return 0;
}

int AudioResample::CreateDstFrame(int nb_samples)
{
    int ret = 0;
    resample_frame = av_frame_alloc();
    resample_frame->channel_layout = dst_ch_layout;
    resample_frame->format = dst_format;
    resample_frame->sample_rate = nb_samples;

    ret = av_frame_get_buffer(resample_frame, 0);
    if (ret != 0){
        av_strerror(ret, error, 128);
        av_log(NULL, AV_LOG_ERROR, "open input failure.[%d][%s]\n", AVERROR(ret), error);
        return ret;
    }
    return 0;
}

void AudioResample::DestoryDstFrame()
{
    av_frame_free(&resample_frame);
}
